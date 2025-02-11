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
    Database db = Database();

    // Some example logs
    // Log log1 = Log(1, "Pine", 10, 5, 100, "Good", "A-1");
    // Log log2 = Log(2, "Oak", 15, 6, 200, "Bad", "A-2");
    // Log log3 = Log(3, "Maple", 20, 7, 300, "Good", "C-3");
    // db.insertLog(log1);
    // db.insertLog(log2);
    // db.insertLog(log3);

    std::vector<Log> logs = db.allLogs();

    for (Log log : logs) {
        std::cout << "ID: " << log.getId() << std::endl;
        std::cout << "Species: " << log.getSpecies() << std::endl;
        std::cout << "Length (in): " << log.getLenIn() << std::endl;
        std::cout << "Diameter (in): " << log.getDiameterIn() << std::endl;
        std::cout << "Cost (cents): " << log.getCostCentsLog() << std::endl;
        std::cout << "Quality: " << log.getQuality() << std::endl;
        std::cout << "Location: " << log.getLocation() << std::endl;
        std::cout << std::endl;
    }
}
