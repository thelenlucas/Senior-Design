#include "widgets/SlabCuttingWindow.hpp"
#include "ui_slabCuttingWindow.h"

#include <QMessageBox>
#include <QInputDialog>

#include "infra/repository.hpp"
#include "domain/log.hpp"

using namespace woodworks::widgets;
using namespace woodworks::domain;
using namespace woodworks::infra;
using namespace woodworks::domain::slabs;

SlabCuttingWindow::SlabCuttingWindow(Log log, QWidget *parent) : QMainWindow(parent), ui(new Ui::slabCuttingWindow), log(Log::uninitialized()), cutter(log)
{
    this->log = log;
    ui->setupUi(this);
    updateUi();

    // Connect signals and slots
    connect(ui->squareOffButton, &QPushButton::clicked, this, &SlabCuttingWindow::onSquareOffButtonClicked);
    connect(ui->addCutButton, &QPushButton::clicked, this, &SlabCuttingWindow::onAddCutButtonClicked);
    connect(ui->nextSlabThicknessSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SlabCuttingWindow::onSlabThicknessChanged);
    connect(ui->finishCutButton, &QPushButton::clicked, this, &SlabCuttingWindow::onFinishCutButtonClicked);
}

SlabCuttingWindow::~SlabCuttingWindow()
{
    delete ui;
}

void SlabCuttingWindow::onSquareOffButtonClicked() {
    Length squareOffLength = Length::fromQuarters(ui->squareOffLengthQuartersSpin->value());
    if (squareOffLength > cutter.getRemainingDiameter()) {
        // Notify
        QMessageBox::warning(this, "Invalid Cut", "Square off length exceeds remaining diameter.");
        return;
    }
    
    // Perform the square off
    cutter.waste(squareOffLength);
    // Update the UI
    updateUi();
}

void SlabCuttingWindow::onAddCutButtonClicked() {
    Length slabThickness = Length::fromQuarters(ui->nextSlabThicknessSpin->value());
    if (slabThickness > cutter.getRemainingDiameter()) {
        // Notify
        QMessageBox::warning(this, "Invalid Cut", "Slab thickness exceeds remaining diameter.");
        return;
    }
    
    // Perform the cut
    cutter.addSlab(slabThickness);
    // Update the UI
    updateUi();
}

void SlabCuttingWindow::onSlabThicknessChanged() {
    updateUi();
}

void SlabCuttingWindow::updateUi()
{
    ui->remainingDiameterLabel->setText(QString::number(cutter.getRemainingDiameter().toQuarters()) + " quarters");
    ui->slabWidthDisplayLabel->setText(QString::number(cutter.slabWidthAtThickness(Length::fromQuarters(ui->nextSlabThicknessSpin->value())).toQuarters()) + " quarters");
    ui->slabWidthDisplayLabel->setText(QString::number(cutter.slabWidthAtThickness(Length::fromQuarters(ui->nextSlabThicknessSpin->value())).toInches()) + " inches");
    ui->listWidget->clear();
    ui->listWidget->addItems(QStringList::fromVector(QVector<QString>::fromStdVector(cutter.getPlannedCuts())));
    ui->nextSlabThicknessSpin->setMaximum(cutter.getRemainingDiameter().toQuarters());
}

void SlabCuttingWindow::onFinishCutButtonClicked() {
    // Get length to cut slabs to, from popup
    bool ok;
    double cutLength = QInputDialog::getDouble(this, "Cut Length", "Enter the length to cut slabs to (in):", 0, 0, log.length.toInches(), 1, &ok);

    // Get location and notes
    QString location = ui->locationLineEdit->text();
    QString notes = ui->notesLineEdit->text();

    if (ok) {
        // Perform the cuts
        auto slabs = cutter.completeCuts(Length::fromInches(cutLength), location.toStdString(), notes.toStdString());
    }

    // Display success message, and close the window
    QMessageBox::information(this, "Cuts Completed", "Slabs have been cut and saved.");
    this->close();
}