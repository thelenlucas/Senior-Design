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
#include <QMenu>

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
#include "infra/images.hpp"

#include "widgets/SlabCuttingWindow.hpp"
#include "widgets/slabSurfacingPopup.hpp"
#include "widgets/dryingPopup.hpp"

using namespace woodworks::domain::imperial;
using namespace woodworks::domain::types;
using namespace woodworks::domain;
using namespace woodworks::infra;
using namespace woodworks::widgets;

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

    // Killed dynamic resizing

    // Add drying options to the combo box
    ui->logEntryLogDryingComboBox->addItem("Green", QVariant(static_cast<int>(Drying::GREEN)));
    ui->logEntryLogDryingComboBox->addItem("Air Dried", QVariant(static_cast<int>(Drying::AIR_DRIED)));
    ui->logEntryLogDryingComboBox->addItem("Kiln Dried", QVariant(static_cast<int>(Drying::KILN_DRIED)));
    ui->logEntryLogDryingComboBox->addItem("Air & Kiln Dried", QVariant(static_cast<int>(Drying::KILN_AND_AIR_DRIED)));

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
    connect(ui->logDryingComboBox, &QComboBox::currentTextChanged, this, &InventoryPage::refreshModels);
    connect(ui->cookiesSpeciesCombo, &QComboBox::currentTextChanged, this, &InventoryPage::refreshModels);
    connect(ui->cookieThicknessSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->cookieThicknessMaxSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->cookieDiameterMinSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->cookieDiameterMaxSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->cookieDryingCombo, &QComboBox::currentTextChanged, this, &InventoryPage::refreshModels);
    connect(ui->slabDryingCombo, &QComboBox::currentTextChanged, this, &InventoryPage::refreshModels);
    connect(ui->slabsSpeciesCombo, &QComboBox::currentTextChanged, this, &InventoryPage::refreshModels);
    connect(ui->slabLengthMin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->slabLengthMax, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->slabWidthMin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->slabWidthMax, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->slabThicknessMin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->slabThicknessMax, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->lumberSpeciesCombo, &QComboBox::currentTextChanged, this, &InventoryPage::refreshModels);
    connect(ui->lumberThicknessCombo, &QComboBox::currentTextChanged, this, &InventoryPage::refreshModels);
    connect(ui->lumberDryingCombo, &QComboBox::currentTextChanged, this, &InventoryPage::refreshModels);
    connect(ui->lumberWidthMin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->lumberWidthMax, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->lumberLengthMin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->lumberLengthMax, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->lumberThicknessCombo, &QComboBox::currentTextChanged, this, &InventoryPage::refreshModels);
    connect(ui->lumberSurfacingCombo, &QComboBox::currentTextChanged, this, &InventoryPage::refreshModels);
    connect(ui->lumberLengthMin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->lumberLengthMax, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InventoryPage::refreshModels);
    connect(ui->firewoodSpeciesCombo, &QComboBox::currentTextChanged, this, &InventoryPage::refreshModels);
    connect(ui->firewoodDryingCombo, &QComboBox::currentTextChanged, this, &InventoryPage::refreshModels);

    connect(ui->addLogButton, &QPushButton::clicked, this, &InventoryPage::onAddLogClicked);
    connect(ui->spreadsheetImporterButton, &QPushButton::clicked, this, &InventoryPage::onSpreadsheetImportClicked);

    // Bind the double clicks
    connect(ui->logsTableView, &QTableView::doubleClicked, this, &InventoryPage::onDoubleClickLogTable);
    connect(ui->cookiesTableView, &QTableView::doubleClicked, this, &InventoryPage::onDoubleClickCookieTable);
    connect(ui->slabsTableView, &QTableView::doubleClicked, this, &InventoryPage::onDoubleClickSlabTable);
    connect(ui->lumberTableView, &QTableView::doubleClicked, this, &InventoryPage::onDoubleClickSlabTable);

    // Context menu policy
    ui->logsTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->slabsTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->cookiesTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->lumberTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->firewoodTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    // Bind context menu handlers
    connect(ui->logsTableView, &QWidget::customContextMenuRequested, this, &InventoryPage::logsCustomContextMenu);
    connect(ui->slabsTableView, &QWidget::customContextMenuRequested, this, &InventoryPage::slabsCustomContextMenu);
    connect(ui->cookiesTableView, &QWidget::customContextMenuRequested, this, &InventoryPage::cookiesCustomContextMenu);
    connect(ui->lumberTableView, &QWidget::customContextMenuRequested, this, &InventoryPage::lumberCustomContextMenu);
    connect(ui->firewoodTableView, &QWidget::customContextMenuRequested, this, &InventoryPage::firewoodCustomContextMenu);

    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle("Inventory Management");
    setWindowFlags(Qt::Window);
}

