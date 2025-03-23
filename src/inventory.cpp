#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "inventory.hpp"
#include "ui_inventory.h"

InventoryPage::InventoryPage(QWidget *parent) : QWidget(parent), ui(new Ui::InventoryPage) 
{
    ui->setupUi(this);
    model = new QSqlTableModel(this, QSqlDatabase::database());
    model->setTable("logs");
    model->select();
    ui->inventoryList->setModel(model);
}

InventoryPage::~InventoryPage() 
{
    delete ui;
}

void InventoryPage::refreshModel() 
{
    model->select();
}