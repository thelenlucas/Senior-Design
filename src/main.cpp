#ifndef BUILDING_WOODWORKS_TEST

#include <string>
#include <iostream>
#include <vector>

#include <sqlite3.h>

#include <QApplication>
#include <QPushButton>
#include <QThread>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDir>

#include "types.hpp"
#include "mainwindow.hpp"

#include "domain/log.hpp"
#include "domain/cookie.hpp"
#include "domain/live_edge_slab.hpp"
#include "domain/lumber.hpp"

#include "infra/connection.hpp"
#include "infra/repository.hpp"
#include "infra/unit_of_work.hpp"

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
    // Mock open the types so that we ensure their tables + views are created
    auto &debee = woodworks::infra::DbConnection::instance();
    woodworks::infra::QtSqlRepository<woodworks::domain::Log> logRepo(debee);
    woodworks::infra::QtSqlRepository<woodworks::domain::Cookie> cookieRepo(debee);
    woodworks::infra::QtSqlRepository<woodworks::domain::LiveEdgeSlab> liveEdgeSlabRepo(debee);
    woodworks::infra::QtSqlRepository<woodworks::domain::Lumber> lumberRepo(debee);

    auto db = qsd::addDatabase("QSQLITE");
    db.setDatabaseName("woodworks.db");
    if (!db.open()) 
        qDebug() << "Failed to open database:" << db.lastError().text();

    // QT SQL DEBUG stuff, will be removed eventually.
    qDebug() << "Current path:" << QDir::currentPath();
    qDebug() << "DB path:" << db.databaseName();
    qDebug() << "Available SQL drivers:" << qsd::drivers();

    QApplication app(argc, argv);

    Container *container = new Container(&app);

    container->run();
}

#endif