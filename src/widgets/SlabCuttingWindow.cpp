#include "widgets/SlabCuttingWindow.hpp"
#include "ui_slabCuttingWindow.h"

#include "infra/repository.hpp"
#include "domain/log.hpp"

using namespace woodworks::widgets;
using namespace woodworks::domain;
using namespace woodworks::infra;
using namespace woodworks::domain::slabs;

SlabCuttingWindow::SlabCuttingWindow(Log log, QWidget* parent) : QMainWindow(parent), ui(new Ui::slabCuttingWindow), log(Log::uninitialized()), cutter(log)
{
    this->log = log;
    ui->setupUi(this);
    updateUi();
}

SlabCuttingWindow::~SlabCuttingWindow()
{
    delete ui;
}

void SlabCuttingWindow::updateUi() {
    ui->remainingDiameterLabel->setText(QString::number(cutter.getRemainingDiameter().toQuarters()) + " quarters");
    ui->slabWidthDisplayLabel->setText(QString::number(cutter.slabWidthAtThickness(Length::fromQuarters(ui->nextSlabThicknessSpin->value())).toQuarters()) + " quarters");
}