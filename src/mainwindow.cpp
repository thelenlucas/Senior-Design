#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include "project_editor.h"
#include <string>
#include "logs.hpp"
#include "types.hpp"
#include "firewood.hpp"
#include <iostream>
#include <iomanip>

#define GROUPED_LOGS_QUERY "SELECT * from logs_view_grouped"
#define LOGS_QUERY "SELECT * FROM logs_view"

/* QT MainWindow C++ File: 
 *  - Includes all ties to the main-window UI and its operation 
*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Establish database connection.
    // Yeah this opens another connection, but it's read-only
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DATABASE_FILE);
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
    connect(ui->makeFirewoodButton, &QPushButton::clicked, this, &MainWindow::onFirewoodButtonClicked);
    connect(ui->projectsEditorAction, &QAction::triggered, this, &MainWindow::onProjectEditActionTriggered);
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
}

void MainWindow::onEnterLogButtonClicked() {
    // Get values from the UI widgets
    QString species = ui->speciesText->text();
    int lenFt = ui->lenFt->value();
    int lenIn = ui->lenIn->value();
    int diamIn = ui->diamIn->value();
    double costVal = ui->cost->value();
    int quality = ui->quality->value();
    std::string location = ui->locationEntry->text().toStdString();

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
    std::optional<Log> log = Log::get_by_id(index.sibling(index.row(), 0).data().toInt());

    // If the log is not found, return, after displaying an error message
    if (!log) {
        QMessageBox::critical(this, "Error", "Log not found");
        return;
    }

    // Scrap the log
    log->scrap();

    // Update the model
    refreshModel();
}

void MainWindow::onFirewoodButtonClicked() {
    // Get the selected log ID, and get log object
    std::optional<Log> opt = Log::get_by_id(ui->individualLogTableView->currentIndex().siblingAtColumn(0).data().toInt());

    if (!opt) {
        QMessageBox::critical(this, "Error", "Log not found");
        return;
    }

    // Confirm that they want to turn the log into firewood
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm", "Are you sure you want to turn this entire log into firewood?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }

    std::cout << "Turning log into firewood" << std::endl;

    // We're going to convert the entire usable length of the log into firewood
    Log log = opt.value();
    int usableLength = log.getAvailableLength();

    // Manufacture the log into firewood
    auto firewood = Firewood::make_from_log(log, usableLength);

    refreshModel();
}

void MainWindow::onProjectEditActionTriggered() {
   ProjectEditorWindow *projectEditor = new ProjectEditorWindow(this);
   projectEditor->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