InventoryPage::~InventoryPage()
{
    delete ui;
}

void InventoryPage::slabsCustomContextMenu(const QPoint &pos) {
    // Surfacing
    QModelIndex index = ui->slabsTableView->indexAt(pos);
    if (!index.isValid()) {
        std::cout << "Invalid index" << std::endl;
        return;
    }
    if (!ui->detailedViewCheckBox->isChecked()) {
        return;
    }

    QMenu contextMenu;
    contextMenu.addAction("Surface Board", [this, index]() {
        auto slab = QtSqlRepository<LiveEdgeSlab>::spawn().get(index.sibling(index.row(), 0).data().toInt());
        slabSurfacingPopUp(slab.value());
    });

    // Dry board
    contextMenu.addAction("Dry Board", [this, index]() {
        auto slab = QtSqlRepository<LiveEdgeSlab>::spawn().get(index.sibling(index.row(), 0).data().toInt());
        dryingPopUp(slab.value());
    });

    contextMenu.exec(ui->slabsTableView->viewport()->mapToGlobal(pos));
}

void InventoryPage::logsCustomContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->logsTableView->indexAt(pos);
    if (!index.isValid()) {
        std::cout << "Invalid index" << std::endl;
        return;
    }

    if (!ui->detailedViewCheckBox->isChecked())
    {
        QMenu contextMenu;
        contextMenu.addAction("Filter similar logs", [this, index]() {
            // Hijack the double click event
            onDoubleClickLogTable(index);
        });
        contextMenu.exec(ui->logsTableView->viewport()->mapToGlobal(pos));
    } else {
        QMenu contextMenu;
        contextMenu.addAction("View Image", [this, index]() {
            // Get id from the first column of the row selected
            int logId = index.sibling(index.row(), 0).data().toInt();
            auto log = QtSqlRepository<Log>::spawn().get(logId);
            if (log) {
                // Show the image
                viewImagePopup(*log, this);
            }
        });

        // Dry log
        contextMenu.addAction("Dry Log", [this, index]() {
            // Get the log ID from the model
            int logId = index.sibling(index.row(), 0).data().toInt();
            // Get the log from the database
            auto log = QtSqlRepository<Log>::spawn().get(logId);
            if (log) {
                // Show a dialog to select the drying state
                dryingPopUp(*log);
                refreshModels();
            }
        });

        contextMenu.addAction("Scrap Log", [this, index]() {
            // Get the log ID from the model
            int logId = index.sibling(index.row(), 0).data().toInt();
            // Get the log from the database
            auto log = QtSqlRepository<Log>::spawn().get(logId);
            if (log) {
                // Show a confirmation dialog
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "Confirm", "Are you sure you want to scrap this log?",
                                              QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    // Scrap the log
                    QtSqlRepository<Log>::spawn().remove(logId);
                    refreshModels();
                }
            }
        });

        contextMenu.addAction("Cut Cookie", [this, index]() {
            // Get the log ID from the model
            int logId = index.sibling(index.row(), 0).data().toInt();
            std::cout << "Log ID: " << logId << std::endl;
            // Get the log from the database
            auto log = QtSqlRepository<Log>::spawn().get(logId);
            if (log) {
                // Show a dialog to cut a cookie
                bool ok;
                double length = QInputDialog::getDouble(this, "Cut Cookie", "Enter length (in):", 0, 0, log.value().length.toInches(), 2, &ok);
                if (ok) {
                    log->cutCookie(Length::fromInches(length));
                    refreshModels();
                }
            }
        });

        contextMenu.addAction("Cut Slabs", [this, index]() {
            auto log = QtSqlRepository<Log>::spawn().get(index.sibling(index.row(), 0).data().toInt());
            if (log) {
                SlabCuttingWindow *slabCuttingWindow = new SlabCuttingWindow(*log, this);
                slabCuttingWindow->setAttribute(Qt::WA_DeleteOnClose);
                slabCuttingWindow->show();
            }
        });

        contextMenu.addAction("Break Firewood", [this, index]() {
            // Get the log ID from the model
            int logId = index.sibling(index.row(), 0).data().toInt();
            std::cout << "Log ID: " << logId << std::endl;
            // Get the log from the database
            auto log = QtSqlRepository<Log>::spawn().get(logId);
            if (log) {
                // Show a dialog to cut firewood
                bool ok;
                double length = QInputDialog::getDouble(this, "Cut Firewood", "Enter length (in):", 0, 0, log.value().length.toInches(), 2, &ok);
                if (ok) {
                    log->cutFirewood(Length::fromInches(length));
                    refreshModels();
                }
            }
        });

        contextMenu.exec(ui->logsTableView->viewport()->mapToGlobal(pos));
    }
}

