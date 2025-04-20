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
#include <QStringList>
#include <QVariant>

#include "inventory.hpp"
#include "csv_importer.hpp"
#include "ui_inventory.h"

#include "infra/mappers/view_helpers.hpp"
#include "domain/units.hpp"
#include "domain/types.hpp"
#include "domain/log.hpp"

#include "domain/units.hpp"
#include "domain/types.hpp"
#include "domain/log.hpp"

#include "infra/connection.hpp"
#include "infra/repository.hpp"
#include "infra/mappers/view_helpers.hpp"
#include "infra/helpers.hpp"

using namespace woodworks::domain::imperial;
using namespace woodworks::domain::types;
using namespace woodworks::domain;
using namespace woodworks::infra;

InventoryPage::InventoryPage(QWidget *parent)
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
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QSize screenSize = screen->availableGeometry().size();
        QSize windowSize(screenSize.width() * 0.6, screenSize.height() * 0.6);
        resize(windowSize);
        move((screenSize.width() - windowSize.width()) / 2,
             (screenSize.height() - windowSize.height()) / 2);
    }

    // // Setup the logical models for our mvc.
    // ui->individualLogsTable->setModel(individualLogsModel);
    // ui->groupedLogsTable->setModel(groupedLogsModel);
    // ui->cookiesTable->setModel(cookiesModel);
    // ui->lumberTable->setModel(lumberModel);
    // ui->slabsTable->setModel(slabsModel);
    // ui->firewoodTable->setModel(firewoodModel);

    // Add drying options to the combo box
    ui->dryingComboBox->addItem("Green", QVariant(static_cast<int>(Drying::GREEN)));
    ui->dryingComboBox->addItem("Air Dried", QVariant(static_cast<int>(Drying::AIR_DRIED)));
    ui->dryingComboBox->addItem("Kiln Dried", QVariant(static_cast<int>(Drying::KILN_DRIED)));
    ui->dryingComboBox->addItem("Air & Kiln Dried", QVariant(static_cast<int>(Drying::KILN_AND_AIR_DRIED)));

    buildFilterWidgets();

    refreshModels();

    // When the views/filters are changed, refresh the models.
    connect(ui->detailedViewCheckBox, &QCheckBox::stateChanged, this, &InventoryPage::refreshModels);
    connect(ui->logSpeciesComboBox, &QComboBox::currentTextChanged, this, &InventoryPage::refreshModels);
    connect(ui->logLengthMin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->logLengthMax, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->logRadiusMin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->logRadiusMax, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->logSpeciesComboBox, &QComboBox::currentTextChanged, this, &InventoryPage::refreshModels);

    connect(ui->addLogButton, &QPushButton::clicked, this, &InventoryPage::onAddLogClicked);
    connect(ui->spreadsheetImporterButton, &QPushButton::clicked, this, &InventoryPage::onSpreadsheetImportClicked);

    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle("Inventory Management");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Window);
}

InventoryPage::~InventoryPage()
{
    delete ui;
}

void InventoryPage::buildFilterWidgets() {
    // speciesComboBox has all the species in the database, pluas an "All" option that filters out nothing.
    QStringList species = getUniqueSpecies();
    species.prepend("All");
    ui->logSpeciesComboBox->addItems(species);
    ui->logSpeciesComboBox->setCurrentIndex(0); // Set to "All" by default

    QStringList dryings = getUniqueValuesOfColumn("display_logs", "Drying");
    dryings.prepend("All");
    ui->logDryingComboBox->addItems(dryings);
    ui->logDryingComboBox->setCurrentIndex(0); // Set to "All" by default
}

