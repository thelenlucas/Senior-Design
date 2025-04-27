/**
 * @file lumber_cutter.hpp
 * @brief Defines the LumberCutter class and related structures for processing live edge slabs into boards.
 */

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

/**
 * @struct InProgressBoard
 * @brief Represents a board that is in the process of being cut.
 */
struct InProgressBoard {
    Length width;
    Length thickness;
};

/**
 * @struct LumberCutter
 * @brief Handles the process of cutting a live edge slab into boards.
 */
struct LumberCutter {
    /**
     * @var LumberCutter::slab
     * @brief The live edge slab to be processed.
     */
    LiveEdgeSlab slab;
    /**
     * @var LumberCutter::trimWidth
     * @brief The width to trim off each side of the slab.
     */
    Length trimWidth = Length::fromTicks(0);            // trim off each side
    /**
     * @var LumberCutter::boardCount
     * @brief The number of boards to produce.
     */
    int boardCount = 0;                                 // number of boards to produce
    /**
     * @var LumberCutter::boardWidth
     * @brief The width of each board.
     */
    Length boardWidth = Length::fromTicks(0);           // width of each board
    /**
     * @var LumberCutter::waste
     * @brief The total waste after planning the cuts.
     */
    Length waste = Length::fromTicks(0);                // total waste after plan
    /**
     * @var LumberCutter::plannedBoards
     * @brief A list of boards planned to be cut from the slab.
     */
    std::vector<InProgressBoard> plannedBoards;

    /**
     * @brief Constructs a LumberCutter with the given live edge slab.
     * @param source The live edge slab to be processed.
     */
    explicit LumberCutter(const LiveEdgeSlab& source)
        : slab(source) {}

    /**
     * @brief Sets the trim width and adjusts the board width or count accordingly.
     * @param tw The trim width to set.
     */
    void setTrimWidth(Length tw) {
        trimWidth = tw;
        adjustBoardWidthOrCount();
    }

    /**
     * @brief Sets the number of boards to produce and adjusts the board width accordingly.
     * @param count The number of boards to produce.
     */
    void setBoardCount(int count) {
        boardCount = count;
        Length avail = slab.width - trimWidth * 2;
        boardWidth = (boardCount > 0 ? avail / boardCount : Length::fromTicks(0));
    }

    /**
     * @brief Sets the width of each board and adjusts the board count accordingly.
     * @param bw The width of each board.
     */
    void setBoardWidth(Length bw) {
        boardWidth = bw;
        Length avail = slab.width - trimWidth * 2;
        boardCount = bw.toTicks() > 0 ? static_cast<int>(avail.toTicks() / bw.toTicks()) : 0;
    }

    /**
     * @brief Plans the cuts for the boards based on the current settings.
     */
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

    /**
     * @brief Retrieves a list of planned boards as strings.
     * @return A vector of QStrings describing the planned boards.
     */
    std::vector<QString> getPlannedBoards() const {
        std::vector<QString> list;
        for (auto& b : plannedBoards) {
            list.push_back("Width: " + QString::number(b.width.toInches()) +
                           "in, Thickness: " + QString::number(b.thickness.toInches()) + "in");
        }
        return list;
    }

    /**
     * @brief Finalizes the cuts and creates the boards.
     * @param location Optional location for the boards.
     * @param notes Optional notes for the boards.
     * @return A vector of finalized Lumber objects.
     */
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
    /**
     * @brief Adjusts the board width or count based on the available slab width and trim width.
     */
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