void InventoryPage::cookiesCustomContextMenu(const QPoint& pos) {
    QModelIndex index = ui->cookiesTableView->indexAt(pos);
    if (!index.isValid() || !ui->detailedViewCheckBox->isChecked()) return;
    QMenu contextMenu;
    contextMenu.addAction("Dry Cookie", [this, index]() {
        int id = index.sibling(index.row(), 0).data().toInt();
        auto cookie = QtSqlRepository<Cookie>::spawn().get(id);
        if (cookie) { dryingPopUp(*cookie); refreshModels(); }
    });
    contextMenu.exec(ui->cookiesTableView->viewport()->mapToGlobal(pos));
}

void InventoryPage::lumberCustomContextMenu(const QPoint& pos) {
    QModelIndex index = ui->lumberTableView->indexAt(pos);
    if (!index.isValid() || !ui->detailedViewCheckBox->isChecked()) return;
    QMenu contextMenu;
    contextMenu.addAction("Dry Lumber", [this, index]() {
        int id = index.sibling(index.row(), 0).data().toInt();
        auto lumber = QtSqlRepository<Lumber>::spawn().get(id);
        if (lumber) { dryingPopUp(*lumber); refreshModels(); }
    });
    contextMenu.exec(ui->lumberTableView->viewport()->mapToGlobal(pos));
}

void InventoryPage::firewoodCustomContextMenu(const QPoint& pos) {
    QModelIndex index = ui->firewoodTableView->indexAt(pos);
    if (!index.isValid()) return;
    QMenu contextMenu;
    contextMenu.addAction("Dry Firewood", [this, index]() {
        int id = index.sibling(index.row(), 0).data().toInt();
        auto fw = QtSqlRepository<Firewood>::spawn().get(id);
        if (fw) { dryingPopUp(*fw); refreshModels(); }
    });
    contextMenu.exec(ui->firewoodTableView->viewport()->mapToGlobal(pos));
}

