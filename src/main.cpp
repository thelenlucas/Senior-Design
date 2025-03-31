#include <string>
#include <iostream>
#include <vector>

#include <sqlite3.h>
#include <SQLiteCpp/SQLiteCpp.h>

#include <QApplication>
#include <QPushButton>
#include <QThread>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDir>

#include "logs.hpp"
#include "types.hpp"
#include "mainwindow.hpp"

using qsd = QSqlDatabase;

int main(int argc, char* argv[]) 
{
    QApplication app(argc, argv);

    qsd db = qsd::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");
    if (!db.open()) 
        qDebug() << "Failed to open database:" << db.lastError().text();

    // Debug info
    qDebug() << "Current path:" << QDir::currentPath();
    qDebug() << "DB path:" << db.databaseName();
    qDebug() << "Available SQL drivers:" << qsd::drivers();

    // Create and show main window
    MainWindow window;
    window.show();

    // Run Qt's main event loop — THIS is critical!
    return app.exec();
}
