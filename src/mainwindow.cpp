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
#include <QVBoxLayout>

#include "project_editor.hpp"
#include "types.hpp"

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "inventory.hpp"
#include "cutlist.hpp"
#include "sales.hpp"
#include "infra/mappers/view_helpers.hpp"
#include <optional>

#include "domain/log.hpp"
#include "infra/connection.hpp"
#include "infra/repository.hpp"
#include "widgets/log_inventory.hpp"

using namespace woodworks::domain;
using namespace woodworks::widgets;

#define GROUPED_LOGS_QUERY "SELECT * from logs_view_grouped"
#define LOGS_QUERY "SELECT * FROM logs_view"

// TODO: Remove this outdated window, replace with landing page

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

    // Set the inventoryLayout vertical layout and insert a log_inventory widget
    auto *w = new LogInventory(this);
    auto *layout = new QVBoxLayout;
    layout->addWidget(w);
    layout->addStretch();
    ui->inventoryLayout->setLayout(layout);

    refreshModel();
}

void MainWindow::refreshModel()
{
    // 
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
    //Log log(0, species.toStdString(), lenQuarters, diamQuarters, costQuarters, quality, location);
    //log.insert();

    // Refresh the model
    refreshModel();
}

void MainWindow::onScrapLogButtonClicked() {
    

    refreshModel();
}

void MainWindow::onFirewoodButtonClicked() {
    }

void MainWindow::onTableCellDoubleClicked() {
    
}

void MainWindow::onProjectEditActionTriggered() {
   
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
