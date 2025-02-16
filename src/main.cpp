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

    return app.exec();
}
