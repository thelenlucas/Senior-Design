#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include "db.hpp"

#define GROUPED_LOGS_QUERY R"(SELECT * from grouped_logs_view;)"

#define LOGS_QUERY "SELECT * FROM logs_view;"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Establish database connection.
    // Yeah this opens another connection, but it's read-only
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.db"); // Update this to your actual database path.
    if (!db.open()) {
        qDebug() << "Database error:" << db.lastError().text();
        return; // Early return if the connection fails.
    }

    auto *groupedModel = new QSqlQueryModel(this);
    auto *logsModel = new QSqlQueryModel(this);
    QString queryStr = GROUPED_LOGS_QUERY;
    QString logsQueryStr = LOGS_QUERY;
    groupedModel->setQuery(queryStr, db);
    logsModel->setQuery(logsQueryStr, db);

    // Check for query errors.
    if (groupedModel->lastError().isValid()) {
        qDebug() << "Query error:" << groupedModel->lastError().text();
    }
    if (logsModel->lastError().isValid()) {
        qDebug() << "Query error:" << logsModel->lastError().text();
    }

    // Set the model to the table view.
    ui->groupedLogsTableView->setModel(groupedModel);
    ui->individualLogTableView->setModel(logsModel);
    refreshModel();

    // Resize the columns to fit the contents.
    ui->groupedLogsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->individualLogTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Connect the add button to the add log slot.
    // Note: With a query model displaying aggregated data, editing is not supported.
    connect(ui->enterLogButton, &QPushButton::clicked, this, &MainWindow::onEnterLogButtonClicked);
    connect(ui->scrapLogButton, &QPushButton::clicked, this, &MainWindow::onScrapLogButtonClicked);
}

void MainWindow::refreshModel()
{
    // Get the current model from the table view.
    auto *groupedModel = qobject_cast<QSqlQueryModel*>(ui->groupedLogsTableView->model());
    if (!groupedModel)
        return;
    auto *logsModel = qobject_cast<QSqlQueryModel*>(ui->individualLogTableView->model());

    // Define the query string (could also be a member variable)
    QString queryStr = GROUPED_LOGS_QUERY;
    QString logsQueryStr = LOGS_QUERY;

    // Re-run the query.
    groupedModel->setQuery(queryStr, QSqlDatabase::database());
    logsModel->setQuery(logsQueryStr, QSqlDatabase::database());

    // Optional: Check for errors.
    if (groupedModel->lastError().isValid()) {
        qDebug() << "Query error:" << groupedModel->lastError().text();
    }
    if (logsModel->lastError().isValid()) {
        qDebug() << "Query error:" << logsModel->lastError().text();
    }

    // Set column headers and resize columns.
    groupedModel->setHeaderData(0, Qt::Horizontal, "Species");
    groupedModel->setHeaderData(1, Qt::Horizontal, "Length");
    groupedModel->setHeaderData(2, Qt::Horizontal, "Diameter");
    groupedModel->setHeaderData(3, Qt::Horizontal, "Count");

    logsModel->setHeaderData(0, Qt::Horizontal, "Species");
    logsModel->setHeaderData(1, Qt::Horizontal, "Length");
    logsModel->setHeaderData(2, Qt::Horizontal, "Diameter");
    logsModel->setHeaderData(3, Qt::Horizontal, "Quality");
    logsModel->setHeaderData(4, Qt::Horizontal, "Location");
    logsModel->setHeaderData(5, Qt::Horizontal, "Value");
    logsModel->setHeaderData(6, Qt::Horizontal, "ID");
}

void MainWindow::onEnterLogButtonClicked() {
    // Get values from the UI widgets
    QString species = ui->speciesText->text();
    int lenFt = ui->lenFt->value();
    int lenIn = ui->lenIn->value();
    int diamIn = ui->diamIn->value();
    double costVal = ui->cost->value();
    int quality = ui->quality->value();
    string location = ui->locationEntry->text().toStdString();

    int lenQuarters = (lenFt * 12 + lenIn) * 4;
    int diamQuarters = diamIn * 4;

    int costCents = costVal * 100;
    int costQuarters = costCents / lenQuarters;

    // Create a log object, insert it into the database
    Log log(0, species.toStdString(), lenQuarters, diamQuarters, costQuarters, quality, location);
    log.insert();

    // Refresh the model
    refreshModel();
}

void MainWindow::onScrapLogButtonClicked() {
    // Get the selected log ID
    QModelIndex index = ui->individualLogTableView->currentIndex();
    Log log = Log::fromID(index.sibling(index.row(), 6).data().toInt());
    
    // Confirm with the user if the log should be deleted, if that log
    // is being actively used in a cutlist
    if (log.isActivelyUsed()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Log in use", "This log is actively used in a cutlist. Are you sure you want to scrap it?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
    }

    // Delete the log
    log.remove();

    // Refresh the model
    refreshModel();
}

MainWindow::~MainWindow()
{
    delete ui;
}
