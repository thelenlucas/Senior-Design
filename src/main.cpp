#include <sqlite3.h>
#include <string>
#include <iostream>
#include "db.hpp"
#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include "logs.hpp"
#include "types.hpp"

#include "mainwindow.h"

#include <QApplication>
#include <QPushButton>

using namespace std;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Button with label
    MainWindow window;
    window.show();

    Database db;

    // Create a log, disconnected from a database
    Log log = Log(0, "Oak", 4, 4, 100, 5, "A-1");

    // Insert it into the database
    db.insertLog(log);

    // Print out all logs
    vector<Log> logs = db.allLogs();
    cout << "| ID | Species | Length | Diameter | Cost | Quality | Location | Notes |" << endl;
    for (Log log : logs) {
        cout << "| " << log.getId() << " | " << log.getSpecies() << " | " << log.getLenQuarters() << " | " << log.getDiameterQuarters() << " | " << log.getCostCentsQuarters() << " | " << log.getQuality() << " | " << log.getLocation() << " | " << log.getNotes() << " |" << endl;
    }

    return app.exec();
}
