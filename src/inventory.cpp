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

#include "inventory.hpp"
#include "csv_importer.hpp"
#include "ui_inventory.h"

#include "infra/mappers/view_helpers.hpp"
#include "domain/units.hpp"
#include "domain/types.hpp"
#include "domain/log.hpp"

#include "infra/connection.hpp"
#include "infra/repository.hpp"

using namespace woodworks::domain::imperial;
using namespace woodworks::domain::types;
using namespace woodworks::domain;

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

    // Dynamically resize window to 60% of screen size and center
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QSize screenSize = screen->availableGeometry().size();
        QSize windowSize(screenSize.width() * 0.6, screenSize.height() * 0.6);
        resize(windowSize);
        move((screenSize.width() - windowSize.width()) / 2, (screenSize.height() - windowSize.height()) / 2);
    }

    // Set up the drying dropdown
    ui->dryingComboBox->addItem("Green", QVariant::fromValue(Drying::GREEN));
    ui->dryingComboBox->addItem("Kiln Dried", QVariant::fromValue(Drying::KILN_DRIED));
    ui->dryingComboBox->addItem("Air Dried", QVariant::fromValue(Drying::AIR_DRIED));
    ui->dryingComboBox->addItem("Kiln & Air Dried", QVariant::fromValue(Drying::KILN_AND_AIR_DRIED));
    ui->dryingComboBox->setCurrentIndex(0);

    refreshModels();

    connect(ui->addLogButton, &QPushButton::clicked, this, &InventoryPage::onAddLogClicked);
    connect(ui->spreadsheetImporterButton, &QPushButton::clicked, this, &InventoryPage::onSpreadsheetImportClicked);
    connect(ui->createCookieButton, &QPushButton::clicked, this, &InventoryPage::onCookieButtonClicked);
    connect(ui->insertImageButton, &QPushButton::clicked, this, &InventoryPage::onImageButtonClicked);

    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle("Inventory Management");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Window);
}

InventoryPage::~InventoryPage()
{
    delete ui;
}

void InventoryPage::refreshModels()
{
    auto* individualLogsModel = woodworks::infra::mappers::makeViewModel("display_logs", this);
    auto* groupedLogsModel = woodworks::infra::mappers::makeViewModel("display_logs_grouped", this);
    auto* cookiesModel = woodworks::infra::mappers::makeViewModel("display_cookies", this);
    auto* lumberModel = woodworks::infra::mappers::makeViewModel("display_lumber", this);
    auto* slabsModel = woodworks::infra::mappers::makeViewModel("display_slabs", this);
    auto* firewoodModel = woodworks::infra::mappers::makeViewModel("display_firewood", this);

    ui->individualLogsTable->setModel(individualLogsModel);
    ui->groupedLogsTable->setModel(groupedLogsModel);
    ui->cookiesTable->setModel(cookiesModel);
    ui->lumberTable->setModel(lumberModel);
    ui->slabsTable->setModel(slabsModel);
    ui->firewoodTable->setModel(firewoodModel);

    ui->individualLogsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->groupedLogsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->cookiesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->lumberTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->slabsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->firewoodTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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
    auto& deebee = woodworks::infra::DbConnection::instance();
    auto repo = woodworks::infra::QtSqlRepository<Log>(deebee);

    if (!repo.add(log))
    {
        QMessageBox::critical(this, "Error", "Failed to insert log: " + deebee.lastError().text());
        return;
    }

    refreshModels();
}

void InventoryPage::onCookieButtonClicked() {
    // // Get the selected log id
    // std::optional<Log> opt = Log::get_by_id(ui->individualLogsTable->currentIndex().siblingAtColumn(0).data().toInt());
    // if (!opt) {
    //     QMessageBox::critical(this, "Error", "Log not found");
    //     return;
    // }

    // Log log = opt.value();

    // // Dialog for the user to enter a uint for cookie thickness
    // // https://doc.qt.io/qt-5/qinputdialog.html
    // // Pointer, Dialog Title, Dialog Text, Initial Val, Min Val, Max Val, Trigger Val
    // bool ok;    
    // float enteredCut = QInputDialog::getInt(this, QObject::tr("Cookie Cutter"), QObject::tr("Please enter the desired cookie thickness (inches):"), 1, 1, log.getAvailableLength(), ok);     
    // if(!ok) {
    //     std::cout << "Cutting Cookie!" << std::endl;
    //     int enteredQuarters = enteredCut*4;

    //     Cookie cookie = Cookie::make_from_log(log, static_cast<unsigned int>(enteredQuarters)).at(0); 
    //     if(!cookie.insert()) {
    //         std::cout << "ERROR: Failed to insert cookie!" << std::endl;
    //     }
    // } else { std::cout << "User canceled input." << std::endl; }

    // refreshModels();
    // std::cout << "Cookies done! Models refreshed!" << std::endl;
}

void InventoryPage::onSpreadsheetImportClicked()
{
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

    if(filename.isEmpty())
        return;

    // TODO: Implement image importing logic - Patrick I leave the rest to you!
    QMessageBox::information(this, "Image Selected", "File selected: " + filename);
}

void InventoryPage::mousePressEvent(QMouseEvent *event)
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