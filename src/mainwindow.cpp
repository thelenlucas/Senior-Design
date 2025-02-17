#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>
#include "db.hpp"

#define QUERY_STRING_LOGS R"(
    SELECT 
        species, 
        CONCAT(
            FLOOR(len_quarters / 48), 'ft ', 
            FLOOR((len_quarters % 48) / 4), 'in'
        ) AS length,
        CONCAT(FLOOR(diameter_quarters / 4), 'in') AS diameter_inches,
        location, 
        COUNT(*) AS log_count
    FROM logs
    GROUP BY species, len_quarters, diameter_quarters, location
)";

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

    auto *model = new QSqlQueryModel(this);
    QString queryStr = QUERY_STRING_LOGS;
    model->setQuery(queryStr, db);

    // Check for query errors.
    if (model->lastError().isValid()) {
        qDebug() << "Query error:" << model->lastError().text();
    }

    // Set the model to the table view.
    ui->tableView->setModel(model);
    refreshModel();

    // Resize the columns to fit the contents.
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Connect the add button to the add log slot.
    // Note: With a query model displaying aggregated data, editing is not supported.
    connect(ui->enterLogButton, &QPushButton::clicked, this, &MainWindow::onEnterLogButtonClicked);
}

void MainWindow::refreshModel()
{
    // Get the current model from the table view.
    auto *model = qobject_cast<QSqlQueryModel*>(ui->tableView->model());
    if (!model)
        return;

    // Define the query string (could also be a member variable)
    QString queryStr = QUERY_STRING_LOGS;

    // Re-run the query.
    model->setQuery(queryStr, QSqlDatabase::database());

    // Optional: Check for errors.
    if (model->lastError().isValid()) {
        qDebug() << "Query error:" << model->lastError().text();
    }

    // Set column headers and resize columns.
    model->setHeaderData(0, Qt::Horizontal, "Species");
    model->setHeaderData(1, Qt::Horizontal, "Length");
    model->setHeaderData(2, Qt::Horizontal, "Diameter (in)");
    model->setHeaderData(3, Qt::Horizontal, "Location");
    model->setHeaderData(4, Qt::Horizontal, "Log Count");
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
    Database db;
    Log log(0, species.toStdString(), lenQuarters, diamQuarters, costQuarters, quality, location, "", &db);
    db.insertLog(log);

    // Refresh the model
    refreshModel();
}

MainWindow::~MainWindow()
{
    delete ui;
}
