#include "widgets/log_inventory.hpp"
#include "ui_log_inventory.h"

#include "infra/repository.hpp"
#include "domain/log.hpp"

using namespace woodworks::widgets;
using namespace woodworks::domain;
using namespace woodworks::infra;

LogInventory::LogInventory(QWidget *parent) : QWidget(parent),
    ui(new Ui::logInventoryWidget)
{
    ui->setupUi(this);

    refreshModel();
}

LogInventory::~LogInventory()
{
    delete ui;
}

void LogInventory::onDatabaseUpdate()
{
    refreshModel();
}

void LogInventory::refreshModel()
{
    auto repo = QtSqlRepository<Log>::spawn();

    auto* individualLogsModel = infra::mappers::makeViewModel("individual_logs", this);
    auto* groupedLogsModel = infra::mappers::makeViewModel("grouped_logs", this);

    ui->individualLogTableView->setModel(individualLogsModel);
    ui->groupedLogsTableView->setModel(groupedLogsModel);

    ui->individualLogTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->groupedLogsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}