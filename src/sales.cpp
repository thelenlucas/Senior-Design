#include "sales.hpp"
#include "ui_sales.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QScreen>
#include <QGuiApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

// TODO: Make this configurable.
const constexpr double DEFAULT_WINDOW_SIZE_RATIO = 0.6;

SalesPage::SalesPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SalesPage)
    , inventoryModel(new QSqlQueryModel(this))
{
    ui->setupUi(this);

    // Resize to screen % on launch
    ResizeToDisplayPercentage(DEFAULT_WINDOW_SIZE_RATIO, DEFAULT_WINDOW_SIZE_RATIO);

    // Connect inventory model to table
    ui->inventoryTable->setModel(inventoryModel);
    RefreshInventoryModel();

    // Hook add button to add selection to panel
    connect(ui->addButton, &QPushButton::clicked, this, [this]() {
        QItemSelectionModel* selectionModel = ui->inventoryTable->selectionModel();
        if (!selectionModel)
            return;

        QModelIndex index = selectionModel->currentIndex();
        if (!index.isValid())
            return;

        QString labelText = ui->inventoryTable->model()->data(index).toString();
        AddSelectedInventoryRow(labelText);
    });

    // Hook preview/export buttons (stub)
    connect(ui->previewButton, &QPushButton::clicked, this, []() {
        qDebug() << "Preview Webpage clicked";
    });

    connect(ui->exportButton, &QPushButton::clicked, this, []() {
        qDebug() << "Export Webpage clicked";
    });
}

SalesPage::~SalesPage()
{
    delete ui;
}

void SalesPage::RefreshInventoryModel()
{
    inventoryModel->setQuery("SELECT * FROM inventory_view", QSqlDatabase::database());

    if (inventoryModel->lastError().isValid())
        qDebug() << "Inventory model error:" << inventoryModel->lastError().text();
}

void SalesPage::ResizeToDisplayPercentage(double width_ratio, double height_ratio)
{
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QSize screenSize = screen->availableGeometry().size();
        QSize windowSize(screenSize.width() * width_ratio, screenSize.height() * height_ratio);
        resize(windowSize);
    }
}

void SalesPage::AddSelectedInventoryRow(const QString& labelText)
{
    QWidget* container = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(6);

    QLabel* label = new QLabel(labelText);
    QLineEdit* quantityEdit = new QLineEdit("1");
    quantityEdit->setFixedWidth(40);
    quantityEdit->setAlignment(Qt::AlignCenter);

    QPushButton* plusButton = new QPushButton("+");
    plusButton->setFixedSize(24, 24);

    QPushButton* minusButton = new QPushButton("-");
    minusButton->setFixedSize(24, 24);

    layout->addWidget(label);
    layout->addWidget(minusButton);
    layout->addWidget(quantityEdit);
    layout->addWidget(plusButton);

    ui->amountEntryLayout->addWidget(container);

    // Optional: Hook up + / - logic
    connect(plusButton, &QPushButton::clicked, this, [quantityEdit]() {
        int value = quantityEdit->text().toInt();
        quantityEdit->setText(QString::number(value + 1));
    });

    connect(minusButton, &QPushButton::clicked, this, [quantityEdit]() {
        int value = quantityEdit->text().toInt();
        quantityEdit->setText(QString::number(qMax(0, value - 1)));
    });
}
