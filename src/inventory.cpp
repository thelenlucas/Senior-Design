#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QScreen>
#include <QMouseEvent>
#include <QTimer>
#include <QInputDialog>

#include "inventory.hpp"
#include "ui_inventory.h"
#include "logs.hpp"
#include "cookies.hpp"

InventoryPage::InventoryPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InventoryPage)
    , individualLogsModel(new QSqlQueryModel(this))
    , groupedLogsModel(new QSqlQueryModel(this))
    , cookiesModel(new QSqlQueryModel(this))
    , lumberModel(new QSqlQueryModel(this))
    , slabsModel(new QSqlQueryModel(this))
    , firewoodModel(new QSqlQueryModel(this))
{
    ui->setupUi(this);

    // Dynamically resize window to 60% of screen size and center
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
    ui->cookiesTable->setModel(cookiesModel);
    ui->lumberTable->setModel(lumberModel);
    ui->slabsTable->setModel(slabsModel);
    ui->firewoodTable->setModel(firewoodModel);

    // Set the drying QComboBox to the enum values, and strings to match them
    ui->dryingComboBox->addItem("Air Dried", static_cast<int>(Drying::AIR_DRIED));
    ui->dryingComboBox->addItem("Kiln Dried", static_cast<int>(Drying::KILN_DRIED));
    ui->dryingComboBox->addItem("Air Dried and Kiln Dried",
                                static_cast<int>(Drying::AIR_AND_KILN_DRIED));
    ui->dryingComboBox->addItem("Air and Kiln Dried", static_cast<int>(Drying::WET));

    RefreshModels();

    connect(ui->addLogButton, &QPushButton::clicked, this, &InventoryPage::onAddLogClicked);
    connect(ui->spreadsheetImporterButton, &QPushButton::clicked, this, &InventoryPage::onSpreadsheetImportClicked);
    connect(ui->createCookieButton, &QPushButton::clicked, this, &InventoryPage::onCookieButtonClicked);

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
    individualLogsModel->setQuery("SELECT * FROM logs_view", QSqlDatabase::database());
    groupedLogsModel->setQuery("SELECT * FROM logs_view_grouped", QSqlDatabase::database());
    cookiesModel->setQuery("SELECT * FROM display_cookies", QSqlDatabase::database());
    lumberModel->setQuery("SELECT * FROM display_lumber", QSqlDatabase::database());
    slabsModel->setQuery("SELECT * FROM display_slabs", QSqlDatabase::database());
    firewoodModel->setQuery("SELECT * FROM display_firewood", QSqlDatabase::database());

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

    // Get the drying by casting back from the QComboBox value
    int dryingVal = ui->dryingComboBox->currentData().toInt();
    Drying drying = static_cast<Drying>(dryingVal);
    // Create a new log
    Log newLog(0, species.toStdString(), lenQuarters, diamQuarters,
               costQuarters, quality, drying, location);
    // Insert the log into the database
    newLog.insert();

    // Refresh the models to show the new log
    RefreshModels();
}

void InventoryPage::onCookieButtonClicked() {
    // Get the selected log id
    std::optional<Log> opt = Log::get_by_id(ui->individualLogsTable->currentIndex().siblingAtColumn(0).data().toInt());

    if (!opt) {
        QMessageBox::critical(this, "Error", "Log not found");
        return;
    }

    Log log = opt.value();

    // Dialog for the user to enter a uint for cookie thickness
    // https://doc.qt.io/qt-5/qinputdialog.html
    // Pointer, Dialog Title, Dialog Text, Initial Val, Min Val, Max Val, Trigger Val
    bool ok;    
    float enteredCut = QInputDialog::getInt(this, QObject::tr("Cookie Cutter"), QObject::tr("Please enter the desired cookie thickness (inches):"), 1, 1, log.getAvailableLength(), ok);     
    //int bladeWidth = 0.125; // Replace with non-hard-coded blade width once UI for it is complete

    if(!ok) {
        std::cout << "Cutting Cookie!" << std::endl;
        //unsigned int cutDepth = static_cast<unsigned int>(enteredCut);
        //int newLogLength = log.getLenQuarters() - enteredCut - bladeWidth;
        int enteredQuarters = enteredCut*4;

        Cookie cookie = Cookie::make_from_log(log, static_cast<unsigned int>(enteredQuarters)).at(0); 
        log.cut_length(static_cast<unsigned int>(enteredCut));
        //std::cout << "Cookie returned and log cut" << std::endl;
        if(!cookie.insert()) {
            std::cout << "ERROR: Failed to insert cookie!" << std::endl;
        }

    } else { std::cout << "User canceled input." << std::endl; }

    refreshModels();
    std::cout << "Cookies done! Models refreshed!" << std::endl;
}

void InventoryPage::onSpreadsheetImportClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Import Spreadsheet", QString(), "Spreadsheets (*.csv *.xls *.xlsx)");

    if (filename.isEmpty())
        return;

    // TODO: Implement spreadsheet import parsing logic in logic module.
    QMessageBox::information(this, "Import Selected", "File selected: " + filename);
}

void InventoryPage::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "InventoryPage clicked at:" << event->pos() << "Mouse click on widgetAt:" << QApplication::widgetAt(QCursor::pos());

    QWidget::mousePressEvent(event);
}

bool InventoryPage::eventFilter(QObject* obj, QEvent* event)
{
    qDebug() << "EVENT TYPE:" << event->type() << "on:" << obj->metaObject()->className() << "named:" << obj->objectName();

    if (event->type() == QEvent::MouseButtonPress)
    {
        qDebug() << "Mouse click intercepted on:" << obj->objectName();
    }

    return QWidget::eventFilter(obj, event);
}