#pragma once
#include "domain/live_edge_slab.hpp"
#include "domain/units.hpp"
#include "domain/types.hpp"
#include "infra/repository.hpp"

#include <vector>
#include <optional>
#include <QString>

using namespace woodworks::domain;
using namespace woodworks::domain::imperial;
using namespace woodworks::domain::types;
using namespace woodworks::infra;

namespace woodworks::domain::lumber {

struct InProgressBoard {
    Length width;
    Length thickness;
};

struct LumberCutter {
    LiveEdgeSlab slab;
    Length trimWidth = Length::fromTicks(0);            // trim off each side
    int boardCount = 0;                                 // number of boards to produce
    Length boardWidth = Length::fromTicks(0);           // width of each board
    Length waste = Length::fromTicks(0);                // total waste after plan
    std::vector<InProgressBoard> plannedBoards;

    explicit LumberCutter(const LiveEdgeSlab& source)
        : slab(source) {}

    void setTrimWidth(Length tw) {
        trimWidth = tw;
        adjustBoardWidthOrCount();
    }

    void setBoardCount(int count) {
        boardCount = count;
        Length avail = slab.width - trimWidth * 2;
        boardWidth = (boardCount > 0 ? avail / boardCount : Length::fromTicks(0));
    }

    void setBoardWidth(Length bw) {
        boardWidth = bw;
        Length avail = slab.width - trimWidth * 2;
        boardCount = bw.toTicks() > 0 ? static_cast<int>(avail.toTicks() / bw.toTicks()) : 0;
    }

    void planCuts() {
        plannedBoards.clear();
        Length avail = slab.width - trimWidth * 2;
        waste = trimWidth * 2;
        int count = boardCount;
        Length used = Length::fromTicks(0);
        for (int i = 0; i < count; ++i) {
            plannedBoards.push_back({ boardWidth, slab.thickness });
            used = used + boardWidth;
        }
        waste = waste + (avail - used);
    }

    std::vector<QString> getPlannedBoards() const {
        std::vector<QString> list;
        for (auto& b : plannedBoards) {
            list.push_back("Width: " + QString::number(b.width.toInches()) +
                           "in, Thickness: " + QString::number(b.thickness.toInches()) + "in");
        }
        return list;
    }

    std::vector<Lumber> finalizeCuts(std::optional<std::string> location = {}, std::optional<std::string> notes = {}) {
        // Original slab worth
        int allocCentsTotal = slab.worth.toCents();
        // Each cut board is worth a fraction of the original slab worth
        // based on the width of the board (i.e. ten inch width lumber on a 100 inch slab worth 100 cents is worth 10 cents)
        int centsPerBoard = slab.worth.toCents() / plannedBoards.size();


        std::vector<Lumber> boards;
        for (auto& b : plannedBoards) {
            Lumber board = Lumber::uninitialized();
            board.species = slab.species;
            board.length = slab.length;
            board.width = b.width;
            board.thickness = b.thickness;
            board.drying = slab.drying;
            board.surfacing = static_cast<LumberSurfacing>(slab.surfacing);
            board.worth = Dollar::fromCents(centsPerBoard);

            if (location) board.location = *location;
            if (notes)    board.notes = *notes;
            board.id = Id{ QtSqlRepository<Lumber>::spawn().add(board) };
            boards.push_back(board);
        }
        plannedBoards.clear();

        // Delete the original slab
        QtSqlRepository<LiveEdgeSlab>::spawn().remove(slab.id.id);

        return boards;
    }

private:
    void adjustBoardWidthOrCount() {
        Length avail = slab.width - trimWidth * 2;
        if (boardCount > 0) {
            boardWidth = avail / boardCount;
        } else if (boardWidth.toTicks() > 0) {
            boardCount = static_cast<int>(avail.toTicks() / boardWidth.toTicks());
        }
    }
};

} // namespace woodworks::domain::lumber
