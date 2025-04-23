#include <QDebug>
#include <QEvent>
#include <QFrame>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QInputDialog>
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
#include <QListWidget>
#include <QDialog>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>

#include <QWebEngineView>

#include "sales.hpp"
#include "ui_sales.h"

#include "domain/cookie.hpp"
#include "domain/firewood.hpp"
#include "domain/live_edge_slab.hpp"
#include "domain/lumber.hpp"

#include "sales/product.hpp"
#include "sales/generator.hpp"

#include "infra/repository.hpp"

using namespace woodworks::domain;
using namespace woodworks::domain::imperial;
using namespace woodworks::domain::types;
using namespace woodworks::infra;
using namespace woodworks::sales;

SalesPage::SalesPage(QWidget *parent) : QWidget(parent), ui(new Ui::SalesPage)
{
    ui->setupUi(this); //! AHHHHHHHHHH - Lucas

    qRegisterMetaType<woodworks::sales::Product>("Product");

    // Add product type to dropdown
    ui->typeCombo->addItem("Cookie", COOKIE);
    ui->typeCombo->addItem("Slab", SLAB);
    ui->typeCombo->addItem("Lumber", LUMBER);
    ui->typeCombo->addItem("Firewood", FIREWOOD);

    // Connect the add item button to the slot
    connect(ui->addItem, &QPushButton::clicked, this, &SalesPage::onAddItemButtonClicked);

    // Connect double-click on list to edit price
    connect(ui->productsListWidget, &QListWidget::itemDoubleClicked, this, &SalesPage::onProductItemDoubleClicked);

    // Connect preview button to slot
    connect(ui->previewButton, &QPushButton::clicked, this, &SalesPage::onPreviewHtmlButtonClicked);
    // Connect save button to slot
    connect(ui->exportButton, &QPushButton::clicked, this, &SalesPage::onSaveHtmlButtonClicked);
}

SalesPage::~SalesPage() { delete ui; }

void SalesPage::onPreviewHtmlButtonClicked()
{
    // Get all of the products
    auto products = this->products();

    auto generator = SalesPageGenerator();
    for (const auto &p : products)
    {
        generator.addProduct(p);
    }
    auto html = generator.generate();
    // Save  right next to executable
    QString filename = QDir::currentPath() + "/sales.html";
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file for writing";
        return;
    }
    QTextStream out(&file);
    out << QString::fromStdString(html);
    file.close();

    // Open the file in a web view
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Sales Preview");
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QWebEngineView *view = new QWebEngineView(dialog);
    layout->addWidget(view);
    connect(dialog, &QDialog::finished, view, &QObject::deleteLater);
    connect(dialog, &QDialog::finished, view->page(), &QObject::deleteLater);
    view->setUrl(QUrl::fromLocalFile(filename));
    dialog->resize(800, 600);
    dialog->show();
}

void SalesPage::onSaveHtmlButtonClicked()
{
    // Save instead of previewing
    auto products = this->products();
    auto generator = SalesPageGenerator();
    for (const auto &p : products)
    {
        generator.addProduct(p);
    }
    auto html = generator.generate();

    // Choose location to save, html file
    QString filename = QFileDialog::getSaveFileName(this, tr("Save HTML"), QDir::currentPath(), tr("HTML Files (*.html);;All Files (*)"));
    if (filename.isEmpty())
    {
        return; // User canceled
    }
    if (!filename.endsWith(".html"))
    {
        filename += ".html"; // Add .html extension if not present
    }
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file for writing";
        return;
    }
    QTextStream out(&file);
    out << QString::fromStdString(html);
    file.close();
}

void SalesPage::addProduct(Product product)
{
    // Add product entry to the list widget
    // Use Product::toListString for display
    QString text = QString::fromStdString(product.toListString());
    auto item = new QListWidgetItem(text);
    item->setData(Qt::UserRole, QVariant::fromValue(product));
    ui->productsListWidget->addItem(item);
}

void SalesPage::onAddItemButtonClicked()
{
    // Id from spin
    int id = ui->serialNumberSpinBox->value();

    // Get product type from combo
    bool found = false;
    auto type = static_cast<ProductType>(ui->typeCombo->currentData().toInt());
    if (type == COOKIE)
    {
        auto p = QtSqlRepository<Cookie>::spawn().get(id);
        if (p)
        {
            addProduct(p.value().toProduct());
            found = true;
        }
    }
    else if (type == SLAB)
    {
        auto p = QtSqlRepository<LiveEdgeSlab>::spawn().get(id);
        if (p)
        {
            addProduct(p.value().toProduct());
            found = true;
        }
    }
    else if (type == LUMBER)
    {
        auto p = QtSqlRepository<Lumber>::spawn().get(id);
        if (p)
        {
            addProduct(p.value().toProduct());
            found = true;
        }
    }
    else if (type == FIREWOOD)
    {
        auto p = QtSqlRepository<Firewood>::spawn().get(id);
        if (p)
        {
            addProduct(p.value().toProduct());
            found = true;
        }
    }

    if (!found)
    {
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

void SalesPage::onProductItemDoubleClicked(QListWidgetItem *item)
{
    QVariant var = item->data(Qt::UserRole);
    auto product = var.value<woodworks::sales::Product>();
    // Create dialog to edit price and units together
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Edit Price and Units"));
    QFormLayout form(&dlg);
    QDoubleSpinBox *priceSpin = new QDoubleSpinBox(&dlg);
    priceSpin->setRange(0, 1e6);
    priceSpin->setDecimals(2);
    priceSpin->setValue(product.price);
    QLineEdit *unitsEdit = new QLineEdit(&dlg);
    unitsEdit->setText(QString::fromStdString(product.pricingUnits));
    form.addRow(tr("Price:"), priceSpin);
    form.addRow(tr("Units:"), unitsEdit);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dlg);
    form.addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    if (dlg.exec() == QDialog::Accepted)
    {
        product.price = priceSpin->value();
        product.pricingUnits = unitsEdit->text().toStdString();
        // Update display using Product::toListString
        QString text = QString::fromStdString(product.toListString());
        item->setText(text);
        item->setData(Qt::UserRole, QVariant::fromValue(product));
    }
}

QVector<woodworks::sales::Product> SalesPage::products() const
{
    QVector<woodworks::sales::Product> list;
    for (int i = 0; i < ui->productsListWidget->count(); ++i)
    {
        auto item = ui->productsListWidget->item(i);
        list.append(item->data(Qt::UserRole).value<woodworks::sales::Product>());
    }
    return list;
}