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

#include "sales.hpp"
#include "ui_sales.h"

// TODO: Make this configurable via a config file or database table.
const constexpr double DEFAULT_WINDOW_SIZE_RATIO = 0.6;

SalesPage::SalesPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::SalesPage),
      inventoryModel(new QSqlQueryModel(this)), selectedIndex(-1),
      runningTotal(0.0)
{
    ui->setupUi(this);

    ResizeToDisplayPercentage(DEFAULT_WINDOW_SIZE_RATIO,
                              DEFAULT_WINDOW_SIZE_RATIO);

    ui->inventoryTable->setModel(inventoryModel);
    RefreshInventoryModel();

    connect(ui->addButton, &QPushButton::clicked, this,
            [this]()
            {
                QItemSelectionModel *selectionModel =
                    ui->inventoryTable->selectionModel();
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

                QString id =
                    inventoryModel
                        ->data(inventoryModel->index(index.row(), idCol))
                        .toString();
                QString species =
                    inventoryModel
                        ->data(inventoryModel->index(index.row(), speciesCol))
                        .toString();
                QString value =
                    inventoryModel
                        ->data(inventoryModel->index(index.row(), valueCol))
                        .toString();

                AddSelectedInventoryRow(id, species, value);
            });

    connect(ui->removeButton, &QPushButton::clicked, this,
            [this]()
            {
                if (selectedIndex >= 0 && selectedIndex < itemWidgets.size())
                {
                    QWidget *widgetToRemove = itemWidgets[selectedIndex];
                    double value = widgetToRemove->property("value").toDouble();
                    int quantity = widgetToRemove->property("quantity").toInt();
                    UpdateTotal(-value * quantity);

                    ui->amountEntryLayout->removeWidget(widgetToRemove);
                    widgetToRemove->deleteLater();
                    itemWidgets.removeAt(selectedIndex);
                    selectedIndex = -1;

                    // for (int i = 0; i < itemWidgets.size(); ++i)
                    //     itemWidgets[i]->setStyleSheet((i % 2 == 0) ?
                    //     "background-color: #f0f0f0;" : "background-color:
                    //     #e0e0e0;");
                }
            });

    connect(ui->previewButton, &QPushButton::clicked, this,
            []() { qDebug() << "Preview Webpage clicked"; });

    connect(ui->exportButton, &QPushButton::clicked, this,
            []() { qDebug() << "Export Webpage clicked"; });

    ui->amountScrollContents->setSizePolicy(QSizePolicy::Preferred,
                                            QSizePolicy::Maximum);
}

SalesPage::~SalesPage() { delete ui; }

void SalesPage::RefreshInventoryModel()
{
    inventoryModel->setQuery("SELECT * FROM logs_view",
                             QSqlDatabase::database());

    if (inventoryModel->lastError().isValid())
        qDebug() << "Inventory model error:"
                 << inventoryModel->lastError().text();
}

void SalesPage::ResizeToDisplayPercentage(double width_ratio,
                                          double height_ratio)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QSize screenSize = screen->availableGeometry().size();
        QSize windowSize(screenSize.width() * width_ratio,
                         screenSize.height() * height_ratio);
        resize(windowSize);
    }
}

void SalesPage::AddSelectedInventoryRow(QString const &id,
                                        QString const &species,
                                        QString const &value)
{
    QString labelText =
        QString("ID: %1 | Species: %2 | Value: %3").arg(id, species, value);

    QWidget *rowWidget = new QWidget;
    rowWidget->setFixedHeight(100);
    rowWidget->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Fixed); // 🛠 Important fix

    QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(6, 2, 6, 2);
    rowLayout->setSpacing(6);

    QLabel *imageLabel = new QLabel;
    imageLabel->setFixedSize(80, 80);
    imageLabel->setPixmap(
        QPixmap(":/images/placeholder.png")
            .scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QLabel *label = new QLabel(labelText);
    label->setMinimumWidth(200);

    QLineEdit *quantityEdit = new QLineEdit("1");
    quantityEdit->setFixedWidth(40);
    quantityEdit->setAlignment(Qt::AlignCenter);
    quantityEdit->setProperty("quantity", 1);

    QPushButton *plusButton = new QPushButton("+");
    plusButton->setFixedSize(24, 24);

    QPushButton *minusButton = new QPushButton("-");
    minusButton->setFixedSize(24, 24);

    QLineEdit *priceEdit = new QLineEdit("0.00");
    priceEdit->setFixedWidth(60);
    priceEdit->setAlignment(Qt::AlignRight);
    priceEdit->setPlaceholderText("Price");

    QPushButton *removeButton = new QPushButton("×");
    removeButton->setFixedSize(24, 24);
    removeButton->setToolTip("Remove entry");

    rowLayout->addWidget(imageLabel);
    rowLayout->addWidget(label);
    rowLayout->addWidget(minusButton);
    rowLayout->addWidget(quantityEdit);
    rowLayout->addWidget(plusButton);
    rowLayout->addWidget(priceEdit);
    rowLayout->addWidget(removeButton);

    int rowIndex = itemWidgets.size();
    QString bgColor = (rowIndex % 2 == 0) ? "#f0f0f0" : "#e0e0e0";
    rowWidget->setStyleSheet(QString("background-color: %1;").arg(bgColor));

    ui->amountEntryLayout->addWidget(rowWidget);
    itemWidgets.append(rowWidget);

    connect(plusButton, &QPushButton::clicked, this,
            [quantityEdit]()
            {
                int value = quantityEdit->text().toInt();
                quantityEdit->setText(QString::number(value + 1));
                quantityEdit->setProperty("quantity", value + 1);
            });

    connect(minusButton, &QPushButton::clicked, this,
            [quantityEdit]()
            {
                int value = quantityEdit->text().toInt();
                int newVal = qMax(0, value - 1);
                quantityEdit->setText(QString::number(newVal));
                quantityEdit->setProperty("quantity", newVal);
            });

    connect(removeButton, &QPushButton::clicked, this,
            [this, rowWidget]()
            {
                ui->amountEntryLayout->removeWidget(rowWidget);
                itemWidgets.removeOne(rowWidget);
                rowWidget->deleteLater();
                ReapplyStripedBackgrounds();
            });

    rowWidget->installEventFilter(this);
}

void SalesPage::ReapplyStripedBackgrounds()
{
    for (int i = 0; i < itemWidgets.size(); ++i)
    {
        QString color = (i % 2 == 0) ? "#f0f0f0" : "#e0e0e0";
        itemWidgets[i]->setStyleSheet(
            QString("background-color: %1;").arg(color));
    }
}

void SalesPage::UpdateTotal(double delta)
{
    runningTotal += delta;
    ui->totalValueDisplay->setText(QString::number(runningTotal, 'f', 2));
}

bool SalesPage::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        for (QWidget *widget : itemWidgets)
        {
            QFrame *frame = widget->findChild<QFrame *>();
            if (frame)
                frame->setStyleSheet("QFrame { background-color: white; }");
        }

        QWidget *clickedWidget = qobject_cast<QWidget *>(obj);
        if (clickedWidget)
        {
            QFrame *frame = clickedWidget->findChild<QFrame *>();
            if (frame)
                frame->setStyleSheet("QFrame { background-color: #cce6ff; }");
        }
    }

    return QWidget::eventFilter(obj, event);
}
