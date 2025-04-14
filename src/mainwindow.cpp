#include <string>
#include <iostream>
#include <iomanip>

#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QDockWidget>
#include <QSqlRecord>
#include <QScreen>

#include "project_editor.hpp"
#include "logs.hpp"
#include "types.hpp"
#include "firewood.hpp"

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "inventory.hpp"
#include "cutlist.hpp"
#include "sales.hpp"

#define GROUPED_LOGS_QUERY "SELECT * from logs_view_grouped"
#define LOGS_QUERY "SELECT * FROM logs_view"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      cutlistPage(new CutlistPage()),
      salesPage(new SalesPage()),
      inventoryPage(nullptr)
{
    // Load UI layout and set the central widget
    ui->setupUi(this);
    setCentralWidget(ui->centralwidget);

    // Enable docking options globally for the main window
    setDockOptions(QMainWindow::AnimatedDocks |
                   QMainWindow::AllowNestedDocks |
                   QMainWindow::AllowTabbedDocks);

    // Add the menu and actions for switching between pages
    QMenu *menu = menuBar()->addMenu("WoodWorks");
    QAction *inventoryAction = new QAction("Inventory", this);
    QAction *cutlistAction = new QAction("Cutlist", this);
    QAction *salesAction = new QAction("Sales", this);

    menu->addAction(inventoryAction);
    menu->addAction(cutlistAction);
    menu->addAction(salesAction);

    // Connect action triggers to handlers
    connect(inventoryAction, &QAction::triggered, this, &MainWindow::showInventoryPage);
    connect(cutlistAction, &QAction::triggered, this, &MainWindow::showCutlistPage);
    connect(salesAction, &QAction::triggered, this, &MainWindow::showSalesPage);

    // Set up the database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DATABASE_FILE);
    if (!db.open()) {
        qDebug() << "Database error:" << db.lastError().text();
        return;
    }

    // Load and bind models
    auto *groupedModel = new QSqlQueryModel(this);
    auto *logsModel = new QSqlQueryModel(this);
    groupedModel->setQuery(GROUPED_LOGS_QUERY, db);
    logsModel->setQuery(LOGS_QUERY, db);

    if (groupedModel->lastError().isValid())
        qDebug() << "Query error:" << groupedModel->lastError().text();
    if (logsModel->lastError().isValid())
        qDebug() << "Query error:" << logsModel->lastError().text();

    ui->groupedLogsTableView->setModel(groupedModel);
    ui->individualLogTableView->setModel(logsModel);
    refreshModel();

    ui->groupedLogsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->individualLogTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->enterLogButton, &QPushButton::clicked, this, &MainWindow::onEnterLogButtonClicked);
    connect(ui->scrapLogButton, &QPushButton::clicked, this, &MainWindow::onScrapLogButtonClicked);
    connect(ui->makeFirewoodButton, &QPushButton::clicked, this, &MainWindow::onFirewoodButtonClicked);
    connect(ui->individualLogTableView, &QTableView::doubleClicked, this, &MainWindow::onTableCellDoubleClicked);
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

void MainWindow::onTableCellDoubleClicked() {
    //get currently selected row
    QTableView *shownTable;
    shownTable = ui->individualLogTableView;
    int tableIndex = shownTable->currentIndex().row();

    //query database for photo of log at selected index
    QString imageQueryStr = "SELECT * FROM logs";
    QSqlQueryModel *imageModel = new QSqlQueryModel(this);
    imageModel->setQuery(imageQueryStr, QSqlDatabase::database());
    QByteArray imageData = imageModel->record(tableIndex).value("media").toByteArray();

    //turn binary data into image data
    QPixmap *imagePixmap = new QPixmap();
    imagePixmap->loadFromData(imageData);

    //open a new window & display image
    QMainWindow *imageWindow = new QMainWindow(this);
    QLabel *imageLabel = new QLabel(imageWindow);
    if (imagePixmap->width() > QGuiApplication::primaryScreen()->availableGeometry().width()) {
        *imagePixmap = imagePixmap->scaledToWidth(QGuiApplication::primaryScreen()->availableGeometry().width() - 100);
    }
    else if(imagePixmap->height() > QGuiApplication::primaryScreen()->availableGeometry().height()) {
        *imagePixmap = imagePixmap->scaledToHeight(QGuiApplication::primaryScreen()->availableGeometry().height() - 100);
    }
    imageLabel->setPixmap(*imagePixmap);
    imageLabel->adjustSize();
    imageWindow->resize(imageLabel->size());
    QString imageWindowTitle = "Log #";
    int logID = imageModel->record(tableIndex).value("ID").toInt();
    imageWindowTitle.append(QString::number(logID));
    imageWindow->setWindowTitle(imageWindowTitle);
    imageWindow->show();
}

void MainWindow::onProjectEditActionTriggered() {
   ProjectEditorWindow *projectEditor = new ProjectEditorWindow(this);
   projectEditor->show();
}

MainWindow::~MainWindow() 
{
    delete ui;
    // TODO: Remove these after testing. This is actually double deleting because
    // of the QT parenting system, these are effectively shared pointers in the main window
    // and will be cleaned up by scope resolution.
    //delete inventoryPage;
    //delete cutlistPage;
    //delete salesPage;
}

void MainWindow::showInventoryPage()
{
    if (!inventoryPage || !inventoryPage->isVisible())
    {
        inventoryPage = new InventoryPage();
        inventoryPage->setAttribute(Qt::WA_DeleteOnClose);
        connect(inventoryPage, &InventoryPage::destroyed, this, [this]() {
            inventoryPage = nullptr;
        });
        inventoryPage->show();
    }
    else
    {
        inventoryPage->raise();
        inventoryPage->activateWindow();
    }
}

void MainWindow::showCutlistPage() 
{
    cutlistPage->show();
}

void MainWindow::showSalesPage() 
{
    salesPage->show();
}
