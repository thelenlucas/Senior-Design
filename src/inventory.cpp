#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QScreen>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>

#include "cookies.hpp"
#include "inventory.hpp"
#include "logs.hpp"
#include "ui_inventory.h"

InventoryPage::InventoryPage(QWidget* parent)
    : QWidget(parent), ui(new Ui::InventoryPage),
      individualLogsModel(new QSqlQueryModel(this)),
      groupedLogsModel(new QSqlQueryModel(this)),
      lumberModel(new QSqlQueryModel(this)),
      slabsModel(new QSqlQueryModel(this)),
      cookiesModel(new QSqlQueryModel(this)),
      firewoodModel(new QSqlQueryModel(this))
{
    ui->setupUi(this);

    // Dynamically resize window to 60% of screen size and center.
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QSize screenSize = screen->availableGeometry().size();
        QSize windowSize(screenSize.width() * 0.6, screenSize.height() * 0.6);
        resize(windowSize);
        move((screenSize.width() - windowSize.width()) / 2,
             (screenSize.height() - windowSize.height()) / 2);
    }

    // Setup the logical models for our mvc.
    ui->individualLogsTable->setModel(individualLogsModel);
    ui->groupedLogsTable->setModel(groupedLogsModel);
    ui->lumberTable->setModel(lumberModel);
    ui->slabsTable->setModel(slabsModel);
    ui->cookiesTable->setModel(cookiesModel);
    ui->firewoodTable->setModel(firewoodModel);

    RefreshModels();

    connect(ui->addLogButton, &QPushButton::clicked, this,
            &InventoryPage::onAddLogClicked);
    connect(ui->spreadsheetImporterButton, &QPushButton::clicked, this,
            &InventoryPage::onSpreadsheetImportClicked);
    connect(ui->createCookieButton, &QPushButton::clicked, this,
            &InventoryPage::onCookieButtonClicked);
    connect(ui->clearFilterButton, &QPushButton::clicked, this,
            &InventoryPage::ClearAllFilters);

    SetupFilterSignals();

    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle("Inventory Management");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Window);
}

InventoryPage::~InventoryPage() { delete ui; }

void InventoryPage::RefreshModels()
{
    individualLogsModel->setQuery("SELECT * FROM logs_view",
                                  QSqlDatabase::database());
    groupedLogsModel->setQuery("SELECT * FROM logs_view_grouped",
                               QSqlDatabase::database());
    lumberModel->setQuery("SELECT * FROM display_lumber",
                          QSqlDatabase::database());
    slabsModel->setQuery("SELECT * FROM display_slabs",
                         QSqlDatabase::database());
    cookiesModel->setQuery("SELECT * FROM display_cookies",
                           QSqlDatabase::database());
    firewoodModel->setQuery("SELECT * FROM display_firewood",
                            QSqlDatabase::database());

    if (individualLogsModel->lastError().isValid())
        qDebug() << "Individual logs query error:"
                 << individualLogsModel->lastError().text();

    if (groupedLogsModel->lastError().isValid())
        qDebug() << "Grouped logs query error:"
                 << groupedLogsModel->lastError().text();

    if (lumberModel->lastError().isValid())
        qDebug() << "Lumber model query error:"
                 << lumberModel->lastError().text();

    if (slabsModel->lastError().isValid())
        qDebug() << "Slabs model query error:"
                 << slabsModel->lastError().text();

    if (cookiesModel->lastError().isValid())
        qDebug() << "Cookies model query error:"
                 << cookiesModel->lastError().text();

    if (firewoodModel->lastError().isValid())
        qDebug() << "Firewood model query error:"
                 << firewoodModel->lastError().text();
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

    Log log(0, species.toStdString(), lenQuarters, diamQuarters, costQuarters,
            quality, location);
    log.insert();

    RefreshModels();
}

