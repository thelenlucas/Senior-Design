#ifndef BUILDING_WOODWORKS_TEST

#include <string>
#include <iostream>
#include <vector>

#include <QApplication>
#include <QPushButton>
#include <QThread>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDir>
#include <QSqlQuery>

#include "mainwindow.hpp"

#include "domain/log.hpp"
#include "domain/cookie.hpp"
#include "domain/live_edge_slab.hpp"
#include "domain/lumber.hpp"
#include "domain/cutlist.hpp"

#include "infra/connection.hpp"
#include "infra/repository.hpp"
#include "infra/unit_of_work.hpp"

#include "inventory.hpp"

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

    QApplication app(argc, argv);
    
    // Mock open the types so that we ensure their tables + views are created
    auto &debee = woodworks::infra::DbConnection::instance();

    // Delete all existing views
    {
        QSqlQuery query(debee);
        query.exec("SELECT name FROM sqlite_master WHERE type='view'");
        while(query.next()) {
            QString viewName = query.value(0).toString();
            QSqlQuery dropQuery(debee);
            if(!dropQuery.exec(QString("DROP VIEW IF EXISTS %1").arg(viewName))) {
                qWarning() << "Failed to drop view" << viewName << ":" << dropQuery.lastError().text();
            }
        }
    }

    woodworks::infra::QtSqlRepository<woodworks::domain::Log> logRepo(debee);
    woodworks::infra::QtSqlRepository<woodworks::domain::Cookie> cookieRepo(debee);
    woodworks::infra::QtSqlRepository<woodworks::domain::LiveEdgeSlab> liveEdgeSlabRepo(debee);
    woodworks::infra::QtSqlRepository<woodworks::domain::Lumber> lumberRepo(debee);
    woodworks::infra::QtSqlRepository<woodworks::domain::Firewood> firewoodRepo(debee);
    woodworks::infra::QtSqlRepository<woodworks::domain::CustomCut> customCutRepo(debee);

    MainWindow window;
    window.show();

    return app.exec();
}

#endif