void InventoryPage::refreshModels()
{
    QVector<FieldFilter> logFilters;

    if (ui->logSpeciesComboBox->currentText() != "All") {
        logFilters.push_back(FieldFilter().exact("species", ui->logSpeciesComboBox->currentText()));
    }

    if (ui->logLengthMin->value() != 0 || ui->logLengthMax->value() != 0) {
        logFilters.push_back(FieldFilter().between(
            "\"Length (ft)\"",
            ui->logLengthMin->value(),
            ui->logLengthMax->value()
        ));
    }

    if (ui->logRadiusMin->value() != 0 || ui->logRadiusMax->value() != 0) {
        logFilters.push_back(FieldFilter().between(
            "\"Diameter (in)\"",
            ui->logRadiusMin->value(),
            ui->logRadiusMax->value()
        ));
    }

    if (ui->logDryingComboBox->currentText() != "All") {
        logFilters.push_back(FieldFilter().exact("drying", ui->logDryingComboBox->currentText()));
    }

    if (ui->detailedViewCheckBox->isChecked()) {
        ui->logsTableView->setModel(makeFilteredModel("display_logs", logFilters, this));
        ui->cookiesTableView->setModel(makeViewModel("display_cookies", this));
        ui->slabsTableView->setModel(makeViewModel("display_slabs", this));
        ui->lumberTableView->setModel(makeViewModel("display_lumber", this));
    } else {
        ui->logsTableView->setModel(makeFilteredModel("display_logs_grouped", logFilters, this));
        ui->cookiesTableView->setModel(makeViewModel("display_cookies_grouped", this));
        ui->slabsTableView->setModel(makeViewModel("display_slabs_grouped", this));
        ui->lumberTableView->setModel(makeViewModel("display_lumber_grouped", this));
    }
    ui->firewoodTableView->setModel(makeViewModel("display_firewood_grouped", this));
    
    ui->logsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->cookiesTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->slabsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->lumberTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->firewoodTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void InventoryPage::refreshTableViews() {
    refreshModels();
}

void InventoryPage::onAddLogClicked()
{
    Length logLen = Length::fromFeet(ui->lengthFt->value()) + Length::fromInches(ui->lengthIn->value());
    Length logDiam = Length::fromInches(ui->diameterIn->value());
    Species logSpecies = {ui->speciesEntry->text().toStdString()};
    Quality logQuality = {ui->qualityValue->value()};
    Drying logDrying = ui->dryingComboBox->currentData().value<Drying>();
    Dollar logCost = {static_cast<int>(ui->costValue->value() * 100)};
    std::string location = ui->locationEntry->text().toStdString();

    Log log = Log::uninitialized();

    log.length = logLen;
    log.diameter = logDiam;
    log.species = logSpecies;
    log.quality = logQuality;
    log.drying = logDrying;
    log.cost = logCost;
    log.location = location;

    if (!log.isValid())
    {
        QMessageBox::critical(this, "Error", "Invalid log data");
        return;
    }

    // Insert the log into the database
    auto &deebee = woodworks::infra::DbConnection::instance();
    auto repo = woodworks::infra::QtSqlRepository<Log>(deebee);

    if (!repo.add(log))
    {
        QMessageBox::critical(this, "Error", "Failed to insert log: " + deebee.lastError().text());
        return;
    }

    refreshModels();
}

void InventoryPage::onCookieButtonClicked()
{
}

void InventoryPage::onSpreadsheetImportClicked()
{
    QString filename =
        QFileDialog::getOpenFileName(this, "Import Spreadsheet", QString(),
                                     "Spreadsheets (*.csv *.xls *.xlsx)");

    if (filename.isEmpty())
        // QMessageBox::information(this, "Warning", "\nPlease ensure that it is in the following format: CSV (Comma delimited)");
        // QString filename = QFileDialog::getOpenFileName(this, "Import Spreadsheet", QString(), "Spreadsheets (*.csv)");
        // // .csv only, import one sheet at a time
        // if (filename.isEmpty())
        //     return;
        // QStringList options;
        // options << "Logs" << "Firewood" << "Slabs" << "Cookies" << "Lumber";
        // bool ok = false;
        // QString userChoice = QInputDialog::getItem(this, QObject::tr("Sheet Picker"), QObject::tr("Please select which sheet you're importing:"), options, 0, false, &ok);

        // if(!ok){
        //     std::cout << "User canceled input." << std::endl;
        //     return;
        // }
        // // TODO: Implement spreadsheet import parsing logic in logic module.
        // QMessageBox::information(this, "Import Selected", "File selected: " + filename + "\nSheet Type: " + userChoice);
        // std::string filePath = filename.toStdString();
        // Importer import;

        // if(userChoice == "Logs"){import.importLogs(filePath);}
        // else if(userChoice == "Firewood"){import.importFirewood(filePath);}
        // else if(userChoice == "Slabs"){import.importSlabs(filePath);}
        // else if(userChoice == "Cookies"){import.importCookies(filePath);}
        // else if(userChoice == "Lumber"){import.importLumber(filePath);}

        refreshModels();
}

void InventoryPage::onImageButtonClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Import Image", QString(), "Images (*.jpg *.png)");

    if (filename.isEmpty())
        return;

    // TODO: Implement spreadsheet import parsing logic in logic module.
    QMessageBox::information(this, "Import Selected",
                             "File selected: " + filename);
}

void InventoryPage::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "InventoryPage clicked at:" << event->pos()
             << "Mouse click on widgetAt:"
             << QApplication::widgetAt(QCursor::pos());

    QWidget::mousePressEvent(event);
}

bool InventoryPage::eventFilter(QObject *obj, QEvent *event)
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
