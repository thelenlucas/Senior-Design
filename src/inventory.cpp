#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QScreen>
#include <QMouseEvent>
#include <QDockWidget>
#include <QTimer>

#include "inventory.hpp"
#include "ui_inventory.h"
#include "logs.hpp"

InventoryPage::InventoryPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InventoryPage)
    , individualLogsModel(new QSqlQueryModel(this))
    , groupedLogsModel(new QSqlQueryModel(this))
{
    ui->setupUi(this);

    // Dynamically resize window to 60% of screen size
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QSize screenSize = screen->availableGeometry().size();
        QSize windowSize(screenSize.width() * 0.6, screenSize.height() * 0.6);
        resize(windowSize);
    }

    // Setup the logical models for our mvc.
    ui->individualLogsTable->setModel(individualLogsModel);
    ui->groupedLogsTable->setModel(groupedLogsModel);

    refreshModels();

    connect(ui->addLogButton, &QPushButton::clicked, this, &InventoryPage::onAddLogClicked);
    connect(ui->spreadsheetImporterButton, &QPushButton::clicked, this, &InventoryPage::onSpreadsheetImportClicked);

    setFocusPolicy(Qt::StrongFocus);

    QTimer::singleShot(0, this, [this]() {
        for (QObject* child : this->findChildren<QObject*>())
        {
            qDebug() << "Late-installing filter on:"
                     << child->metaObject()->className()
                     << "Name:" << child->objectName();
            child->installEventFilter(this);
        }

        QDockWidget* dock = qobject_cast<QDockWidget*>(this->parentWidget());
        if (dock)
        {
            qDebug() << "Dock found in deferred setup:";
            //dock->setAttribute(Qt::WA_TransparentForMouseEvents, false);
            //dock->setFocusPolicy(Qt::StrongFocus);
            dock->setFocus();
            this->setFocus();
        }
        else
        {
            qDebug() << "Dock still null in deferred QTimer setup";
        }
    });
}

InventoryPage::~InventoryPage()
{
    delete ui;
}

void InventoryPage::refreshModels()
{
    individualLogsModel->setQuery("SELECT * FROM logs_view", QSqlDatabase::database());
    groupedLogsModel->setQuery("SELECT * FROM logs_view_grouped", QSqlDatabase::database());

    if (individualLogsModel->lastError().isValid())
        qDebug() << "Individual logs query error:" << individualLogsModel->lastError().text();

    if (groupedLogsModel->lastError().isValid())
        qDebug() << "Grouped logs query error:" << groupedLogsModel->lastError().text();
}

void InventoryPage::onAddLogClicked()
{
    QString species = ui->speciesEntry->text();
    int lenFt = ui->lengthFt->value();
    int lenIn = ui->lengthIn->value();
    int diamIn = ui->diameterIn->value();
    double costVal = ui->costValue->value();
    int quality = ui->qualityValue->value();
    std::string location = ui->locationEntry->text().toStdString();

    int lenQuarters = (lenFt * 12 + lenIn) * 4;
    int diamQuarters = diamIn * 4;

    int costCents = static_cast<int>(costVal * 100);
    int costQuarters = costCents / lenQuarters;

    Log log(0, species.toStdString(), lenQuarters, diamQuarters, costQuarters, quality, location);
    log.insert();

    refreshModels();
}

void InventoryPage::onSpreadsheetImportClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Import Spreadsheet", QString(), "Spreadsheets (*.csv *.xls *.xlsx)");

    if (filename.isEmpty())
        return;

    QMessageBox::information(this, "Import Selected", "File selected: " + filename);
}

void InventoryPage::mousePressEvent(QMouseEvent* event)
{
    qDebug() << "InventoryPage clicked at:" << event->pos()
             << "Mouse click on widgetAt:" << QApplication::widgetAt(QCursor::pos());

    QWidget::mousePressEvent(event);
}

bool InventoryPage::eventFilter(QObject* obj, QEvent* event)
{
    qDebug() << "EVENT TYPE:" << event->type()
             << "on:" << obj->metaObject()->className()
             << "named:" << obj->objectName();

    if (event->type() == QEvent::MouseButtonPress)
        qDebug() << "Mouse click intercepted on:" << obj->objectName();

    return QWidget::eventFilter(obj, event);
}

void InventoryPage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    QDockWidget* dock = qobject_cast<QDockWidget*>(this->parentWidget());
    if (dock)
    {
        qDebug() << "Dock detected in showEvent:" << dock->objectName();
        dock->setFocusPolicy(Qt::StrongFocus);
        dock->setFocus();
        this->setFocus();

        if (!dock->isFloating())
        {
            dock->setFloating(true);
            qDebug() << "Forced dock to float in showEvent.";
        }

        dock->raise();
        dock->activateWindow();
    }
    else
    {
        qDebug() << "Dock still null in showEvent.";
    }
}

bool InventoryPage::event(QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
        qDebug() << "[InventoryPage] caught mouse event in generic event()";

    return QWidget::event(event);
}