void InventoryPage::onCookieButtonClicked()
{
    // Get the selected log id
    std::optional<Log> opt =
        Log::get_by_id(ui->individualLogsTable->currentIndex()
                           .siblingAtColumn(0)
                           .data()
                           .toInt());

    if (!opt)
    {
        QMessageBox::critical(this, "Error", "Log not found");
        return;
    }

    Log log = opt.value();

    // Dialog for the user to enter a uint for cookie thickness
    bool ok;
    int enteredCut = QInputDialog::getInt(
        this, QObject::tr("Cookie Cutter"),
        QObject::tr("Please enter the desired cookie thickness (inches):"),
        0.01, 0.01, log.getLenQuarters(), ok);

    if (!ok)
    {
        std::cout << "User canceled input." << std::endl;
        return;
    }

    std::cout << "Cutting Cookie!" << std::endl;
    Cookie::make_from_log(log, static_cast<unsigned int>(enteredCut));
    log.cut_length(static_cast<unsigned int>(enteredCut));

    RefreshModels();
}

void InventoryPage::onSpreadsheetImportClicked()
{
    QString filename =
        QFileDialog::getOpenFileName(this, "Import Spreadsheet", QString(),
                                     "Spreadsheets (*.csv *.xls *.xlsx)");

    if (filename.isEmpty())
        return;

    // TODO: Implement spreadsheet import parsing logic in logic module.
    QMessageBox::information(this, "Import Selected",
                             "File selected: " + filename);
}

void InventoryPage::mousePressEvent(QMouseEvent* event)
{
    qDebug() << "InventoryPage clicked at:" << event->pos()
             << "Mouse click on widgetAt:"
             << QApplication::widgetAt(QCursor::pos());

    QWidget::mousePressEvent(event);
}

bool InventoryPage::eventFilter(QObject* obj, QEvent* event)
{
    qDebug() << "EVENT TYPE:" << event->type()
             << "on:" << obj->metaObject()->className()
             << "named:" << obj->objectName();

    if (event->type() == QEvent::MouseButtonPress)
    {
        qDebug() << "Mouse click intercepted on:" << obj->objectName();
    }

    return QWidget::eventFilter(obj, event);
}

void InventoryPage::ApplyActiveFilters()
{
    QString species = ui->speciesFilterEdit->text().trimmed();
    QString location = ui->locationFilterEdit->text().trimmed();
    int minLength = ui->minLengthSpin->value();
    int minDiameter = ui->minDiameterSpin->value();
    double minCost = ui->minCostSpin->value();
    int minQuality = ui->minQualitySpin->value();

    QString currentTab = ui->inventoryTabs->currentWidget()->objectName();
    QSqlQueryModel* targetModel = nullptr;
    QString baseQuery;

    if (currentTab == "logsTab")
    {
        targetModel = individualLogsModel;
        baseQuery = "SELECT * FROM logs_view";
    }
    else if (currentTab == "groupedLogsTab")
    {
        targetModel = groupedLogsModel;
        baseQuery = "SELECT * FROM logs_view_grouped";
    }
    else if (currentTab == "lumberTab")
    {
        targetModel = lumberModel;
        baseQuery = "SELECT * FROM display_lumber";
    }
    else if (currentTab == "slabsTab")
    {
        targetModel = slabsModel;
        baseQuery = "SELECT * FROM display_slabs";
    }
    else if (currentTab == "cookiesTab")
    {
        targetModel = cookiesModel;
        baseQuery = "SELECT * FROM display_cookies";
    }
    else if (currentTab == "firewoodTab")
    {
        targetModel = firewoodModel;
        baseQuery = "SELECT * FROM display_firewood";
    }

    if (!targetModel)
    {
        qDebug() << "[InventoryPage] Unknown tab for filtering.";
        return;
    }

    QStringList conditions;

    if (!species.isEmpty())
        conditions << QString("species LIKE '%%1%'").arg(species);
    if (!location.isEmpty())
        conditions << QString("location LIKE '%%1%'").arg(location);
    if (minLength > 0)
        conditions << QString("length >= %1").arg(minLength);
    if (minDiameter > 0)
        conditions << QString("diameter >= %1").arg(minDiameter);
    if (minCost > 0.0)
        conditions << QString("cost >= %1").arg(minCost);
    if (minQuality > 0)
        conditions << QString("quality >= %1").arg(minQuality);

    QString finalQuery = baseQuery;
    if (!conditions.isEmpty())
        finalQuery += " WHERE " + conditions.join(" AND ");

    QSqlQuery testQuery(finalQuery, QSqlDatabase::database());
    if (!testQuery.exec())
    {
        qDebug() << "[InventoryPage] Filter query rejected.";
        ResetFilterColors();
        UpdateFieldColor(ui->speciesFilterEdit, species.isEmpty());
        UpdateFieldColor(ui->locationFilterEdit, location.isEmpty());
        UpdateFieldColor(ui->minLengthSpin, minLength == 0);
        UpdateFieldColor(ui->minDiameterSpin, minDiameter == 0);
        UpdateFieldColor(ui->minCostSpin, minCost == 0.0);
        UpdateFieldColor(ui->minQualitySpin, minQuality == 0);
        return;
    }

    targetModel->setQuery(finalQuery, QSqlDatabase::database());
    ResetFilterColors();

    if (!species.isEmpty())
        UpdateFieldColor(ui->speciesFilterEdit, true);
    if (!location.isEmpty())
        UpdateFieldColor(ui->locationFilterEdit, true);
    if (minLength > 0)
        UpdateFieldColor(ui->minLengthSpin, true);
    if (minDiameter > 0)
        UpdateFieldColor(ui->minDiameterSpin, true);
    if (minCost > 0.0)
        UpdateFieldColor(ui->minCostSpin, true);
    if (minQuality > 0)
        UpdateFieldColor(ui->minQualitySpin, true);
}

