#ifndef DATABASE_H
#define DATABASE_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include "types.hpp"
#include <iostream>

using namespace std;

#define DATABASE_FILE "test.db"
#define LOGS_TABLE "logs"

#define DB_LOGGING false

class Database {
private:
    SQLite::Database db;

public:
    Database() : db(DATABASE_FILE, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
        db.exec("CREATE TABLE IF NOT EXISTS " LOGS_TABLE "("
            "id integer primary key autoincrement,"
            "species varchar not null,"
            "len_in int not null,"
            "diameter_in int not null,"
            "cost_cents_log int not null,"
            "quality varchar,"
            "location varchar);");

        if (DB_LOGGING) {
            cout << "Database initialized" << endl;
        }
    }

    // Gets a singular log from the database, given an ID
    Log getLog(int id) {
        SQLite::Statement query(db, "SELECT * FROM " LOGS_TABLE " WHERE id = ?");

        query.bind(1, id);

        if (DB_LOGGING) cout << "Getting log with id " << id << endl;

        if (query.executeStep()) {
            int id = query.getColumn(0);
            std::string species = query.getColumn(1);
            int len_in = query.getColumn(2);
            int diameter_in = query.getColumn(3);
            int cost_cents_log = query.getColumn(4);
            std::string quality = query.getColumn(5);
            std::string location = query.getColumn(6);

            return Log(id, species, len_in, diameter_in, cost_cents_log, quality, location);
        } else {
            throw std::invalid_argument("Log not found");
        }
    }

    // Gets all logs from the database
    std::vector<Log> allLogs() {
        if (DB_LOGGING) cout << "Getting all logs" << endl;

        SQLite::Statement query(db, "SELECT * FROM " LOGS_TABLE);

        std::vector<Log> logs;

        while (query.executeStep()) {
            int id = query.getColumn(0);
            std::string species = query.getColumn(1);
            int len_in = query.getColumn(2);
            int diameter_in = query.getColumn(3);
            int cost_cents_log = query.getColumn(4);
            std::string quality = query.getColumn(5);
            std::string location = query.getColumn(6);

            Log log = Log(id, species, len_in, diameter_in, cost_cents_log, quality, location);
            logs.push_back(log);
        }

        return logs;
    }

    // Puts a new log into the database
    void insertLog(Log log) {
        SQLite::Statement query(db, "INSERT INTO " LOGS_TABLE "(species, len_in, diameter_in, cost_cents_log, quality, location) VALUES (?, ?, ?, ?, ?, ?)");

        query.bind(1, log.getSpecies());
        query.bind(2, log.getLenIn());
        query.bind(3, log.getDiameterIn());
        query.bind(4, log.getCostCentsLog());
        query.bind(5, log.getQuality());
        query.bind(6, log.getLocation());

        query.exec();
    }

    // Adds multiple logs to the database
    void insertLogs(std::vector<Log> logs) {
        for (Log log : logs) {
            insertLog(log);
        }
    }

    // Updates a log in the database
    void updateLog(Log log) {
        SQLite::Statement query(db, "UPDATE " LOGS_TABLE " SET species = ?, len_in = ?, diameter_in = ?, cost_cents_log = ?, quality = ?, location = ? WHERE id = ?");

        query.bind(1, log.getSpecies());
        query.bind(2, log.getLenIn());
        query.bind(3, log.getDiameterIn());
        query.bind(4, log.getCostCentsLog());
        query.bind(5, log.getQuality());
        query.bind(6, log.getId());
        query.bind(7, log.getLocation());

        query.exec();
    }

    // Updates multiple logs in the database
    void updateLogs(std::vector<Log> logs) {
        for (Log log : logs) {
            updateLog(log);
        }
    }

    // Deletes a log from the database
    void deleteLog(Log log) {
        SQLite::Statement query(db, "DELETE FROM " LOGS_TABLE " WHERE id = ?");

        query.bind(1, log.getId());

        query.exec();
    }
};

#endif // DATABASE_H
