#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "cutlist.hpp"
#include "ui_cutlist.h"

CutlistPage::CutlistPage(QWidget *parent) : QWidget(parent), ui(new Ui::CutlistPage) 
{
    ui->setupUi(this);
    model = new QSqlTableModel(this, QSqlDatabase::database());
    model->setTable("cutlist");
    model->select();
    ui->cutlistTable->setModel(model);
}

CutlistPage::~CutlistPage() 
{
    delete ui;
}

void CutlistPage::refreshModel() 
{
    model->select();
}