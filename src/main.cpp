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

#include "startup.cpp"

using qsd = QSqlDatabase;

class Container : public QThread 
{
private:
    QApplication *app;
public:
    Container(QApplication *app) 
    {
        this->app = app;
    }
public:
    void run() 
    {
        MainWindow window;
        window.show();
        app->exec();
    }
};

int main(int argc, char* argv[]) 
{

    auto db = qsd::addDatabase("QSQLITE");
    db.setDatabaseName("test.db");
    if (!db.open()) 
        qDebug() << "Failed to open database:" << db.lastError().text();

    if (!ensureSchema(db))
        qDebug() << "Failed to ensure schema:" << db.lastError().text();

    // QT SQL DEBUG stuff, will be removed eventually.
    qDebug() << "Current path:" << QDir::currentPath();
    qDebug() << "DB path:" << db.databaseName();
    qDebug() << "Available SQL drivers:" << qsd::drivers();

    QApplication app(argc, argv);

    Container *container = new Container(&app);

    container->run();
}
