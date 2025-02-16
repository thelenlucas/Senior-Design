#include "mainwindow.h"
#include "ui_mainwindow.h" // Generated from ui/mainwindow.ui
#include "logtablemodel.hpp"
#include <string>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), 
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);  // This call builds the UI as designed in Qt Designer.

    // Create model 
    auto *model = new LogTableModel(this);

    Database db;

    // Get all logs from the database
    auto logs = db.allLogs();

    // Populate the model with logs
    model->setLogs(logs);

    // Set the model to the table view
    ui->tableView->setModel(model);

    // Resize the columns to fit the contents
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Connect the add button to the add log slot
    connect(ui->enterLogButton, &QPushButton::clicked, this, &MainWindow::onEnterLogButtonClicked);
}

void MainWindow::onEnterLogButtonClicked() {
    // Get values from the UI widgets
    QString species = ui->speciesText->text();
    int lenFt = ui->lenFt->value();
    int lenIn = ui->lenIn->value();
    int diamIn = ui->diamIn->value();
    double costVal = ui->cost->value();
    int quality = ui->quality->value();

    int lenQuarters = (lenFt * 12 + lenIn) * 4;
    int diamQuarters = diamIn * 4;

    int costCents = costVal * 100;
    int costQuarters = costCents / lenQuarters;

    // Create a log object, insert it into the database
    Database db;
    Log log(0, species.toStdString(), lenQuarters, diamQuarters, costQuarters, quality, "", "", &db);
    db.insertLog(log);

    // Refresh the table view
    auto *model = new LogTableModel(this);
    auto logs = db.allLogs();
    model->setLogs(logs);
    ui->tableView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}