void InventoryPage::buildFilterWidgets()
{
    // speciesComboBox has all the species in the database, pluas an "All" option that filters out nothing.
    QStringList species = getUniqueSpecies(); // Doesn't have an "All" option
    ui->logEntrySpeciesCombo->addItems(species);
    ui->logEntrySpeciesCombo->setCurrentIndex(0);
    species.prepend("All");
    ui->logSpeciesComboBox->addItems(species);
    ui->cookiesSpeciesCombo->addItems(species);
    ui->slabsSpeciesCombo->addItems(species);
    ui->lumberSpeciesCombo->addItems(species);
    ui->firewoodSpeciesCombo->addItems(species);
    // Set to "All" by default
    ui->logSpeciesComboBox->setCurrentIndex(0);
    ui->cookiesSpeciesCombo->setCurrentIndex(0);
    ui->slabsSpeciesCombo->setCurrentIndex(0);
    ui->lumberSpeciesCombo->setCurrentIndex(0);
    ui->firewoodSpeciesCombo->setCurrentIndex(0);

    QStringList dryings = getUniqueDryingOptions();
    dryings.prepend("All");
    ui->logDryingComboBox->addItems(dryings);
    ui->cookieDryingCombo->addItems(dryings);
    ui->slabDryingCombo->addItems(dryings);
    ui->lumberDryingCombo->addItems(dryings);
    ui->firewoodDryingCombo->addItems(dryings);
    // Set to "All" by default
    ui->logDryingComboBox->setCurrentIndex(0);
    ui->cookieDryingCombo->setCurrentIndex(0);
    ui->slabDryingCombo->setCurrentIndex(0);
    ui->lumberDryingCombo->setCurrentIndex(0);
    ui->firewoodDryingCombo->setCurrentIndex(0);

    // Surfacing options for the slabs are different than the ones for the lumber
    ui->slabSurfacingCombo->addItem("All");
    ui->slabSurfacingCombo->addItems(getUniqueValuesOfColumn("display_slabs", "Surfacing"));
    ui->slabSurfacingCombo->setCurrentIndex(0);
    ui->lumberSurfacingCombo->addItem("All");
    ui->lumberSurfacingCombo->addItems(getUniqueValuesOfColumn("display_lumber", "Surfacing"));
    ui->lumberSurfacingCombo->setCurrentIndex(0);

    // Since we're displaying lumber thickness in 'quarters'/4, we combo box it instead
    ui->lumberThicknessCombo->addItem("All");
    ui->lumberThicknessCombo->addItems(getUniqueValuesOfColumn("display_lumber", "Thickness"));
    ui->lumberThicknessCombo->setCurrentIndex(0);

    // Unique locations for log entry
    QStringList locations = getUniqueLocations();
    ui->logEntryLocationCombo->addItems(locations);
}

