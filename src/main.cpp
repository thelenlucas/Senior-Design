#include <sqlite3.h>
#include <string>
#include <iostream>
#include "db.hpp"
#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include "logs.hpp"
#include "types.hpp"

#include <QApplication>
#include <QPushButton>

using namespace std;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Button with label
    QPushButton button("Hello world!");
    button.resize(200, 100);
    button.show();

    Database db = Database();

    // Create a log, disconnected from a database
    /*Log::Log(int id,
        std::string species,
        uint count, uint len_quarters,
        uint diameter_quarters,
        uint cost_cents_quarters,
        uint quality,
        std::string location = "",
        std::string notes = "",
        std::optional<Database*> db = std::nullopt
    ) */
    Log log = Log(0, "Oak", 1, 4, 4, 100, 5, "A-1");

    // Insert it into the database
    db.insertLog(log);

    // Print out all logs
    vector<Log> logs = db.allLogs();
    cout << "| ID | Species | Count |" << endl;
    for (Log log : logs) {
        cout << "| " << log.getId() << " | " << log.getSpecies() << " | " << log.getCount() << " |" << endl;
    }

    return app.exec();
}
