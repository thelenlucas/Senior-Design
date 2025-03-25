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
#include <QFrame>
#include <QStyle>
#include <QPalette>
#include <QSqlRecord>

// TODO: Make this configurable via a config file or database table.
const constexpr double DEFAULT_WINDOW_SIZE_RATIO = 0.6;

SalesPage::SalesPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SalesPage)
    , inventoryModel(new QSqlQueryModel(this))
    , selectedIndex(-1)
    , runningTotal(0.0)
{
    ui->setupUi(this);

    ResizeToDisplayPercentage(DEFAULT_WINDOW_SIZE_RATIO, DEFAULT_WINDOW_SIZE_RATIO);

    ui->inventoryTable->setModel(inventoryModel);
    RefreshInventoryModel();

    connect(ui->addButton, &QPushButton::clicked, this, [this]() {
        QItemSelectionModel* selectionModel = ui->inventoryTable->selectionModel();
        if (!selectionModel)
            return;

        QModelIndex index = selectionModel->currentIndex();
        if (!index.isValid())
            return;

        int idCol = inventoryModel->record(0).indexOf("ID");
        int speciesCol = inventoryModel->record(0).indexOf("Species");
        int valueCol = inventoryModel->record(0).indexOf("Value ($)");

        if (idCol == -1 || speciesCol == -1 || valueCol == -1)
        {
            qDebug() << "Expected columns not found in inventory model";
            return;
        }

        QString id = inventoryModel->data(inventoryModel->index(index.row(), idCol)).toString();
        QString species = inventoryModel->data(inventoryModel->index(index.row(), speciesCol)).toString();
        QString value = inventoryModel->data(inventoryModel->index(index.row(), valueCol)).toString();

        AddSelectedInventoryRow(id, species, value);
    });

    connect(ui->removeButton, &QPushButton::clicked, this, [this]() {
        if (selectedIndex >= 0 && selectedIndex < itemWidgets.size())
        {
            QWidget* widgetToRemove = itemWidgets[selectedIndex];
            double value = widgetToRemove->property("value").toDouble();
            int quantity = widgetToRemove->property("quantity").toInt();
            UpdateTotal(-value * quantity);

            ui->amountEntryLayout->removeWidget(widgetToRemove);
            widgetToRemove->deleteLater();
            itemWidgets.removeAt(selectedIndex);
            selectedIndex = -1;

            for (int i = 0; i < itemWidgets.size(); ++i)
                itemWidgets[i]->setStyleSheet((i % 2 == 0) ? "background-color: #f0f0f0;" : "background-color: #e0e0e0;");
        }
    });

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
    inventoryModel->setQuery("SELECT * FROM logs_view", QSqlDatabase::database());

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

void SalesPage::AddSelectedInventoryRow(const QString& id, const QString& species, const QString& valueStr)
{
    QWidget* container = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(6);

    QString labelText = QString("ID: %1 | Species: %2 | Value: $%3").arg(id, species, valueStr);
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

    double value = valueStr.toDouble();
    container->setProperty("value", value);
    container->setProperty("quantity", 1);

    int index = itemWidgets.size();
    container->setStyleSheet((index % 2 == 0) ? "background-color: #f0f0f0;" : "background-color: #e0e0e0;");
    itemWidgets.append(container);
    ui->amountEntryLayout->addWidget(container);

    // Enable context menu signal on left click.
    container->setContextMenuPolicy(Qt::CustomContextMenu);
    container->setCursor(Qt::PointingHandCursor);

    connect(container, &QWidget::customContextMenuRequested, this, [this, container]() {
        selectedIndex = ui->amountEntryLayout->indexOf(container);
        for (int i = 0; i < itemWidgets.size(); ++i)
        {
            itemWidgets[i]->setStyleSheet((i % 2 == 0) ? "background-color: #f0f0f0;" : "background-color: #e0e0e0;");
        }
        if (selectedIndex >= 0 && selectedIndex < itemWidgets.size())
        {
            itemWidgets[selectedIndex]->setStyleSheet("background-color: #ff7777;");
        }
    });

    connect(plusButton, &QPushButton::clicked, this, [this, quantityEdit, container]() {
        int qty = quantityEdit->text().toInt();
        quantityEdit->setText(QString::number(qty + 1));
        double value = container->property("value").toDouble();
        container->setProperty("quantity", qty + 1);
        UpdateTotal(value);
    });

    connect(minusButton, &QPushButton::clicked, this, [this, quantityEdit, container]() {
        int qty = quantityEdit->text().toInt();
        if (qty > 0)
        {
            quantityEdit->setText(QString::number(qty - 1));
            double value = container->property("value").toDouble();
            container->setProperty("quantity", qty - 1);
            UpdateTotal(-value);
        }
    });

    UpdateTotal(value);
}

/*void SalesPage::AddSelectedInventoryRow(const QString& id, const QString& species, const QString& valueStr)
{
    QWidget* container = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(6);

    QString labelText = QString("ID: %1 | Species: %2 | Value: $%3").arg(id, species, valueStr);
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

    double value = valueStr.toDouble();
    container->setProperty("value", value);
    container->setProperty("quantity", 1);

    int index = itemWidgets.size();
    container->setStyleSheet((index % 2 == 0) ? "background-color: #f0f0f0;" : "background-color: #e0e0e0;");
    itemWidgets.append(container);
    ui->amountEntryLayout->addWidget(container);

    connect(container, &QWidget::mousePressEvent, this, [this, index](QMouseEvent*) {
        for (int i = 0; i < itemWidgets.size(); ++i)
        {
            itemWidgets[i]->setStyleSheet((i % 2 == 0) ? "background-color: #f0f0f0;" : "background-color: #e0e0e0;");
        }
        itemWidgets[index]->setStyleSheet("background-color: #ff7777;");
        selectedIndex = index;
    });

    connect(plusButton, &QPushButton::clicked, this, [this, quantityEdit, container]() {
        int qty = quantityEdit->text().toInt();
        quantityEdit->setText(QString::number(qty + 1));
        double value = container->property("value").toDouble();
        container->setProperty("quantity", qty + 1);
        UpdateTotal(value);
    });

    connect(minusButton, &QPushButton::clicked, this, [this, quantityEdit, container]() {
        int qty = quantityEdit->text().toInt();
        if (qty > 0)
        {
            quantityEdit->setText(QString::number(qty - 1));
            double value = container->property("value").toDouble();
            container->setProperty("quantity", qty - 1);
            UpdateTotal(-value);
        }
    });

    UpdateTotal(value);
}*/

void SalesPage::UpdateTotal(double delta)
{
    runningTotal += delta;
    ui->totalValueDisplay->setText(QString::number(runningTotal, 'f', 2));
}
