#include "widgets/LumberCuttingWindow.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

using namespace woodworks::widgets;
using namespace woodworks::domain;
using namespace woodworks::domain::imperial;
using namespace woodworks::domain::lumber;

LumberCuttingWindow::LumberCuttingWindow(const LiveEdgeSlab& slab, QWidget* parent)
    : QMainWindow(parent), cutter(slab) {
    setupUi();
    updateUi();
}

LumberCuttingWindow::~LumberCuttingWindow() {}

void LumberCuttingWindow::setupUi() {
    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Trim width
    auto* trimLayout = new QHBoxLayout();
    trimLayout->addWidget(new QLabel("Trim each side (in):"));
    trimWidthSpin = new QDoubleSpinBox();
    trimWidthSpin->setSuffix("in");
    trimWidthSpin->setRange(0, cutter.slab.width.toInches()/2);
    trimLayout->addWidget(trimWidthSpin);
    mainLayout->addLayout(trimLayout);

    // Board count
    auto* countLayout = new QHBoxLayout();
    countLayout->addWidget(new QLabel("Board count:"));
    boardCountSpin = new QSpinBox();
    boardCountSpin->setRange(0, 100);
    countLayout->addWidget(boardCountSpin);
    mainLayout->addLayout(countLayout);

    // Board width
    auto* widthLayout = new QHBoxLayout();
    widthLayout->addWidget(new QLabel("Board width (in):"));
    boardWidthSpin = new QDoubleSpinBox();
    boardWidthSpin->setSuffix("in");
    boardWidthSpin->setRange(0, cutter.slab.width.toInches());
    widthLayout->addWidget(boardWidthSpin);
    mainLayout->addLayout(widthLayout);

    // Display cut summary
    cutsCountLabel = new QLabel(this);
    boardWidthLabel = new QLabel(this);
    auto* summaryLayout = new QVBoxLayout();
    summaryLayout->setSpacing(2);
    summaryLayout->setContentsMargins(0, 0, 0, 0);
    summaryLayout->addWidget(cutsCountLabel);
    summaryLayout->addWidget(boardWidthLabel);
    mainLayout->addLayout(summaryLayout);

    // Finish
    finishButton = new QPushButton("Finish Cuts");
    mainLayout->addWidget(finishButton);

    // Connections
    connect(trimWidthSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LumberCuttingWindow::onTrimWidthChanged);
    connect(boardCountSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &LumberCuttingWindow::onBoardCountChanged);
    connect(boardWidthSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LumberCuttingWindow::onBoardWidthChanged);
    connect(finishButton, &QPushButton::clicked, this, &LumberCuttingWindow::onFinishCuts);

    setWindowTitle("Lumber Cutting");
    adjustSize();
}

void LumberCuttingWindow::updateUi() {
    // sync spins
    bool block1 = trimWidthSpin->blockSignals(true);
    trimWidthSpin->setValue(cutter.trimWidth.toInches());
    trimWidthSpin->blockSignals(block1);

    bool block2 = boardCountSpin->blockSignals(true);
    boardCountSpin->setValue(cutter.boardCount);
    boardCountSpin->blockSignals(block2);

    bool block3 = boardWidthSpin->blockSignals(true);
    boardWidthSpin->setValue(cutter.boardWidth.toInches());
    boardWidthSpin->blockSignals(block3);

    // update summary labels
    cutsCountLabel->setText(QString("Boards made: %1").arg(cutter.boardCount));
    boardWidthLabel->setText(QString("Board width: %1 in").arg(cutter.boardWidth.toInches()));
}

void LumberCuttingWindow::onTrimWidthChanged(double value) {
    cutter.setTrimWidth(Length::fromInches(value));
    updateUi();
}

void LumberCuttingWindow::onBoardCountChanged(int count) {
    cutter.setBoardCount(count);
    updateUi();
}

void LumberCuttingWindow::onBoardWidthChanged(double value) {
    cutter.setBoardWidth(Length::fromInches(value));
    updateUi();
}

void LumberCuttingWindow::onFinishCuts() {
    // prepare boards then finalize
    cutter.planCuts();
    cutter.finalizeCuts();
    close();
}