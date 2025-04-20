#include <QAction>
#include <QDebug>
#include <QDockWidget>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollArea>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QVBoxLayout>

#include "cutlist.hpp"
#include "firewood.hpp"
#include "inventory.hpp"
#include "logs.hpp"
#include "mainwindow.hpp"
#include "project_editor.hpp"
#include "sales.hpp"
#include "types.hpp"
#include "ui_mainwindow.h"

#include <iomanip>
#include <iostream>
#include <string>

#define GROUPED_LOGS_QUERY "SELECT * from logs_view_grouped"
#define LOGS_QUERY "SELECT * FROM logs_view"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      cutlistPage(new CutlistPage()), salesPage(new SalesPage()),
      inventoryPage(nullptr)
{
    // Load UI layout and set the central widget
    ui->setupUi(this);
    setCentralWidget(ui->centralwidget);

    // Enable docking options globally for the main window. We aren't using this
    // yet on the main branch.
    // setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks
    // |
    //               QMainWindow::AllowTabbedDocks);

    // Add the menu and actions for switching between pages
    QMenu* menu = menuBar()->addMenu("WoodWorks");
    QAction* inventoryAction = new QAction("Inventory", this);
    QAction* cutlistAction = new QAction("Cutlist", this);
    QAction* salesAction = new QAction("Sales", this);

    menu->addAction(inventoryAction);
    menu->addAction(cutlistAction);
    menu->addAction(salesAction);

    connect(inventoryAction, &QAction::triggered, this,
            &MainWindow::showInventoryPage);
    connect(cutlistAction, &QAction::triggered, this,
            &MainWindow::showCutlistPage);
    connect(salesAction, &QAction::triggered, this, &MainWindow::showSalesPage);

    connect(ui->openInventoryButton, &QPushButton::clicked, this,
            &MainWindow::showInventoryPage);
    connect(ui->openCutlistButton, &QPushButton::clicked, this,
            &MainWindow::showCutlistPage);
    connect(ui->openSalesButton, &QPushButton::clicked, this,
            &MainWindow::showSalesPage);

    // Setup a simple welcome message landing page
    QLabel* welcomeLabel = new QLabel("Welcome to WoodWorks Sawmill Manager");
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet(
        "font-size: 24px; font-weight: bold; padding: 40px;");

    QVBoxLayout* layout = new QVBoxLayout(ui->centralwidget);
    layout->addWidget(welcomeLabel);

    // Database connection setup
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DATABASE_FILE);
    if (!db.open())
    {
        qDebug() << "Database error:" << db.lastError().text();
        return;
    }

    ui->openInventoryButton->setStyleSheet(
        "font-family: 'Segoe UI Symbol'; font-size: 14pt;");
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::showInventoryPage()
{
    if (!inventoryPage)
        inventoryPage = new InventoryPage();
    inventoryPage->show();
    inventoryPage->raise();
    inventoryPage->activateWindow();
}

void MainWindow::showCutlistPage()
{
    if (!cutlistPage)
        cutlistPage = new CutlistPage();
    cutlistPage->show();
    cutlistPage->raise();
    cutlistPage->activateWindow();
}

void MainWindow::showSalesPage()
{
    if (!salesPage)
        salesPage = new SalesPage();
    salesPage->show();
    salesPage->raise();
    salesPage->activateWindow();
}