void InventoryPage::SetupFilterSignals()
{
    connect(ui->speciesFilterEdit, &QLineEdit::textChanged, this,
            &InventoryPage::ApplyActiveFilters);
    connect(ui->locationFilterEdit, &QLineEdit::textChanged, this,
            &InventoryPage::ApplyActiveFilters);
    connect(ui->minLengthSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &InventoryPage::ApplyActiveFilters);
    connect(ui->minDiameterSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &InventoryPage::ApplyActiveFilters);
    connect(ui->minCostSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &InventoryPage::ApplyActiveFilters);
    connect(ui->minQualitySpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &InventoryPage::ApplyActiveFilters);
    connect(ui->inventoryTabs, &QTabWidget::currentChanged, this,
            &InventoryPage::ApplyActiveFilters);
    connect(ui->clearFilterButton, &QPushButton::clicked, this,
            &InventoryPage::ClearAllFilters);
}

void InventoryPage::UpdateFieldColor(QWidget* widget, bool valid)
{
    if (!widget)
        return;

    if (valid)
        widget->setStyleSheet("background-color: #e6f4ff;"); // light blue
    else
        widget->setStyleSheet("background-color: #ffe6e6;"); // dull red
}

void InventoryPage::ResetFilterColors()
{
    ui->speciesFilterEdit->setStyleSheet("background-color: white;");
    ui->locationFilterEdit->setStyleSheet("background-color: white;");
    ui->minLengthSpin->setStyleSheet("background-color: white;");
    ui->minDiameterSpin->setStyleSheet("background-color: white;");
    ui->minCostSpin->setStyleSheet("background-color: white;");
    ui->minQualitySpin->setStyleSheet("background-color: white;");
}

void InventoryPage::ClearAllFilters()
{
    ui->speciesFilterEdit->clear();
    ui->locationFilterEdit->clear();
    ui->minLengthSpin->setValue(0);
    ui->minDiameterSpin->setValue(0);
    ui->minCostSpin->setValue(0.0);
    ui->minQualitySpin->setValue(0);

    ResetFilterColors();
    ApplyActiveFilters();
}
