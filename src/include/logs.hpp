#ifndef LOGDB_H
#define LOGDB_H

#include <sqlite3.h>
#include <string>
#include <iostream>
#include <stdexcept>  // For std::runtime_error

class LogDB {
public:
    // Constructor: Opens (or creates) the database.
    LogDB(const std::string& dbFile) : db_(nullptr) {
        int rc = sqlite3_open(dbFile.c_str(), &db_);
        if (rc != SQLITE_OK) {
            // Retrieve error message and close the database if necessary.
            std::string errMsg = sqlite3_errmsg(db_);
            sqlite3_close(db_);
            throw std::runtime_error("Failed to open database: " + errMsg);
        }
    }

    // Destructor: Closes the database connection.
    ~LogDB() {
        if (db_) {
            sqlite3_close(db_);
        }
    }

    // Create the 'logs' table if it doesn't exist.
    void createTable() {
        const char* sql = R"(
            CREATE TABLE IF NOT EXISTS logs (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                count INTEGER,
                length_in_inches INTEGER,
                price REAL,
                species TEXT
            );
        )";

        char* errMsg = nullptr;
        int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::string errorMsg = errMsg ? errMsg : "Unknown error";
            sqlite3_free(errMsg);
            throw std::runtime_error("Failed to create table: " + errorMsg);
        }
    }

private:
    sqlite3* db_;
};

#endif // LOGDB_H
