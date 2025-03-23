#include "sales.hpp"
#include "ui_sales.h"

SalesPage::SalesPage(QWidget *parent) : QWidget(parent), ui(new Ui::SalesPage) 
{
    ui->setupUi(this);
}

SalesPage::~SalesPage() 
{
    delete ui;
}