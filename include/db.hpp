#ifndef DATABASE_H
#define DATABASE_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include "logs.hpp"
#include <iostream>
#include <optional>

using namespace std;

#define DATABASE_FILE "test.db"
#define LOGS_TABLE "logs"

#define DB_LOGGING true

class Log;

class Database {
private:
    SQLite::Database db;

public:
    Database() : db(DATABASE_FILE, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
        db.exec("CREATE TABLE IF NOT EXISTS " LOGS_TABLE " ("
                "id                  INTEGER PRIMARY KEY AUTOINCREMENT,"
                "species             VARCHAR NOT NULL,"
                "len_quarters        INT     NOT NULL CHECK ( (len_quarters > 0) ),"
                "diameter_quarters   INT     NOT NULL CHECK ( (diameter_quarters > 0) ),"
                "cost_cents_quarters INT     NOT NULL CHECK ( (cost_cents_quarters > 0) ),"
                "quality             INTEGER CHECK ( (quality BETWEEN 1 AND 5) ),"
                "location            VARCHAR,"
                "notes               TEXT"
                ");");

        if (DB_LOGGING) {
            cout << "Database initialized" << endl;
        }
    }

    // Gets a singular log from the database, given an ID
    Log getLog(uint id);

    // Finds a log that matches the given log
    std::optional<Log> findLog(Log log);

    // Gets all logs from the database
    std::vector<Log> allLogs();

    // Puts a new log into the database, returns the log that
    // this log insertion refers to, could be new or an updated
    // extant log
    Log insertLog(Log log);

    // Adds multiple logs to the database
    void insertLogs(std::vector<Log> logs);

    // Updates a log in the database
    void updateLog(Log log);

    // Updates multiple logs in the database
    void updateLogs(std::vector<Log> logs);

    // Deletes a log from the database
    void deleteLog(Log log);
};

#endif // DATABASE_H
