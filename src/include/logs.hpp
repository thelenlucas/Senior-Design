#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include "types.hpp"
#include <vector>

class Database {
public:
    // Constructor: Opens (or creates) the database.
    Database(const std::string& dbFile) : db_(nullptr) {
        int rc = sqlite3_open(dbFile.c_str(), &db_);
        if (rc != SQLITE_OK) {
            std::string errMsg = sqlite3_errmsg(db_);
            sqlite3_close(db_);
            throw std::runtime_error("Failed to open database: " + errMsg);
        }
    }

    // Destructor: Closes the database connection.
    ~Database() {
        if (db_) {
            sqlite3_close(db_);
        }
    }

    // Executes a given SQL command and throws a runtime_error on failure.
    void executeSQL(const std::string& sql) {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::string errorMsg = errMsg ? errMsg : "Unknown error";
            sqlite3_free(errMsg);
            throw std::runtime_error("SQL execution error: " + errorMsg);
        }
    }

    // Create the 'logs' table if it doesn't exist.
    void createLogsTable() {
        const std::string sql = R"(
            CREATE TABLE IF NOT EXISTS logs (
                serial_number INTEGER PRIMARY KEY AUTOINCREMENT,
                location TEXT
                length_inches INTEGER,
                diameter_inches INTEGER,
                price REAL,
                species TEXT,
            );
        )";

        executeSQL(sql);
    }

    // Create the 'boards' table if it doesn't exist.
    void createBoardsTable() {
        const std::string sql = R"(
            CREATE TABLE IF NOT EXISTS boards (
                serial_number INTEGER PRIMARY KEY AUTOINCREMENT,
                in_stock INTEGER,
                location TEXT,
                species TEXT,
                length_inches INTEGER,
                thickness_quarters INTEGER,
                price_cents_inch INTEGER,
            );
        )";

        executeSQL(sql);
    }

    // Insert a log object into the 'logs' table.
    void insertLog(const Log log) {
        const std::string sql = "INSERT INTO logs (length_inches, price, species, surface, location) VALUES ("
            + std::to_string(log.getLengthInches()) + ", "
            + std::to_string(log.getPrice()) + ", '"
            + log.getSpecies() + "', '"
            + log.getSurface() + "', '"
            + log.getLocation() + "');";

        executeSQL(sql);
    }

    // Insert a board object into the 'boards' table.
    void insertBoard(const Board board) {
        const std::string sql = "INSERT INTO boards (in_stock, location, species, length_inches, thickness_quarters, price_cents_inch) VALUES ("
            + std::to_string(board.getInStock()) + ", '"
            + board.getLocation() + "', '"
            + board.getSpecies() + "', "
            + std::to_string(board.getLengthInches()) + ", "
            + std::to_string(board.getThicknessQuarters()) + ", "
            + std::to_string(board.getPriceCentsInch()) + ");";

        executeSQL(sql);
    }

    // Get all logs from the 'logs' table as a vector of Log objects.
    std::vector<Log> getLogs() {
        std::vector<Log> logs;
        const std::string sql = "SELECT * FROM logs;";
        sqlite3_stmt* stmt = nullptr;

        int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare SQL statement.");
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            int serial_number = sqlite3_column_int(stmt, 0);
            int length = sqlite3_column_int(stmt, 1);
            double price = sqlite3_column_double(stmt, 2);
            std::string species = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            std::string surface = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            std::string location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

            logs.push_back(Log(serial_number, length, price, species, surface, location));
        }

        sqlite3_finalize(stmt);
        return logs;
    }

private:
    sqlite3* db_;
};

#endif // DATABASE_H