void InventoryPage::refreshModels()
{
    QVector<FieldFilter> logFilters;
    QVector<FieldFilter> cookieFilters;
    QVector<FieldFilter> slabsFilter;
    QVector<FieldFilter> lumberFilters;
    QVector<FieldFilter> firewoodFilters;

    if (ui->logSpeciesComboBox->currentText() != "All")
    {
        logFilters.push_back(FieldFilter().exact("species", ui->logSpeciesComboBox->currentText()));
    }

    if (ui->logLengthMin->value() != 0 || ui->logLengthMax->value() != 0)
    {
        logFilters.push_back(FieldFilter().between(
            "\"Length (ft)\"",
            ui->logLengthMin->value(),
            ui->logLengthMax->value()));
    }

    if (ui->logRadiusMin->value() != 0 || ui->logRadiusMax->value() != 0)
    {
        logFilters.push_back(FieldFilter().between(
            "\"Diameter (in)\"",
            ui->logRadiusMin->value(),
            ui->logRadiusMax->value()));
    }

    if (ui->logDryingComboBox->currentText() != "All")
    {
        logFilters.push_back(FieldFilter().exact("drying", ui->logDryingComboBox->currentText()));
    }

    if (ui->cookiesSpeciesCombo->currentText() != "All")
    {
        cookieFilters.push_back(FieldFilter().exact("species", ui->cookiesSpeciesCombo->currentText()));
    }
    if (ui->cookieThicknessSpinBox->value() != 0 || ui->cookieThicknessMaxSpinBox->value() != 0)
    {
        cookieFilters.push_back(FieldFilter().between(
            "\"Thickness (in)\"",
            ui->cookieThicknessSpinBox->value(),
            ui->cookieThicknessMaxSpinBox->value()));
    }
    if (ui->cookieDiameterMinSpinBox->value() != 0 || ui->cookieDiameterMaxSpinBox->value() != 0)
    {
        cookieFilters.push_back(FieldFilter().between(
            "\"Diameter (in)\"",
            ui->cookieDiameterMinSpinBox->value(),
            ui->cookieDiameterMaxSpinBox->value()));
    }
    if (ui->cookieDryingCombo->currentText() != "All")
    {
        cookieFilters.push_back(FieldFilter().exact("drying", ui->cookieDryingCombo->currentText()));
    }

    if (ui->slabsSpeciesCombo->currentText() != "All")
    {
        slabsFilter.push_back(FieldFilter().exact("species", ui->slabsSpeciesCombo->currentText()));
    }
    if (ui->slabLengthMin->value() != 0 || ui->slabLengthMax->value() != 0)
    {
        slabsFilter.push_back(FieldFilter().between(
            "\"Length (in)\"",
            ui->slabLengthMin->value(),
            ui->slabLengthMax->value()));
    }
    if (ui->slabWidthMax->value() != 0 || ui->slabWidthMin->value() != 0)
    {
        slabsFilter.push_back(FieldFilter().between(
            "\"Width (in)\"",
            ui->slabWidthMin->value(),
            ui->slabWidthMax->value()));
    }
    if (ui->slabThicknessMin->value() != 0 || ui->slabThicknessMax->value() != 0)
    {
        slabsFilter.push_back(FieldFilter().between(
            "\"Thickness (in)\"",
            ui->slabThicknessMin->value(),
            ui->slabThicknessMax->value()));
    }
    if (ui->slabDryingCombo->currentText() != "All")
    {
        slabsFilter.push_back(FieldFilter().exact("drying", ui->slabDryingCombo->currentText()));
    }
    if (ui->slabSurfacingCombo->currentText() != "All")
    {
        slabsFilter.push_back(FieldFilter().exact("surfacing", ui->slabSurfacingCombo->currentText()));
    }

    if (ui->lumberSpeciesCombo->currentText() != "All")
    {
        lumberFilters.push_back(FieldFilter().exact("species", ui->lumberSpeciesCombo->currentText()));
    }
    if (ui->lumberLengthMin->value() != 0 || ui->lumberLengthMax->value() != 0)
    {
        lumberFilters.push_back(FieldFilter().between(
            "\"Length (in)\"",
            ui->lumberLengthMin->value(),
            ui->lumberLengthMax->value()));
    }
    if (ui->lumberWidthMin->value() != 0 || ui->lumberWidthMax->value() != 0)
    {
        lumberFilters.push_back(FieldFilter().between(
            "\"Width (in)\"",
            ui->lumberWidthMin->value(),
            ui->lumberWidthMax->value()));
    }
    if (ui->lumberThicknessCombo->currentText() != "All")
    {
        lumberFilters.push_back(FieldFilter().exact("thickness", ui->lumberThicknessCombo->currentText()));
    }
    if (ui->lumberDryingCombo->currentText() != "All")
    {
        lumberFilters.push_back(FieldFilter().exact("drying", ui->lumberDryingCombo->currentText()));
    }
    if (ui->lumberSurfacingCombo->currentText() != "All")
    {
        lumberFilters.push_back(FieldFilter().exact("surfacing", ui->lumberSurfacingCombo->currentText()));
    }

    if (ui->firewoodSpeciesCombo->currentText() != "All")
    {
        firewoodFilters.push_back(FieldFilter().exact("species", ui->firewoodSpeciesCombo->currentText()));
    }
    if (ui->firewoodDryingCombo->currentText() != "All")
    {
        firewoodFilters.push_back(FieldFilter().exact("drying", ui->firewoodDryingCombo->currentText()));
    }

    if (ui->detailedViewCheckBox->isChecked())
    {
        ui->logsTableView->setModel(makeFilteredModel("display_logs", logFilters, this));
        ui->cookiesTableView->setModel(makeFilteredModel("display_cookies", cookieFilters, this));
        ui->slabsTableView->setModel(makeFilteredModel("display_slabs", slabsFilter, this));
        ui->lumberTableView->setModel(makeFilteredModel("display_lumber", lumberFilters, this));
    }
    else
    {
        ui->logsTableView->setModel(makeFilteredModel("display_logs_grouped", logFilters, this));
        ui->cookiesTableView->setModel(makeFilteredModel("display_cookies_grouped", cookieFilters, this));
        ui->slabsTableView->setModel(makeFilteredModel("display_slabs_grouped", slabsFilter, this));
        ui->lumberTableView->setModel(makeFilteredModel("display_lumber_grouped", lumberFilters, this));
    }
    ui->firewoodTableView->setModel(makeFilteredModel("display_firewood_grouped", firewoodFilters, this));

    ui->logsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->cookiesTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->slabsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->lumberTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->firewoodTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void InventoryPage::refreshTableViews()
{
    refreshModels();
}

void InventoryPage::onAddLogClicked()
{
    Length logLen = Length::fromFeet(ui->lengthFt->value()) + Length::fromInches(ui->lengthIn->value());
    Length logDiam = Length::fromInches(ui->logEntryDiameterSpin->value());
    Species logSpecies = {ui->logEntrySpeciesCombo->currentText().toStdString()};
    Quality logQuality = {ui->logEntryLogQuality->value()};
    Drying logDrying = ui->logEntryLogDryingComboBox->currentData().value<Drying>();
    Dollar logCost = {static_cast<int>(ui->logEntryLogCost->value() * 100)};
    std::string location = ui->logEntryLocationCombo->currentText().toStdString();

    Log log = Log::uninitialized();

    for (int i = 0; i < ui->logEntryLogCountSpin->value(); i++)
    {
        log.length = logLen;
        log.diameter = logDiam;
        log.species = logSpecies;
        log.quality = logQuality;
        log.drying = logDrying;
        log.cost = logCost;
        log.location = location;
        log.notes = ui->logEntryNotes->text().toStdString();

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
    }

    refreshModels();
}

void InventoryPage::onDoubleClickLogTable(const QModelIndex &index)
{
    // If not in detailed view, set the species and drying to the selected row, and then move
    // to the detailed view.
    if (!ui->detailedViewCheckBox->isChecked())
    {
        // Species - Column 1 (2nd), Drying - Column 5 (6th)
        QString species = index.sibling(index.row(), 1).data().toString();
        QString drying = index.sibling(index.row(), 5).data().toString();
        ui->logSpeciesComboBox->setCurrentText(species);
        ui->logDryingComboBox->setCurrentText(drying);
        ui->detailedViewCheckBox->setChecked(true);
        refreshModels();
    }
    else
    {
        // Otherwise, bring up the image dialog for the selected log.
        // Id is first column (0th)
        int id = index.sibling(index.row(), 0).data().toInt();
        auto log = QtSqlRepository<Log>::spawn().get(id).value();
        viewImagePopup(log, this);
    }
}
void InventoryPage::onDoubleClickCookieTable(const QModelIndex &index)
{
    // Same as above, but for cookies - species on 1, drying on 4, id on 0
    if (!ui->detailedViewCheckBox->isChecked())
    {
        QString species = index.sibling(index.row(), 1).data().toString();
        QString drying = index.sibling(index.row(), 4).data().toString();
        ui->cookiesSpeciesCombo->setCurrentText(species);
        ui->cookieDryingCombo->setCurrentText(drying);
        ui->detailedViewCheckBox->setChecked(true);
        refreshModels();
    }
    else
    {
        int id = index.sibling(index.row(), 0).data().toInt();
        auto cookie = QtSqlRepository<Cookie>::spawn().get(id).value();
        viewImagePopup(cookie, this);
    }
}
void InventoryPage::onDoubleClickSlabTable(const QModelIndex &index)
{
    // Slabs are on species (1), drying (5), surfacing (6) and id (0)
    if (!ui->detailedViewCheckBox->isChecked())
    {
        QString species = index.sibling(index.row(), 1).data().toString();
        QString drying = index.sibling(index.row(), 5).data().toString();
        QString surfacing = index.sibling(index.row(), 6).data().toString();
        ui->slabsSpeciesCombo->setCurrentText(species);
        ui->slabDryingCombo->setCurrentText(drying);
        ui->slabSurfacingCombo->setCurrentText(surfacing);
        ui->detailedViewCheckBox->setChecked(true);
        refreshModels();
    }
    else
    {
        int id = index.sibling(index.row(), 0).data().toInt();
        auto slab = QtSqlRepository<LiveEdgeSlab>::spawn().get(id).value();
        viewImagePopup(slab, this);
    }
}
void InventoryPage::onDoubleClickLumberTable(const QModelIndex &index)
{
    // Lumber on species (1), thickness (3), drying (5), surfacing (6) and id (0)
    if (!ui->detailedViewCheckBox->isChecked())
    {
        QString species = index.sibling(index.row(), 1).data().toString();
        QString thickness = index.sibling(index.row(), 3).data().toString();
        QString drying = index.sibling(index.row(), 5).data().toString();
        QString surfacing = index.sibling(index.row(), 6).data().toString();
        ui->lumberSpeciesCombo->setCurrentText(species);
        ui->lumberThicknessCombo->setCurrentText(thickness);
        ui->lumberDryingCombo->setCurrentText(drying);
        ui->lumberSurfacingCombo->setCurrentText(surfacing);
        ui->detailedViewCheckBox->setChecked(true);
        refreshModels();
    }
    else
    {
        int id = index.sibling(index.row(), 0).data().toInt();
        auto lumber = QtSqlRepository<Lumber>::spawn().get(id).value();
        viewImagePopup(lumber, this);
    }
}

void InventoryPage::onCookieButtonClicked()
{
}

void InventoryPage::onSpreadsheetImportClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Import Spreadsheet", QString(), "Spreadsheets (*.csv)");

    if (filename.isEmpty()){return;}

    QStringList options;
    options << "Logs" << "Firewood" << "Slabs" << "Cookies" << "Lumber";
    bool ok = false;
    QString userChoice = QInputDialog::getItem(this, QObject::tr("Sheet Picker"), QObject::tr("Please select which sheet you're importing:"), options, 0, false, &ok);

    if(!ok){
        std::cout << "User canceled input." << std::endl;
        return;
    }
    QMessageBox::information(this, "Import Selected", "File selected: " + filename + "\nSheet Type: " + userChoice);
    std::string filePath = filename.toStdString();
    Importer import;

    if(userChoice == "Logs"){import.importLogs(filePath);}
    else if(userChoice == "Firewood"){import.importFirewood(filePath);}
    else if(userChoice == "Slabs"){import.importSlabs(filePath);}
    else if(userChoice == "Cookies"){import.importCookies(filePath);}
    else if(userChoice == "Lumber"){import.importLumber(filePath);}

    refreshModels();
}

void InventoryPage::onImageButtonClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Import Image", QString(), "Images (*.jpg *.png)");

    if (filename.isEmpty())
        return;

    // TODO: Implement spreadsheet import parsing logic in logic module.
    QMessageBox::information(this, "Import Selected", "File selected: " + filename);
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
