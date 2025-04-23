#include <QDebug>
#include <QEvent>
#include <QFrame>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPalette>
#include <QPushButton>
#include <QScreen>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStyle>
#include <QVBoxLayout>
#include <QMessageBox>

#include "widgets/product_editing_widget.hpp"

#include "sales.hpp"
#include "ui_sales.h"

#include "domain/cookie.hpp"
#include "domain/firewood.hpp"
#include "domain/live_edge_slab.hpp"
#include "domain/lumber.hpp"

#include "infra/repository.hpp"

using namespace woodworks::domain;
using namespace woodworks::domain::imperial;
using namespace woodworks::domain::types;
using namespace woodworks::infra;
using namespace woodworks::sales;


SalesPage::SalesPage(QWidget *parent) : QWidget(parent), ui(new Ui::SalesPage)
{
    ui->setupUi(this); //! AHHHHHHHHHH - Lucas

    // Add product type to dropdown
    ui->typeCombo->addItem("Cookie", COOKIE);
    ui->typeCombo->addItem("Slab", SLAB);
    ui->typeCombo->addItem("Lumber", LUMBER);
    ui->typeCombo->addItem("Firewood", FIREWOOD);

    // Connect the add item button to the slot
    connect(ui->addItem, &QPushButton::clicked, this, &SalesPage::onAddItemButtonClicked);
}

SalesPage::~SalesPage() { delete ui; }

void SalesPage::addProduct(Product product)
{
    auto scrollArea = ui->amountScrollArea;
    auto scrollContents = ui->salesScrollContents;
    auto productWidget = new ProductEditorWidget(product, scrollContents);

    // Add the product widget to the scroll area
    scrollArea->setWidgetResizable(true);
    scrollArea->widget()->layout()->addWidget(productWidget);
}

void SalesPage::onAddItemButtonClicked()
{
    // Id from spin
    int id = ui->serialNumberSpinBox->value();

    // Get product type from combo
    bool found = false;
    auto type = static_cast<ProductType>(ui->typeCombo->currentData().toInt());
    if (type == COOKIE) {
        auto p = QtSqlRepository<Cookie>::spawn().get(id);
        if (p) {
            addProduct(p.value().toProduct());
            found = true;
        }
    } else if (type == SLAB)
    {
        auto p = QtSqlRepository<LiveEdgeSlab>::spawn().get(id);
        if (p) {
            addProduct(p.value().toProduct());
            found = true;
        }
    } else if (type == LUMBER) {
        auto p = QtSqlRepository<Lumber>::spawn().get(id);
        if (p) {
            addProduct(p.value().toProduct());
            found = true;
        }
    } else if (type == FIREWOOD) {
        auto p = QtSqlRepository<Firewood>::spawn().get(id);
        if (p) {
            addProduct(p.value().toProduct());
            found = true;
        }
    }

    if (!found) {
        // Warn with popup
        auto msgBox = new QMessageBox(this);
        msgBox->setText("Product not found");
        msgBox->setInformativeText("No product found with the given ID.");
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->setDefaultButton(QMessageBox::Ok);
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setWindowTitle("Product Not Found");
        msgBox->setAttribute(Qt::WA_DeleteOnClose);
    }   
}