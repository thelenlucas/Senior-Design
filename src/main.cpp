#include <qapplication.h>
#include <qwidget.h>
#include <string>
#include <iostream>
#include <vector>

#include <sqlite3.h>
#include <SQLiteCpp/SQLiteCpp.h>

#include <QApplication>
#include <QWidget>

#include "db.hpp"
#include "logs.hpp"
#include "main_window.hpp"

using std::cout, std::endl, std::vector;

int main(int argc, char** argv) {
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

    QApplication logdb_application(argc, argv);
    MainWindow logdb_main_window;

    logdb_main_window.show();
    return logdb_application.exec();
}
