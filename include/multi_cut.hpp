#pragma once
/**
 * @file multi_cut.hpp
 * @brief Helper class for objects that can have multiple objects tied to a single cut
 */

#include <vector>
#include <string>
#include "manufacturable.hpp"
#include "SQLiteCpp/SQLiteCpp.h"

#include "interfaces.hpp"

// CREATE TABLE partial_cuts (
//     id                INTEGER PRIMARY KEY AUTOINCREMENT
//                               NOT NULL
//                               UNIQUE,
//     from_log          INTEGER REFERENCES logs (id) 
//                               NOT NULL,
//     len_quarters      INTEGER NOT NULL
//                               CHECK ( (len_quarters > 0) ),
//     num_products_made INTEGER NOT NULL
//                               DEFAULT ( (1) ) 
// );

namespace {
    constexpr const char* INSERT_SQL_MULTI =
        "INSERT INTO partial_cuts (from_log, len_quarters, num_products_made) "
        "VALUES (?, ?, ?);";
    
    constexpr const char* UPDATE_SQL_MULTI =
        "UPDATE partial_cuts SET from_log = ?, len_quarters = ?, num_products_made = ? WHERE id = ?;";

    constexpr const char* SELECT_ONE_SQL_MULTI =
        "SELECT id, from_log, len_quarters, num_products_made "
        "FROM partial_cuts WHERE id = ?;";

    constexpr const char* SELECT_ALL_SQL_MULTI = 
        "SELECT id, from_log, len_quarters, num_products_made "
        "FROM partial_cuts;";
}

class MultiCut {
    public:
        MultiCut() = default;
        MultiCut(int id, int from_log_id, int len_quarters, int num_products_made)
            : id_{id}, from_log_id_{from_log_id}, len_quarters_{len_quarters}, num_products_made_{num_products_made} {}

        [[nodiscard]] int get_id() const noexcept { return id_; }
        [[nodiscard]] int get_from_log_id() const noexcept { return from_log_id_; }
        [[nodiscard]] int get_len_quarters() const noexcept { return len_quarters_; }
        [[nodiscard]] int get_num_products_made() const noexcept { return num_products_made_; }

        bool insert() {
            try {
                SQLite::Database db{"test.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
                SQLite::Statement stmt{db, INSERT_SQL_MULTI};

                stmt.bind(1, from_log_id_);
                stmt.bind(2, len_quarters_);
                stmt.bind(3, num_products_made_);

                if (stmt.exec() > 0) {
                    id_ = static_cast<int>(db.getLastInsertRowid());
                    return true;
                }
            } catch (const SQLite::Exception& e) {
                std::cerr << "SQLite error (insert): " << e.what() << "\n";
            }
            return false;
        }
        bool update() {
            try {
                SQLite::Database db{"test.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
                SQLite::Statement stmt{db, UPDATE_SQL_MULTI};

                stmt.bind(1, from_log_id_);
                stmt.bind(2, len_quarters_);
                stmt.bind(3, num_products_made_);
                stmt.bind(4, id_);

                return (stmt.exec() > 0);
            } catch (const SQLite::Exception& e) {
                std::cerr << "SQLite error (update): " << e.what() << "\n";
            }
            return false;
        }
        static std::optional<MultiCut> get_by_id(int id);
        static std::vector<MultiCut> get_all();

private:
    int id_{-1};
    int from_log_id_{-1};
    int len_quarters_{0};
    int num_products_made_{1};
};