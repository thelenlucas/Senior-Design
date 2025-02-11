#include <sqlite3.h>
#include <string>
#include <iostream>
#include "db.hpp"
#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include "types.hpp"

// CREATE TABLE logs(
// id integer primary key autoincrement,
// species varchar not null,
// len_in int not null,
// diameter_in int not null,
// cost_cents_log int not null,
// quality varchar);

int main() {
    SQLite::Database testdb("test.db");
    SQLite::Statement query(testdb, "SELECT * FROM logs");

    std::vector<Log> logs;

    while (query.executeStep()) {
        int id = query.getColumn(0);
        std::string species = query.getColumn(1);
        int len_in = query.getColumn(2);
        int diameter_in = query.getColumn(3);
        int cost_cents_log = query.getColumn(4);
        std::string quality = query.getColumn(5);

        Log log(id, species, len_in, diameter_in, cost_cents_log, quality);
        logs.push_back(log);
    }

    for (Log log : logs) {
        std::cout << "ID: " << log.getId() << std::endl;
        std::cout << "Species: " << log.getSpecies() << std::endl;
        std::cout << "Length (in): " << log.getLenIn() << std::endl;
        std::cout << "Diameter (in): " << log.getDiameterIn() << std::endl;
        std::cout << "Cost (cents): " << log.getCostCentsLog() << std::endl;
        std::cout << "Quality: " << log.getQuality() << std::endl;
        std::cout << std::endl;
    }
}
