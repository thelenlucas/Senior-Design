#pragma once
#include <QMainWindow>
#include "domain/live_edge_slab.hpp"
#include "domain/lumber_cutter.hpp"
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

namespace woodworks::widgets
{

    class LumberCuttingWindow : public QMainWindow
    {
        Q_OBJECT
    public:
        explicit LumberCuttingWindow(const woodworks::domain::LiveEdgeSlab &slab, QWidget *parent = nullptr);
        ~LumberCuttingWindow();

    private slots:
        void onTrimWidthChanged(double value);
        void onBoardCountChanged(int count);
        void onBoardWidthChanged(double value);
        void onFinishCuts();

    private:
        woodworks::domain::lumber::LumberCutter cutter;
        QDoubleSpinBox *trimWidthSpin;
        QSpinBox *boardCountSpin;
        QDoubleSpinBox *boardWidthSpin;
        QLabel *cutsCountLabel;
        QLabel *boardWidthLabel;
        QPushButton *finishButton;
        void setupUi();
        void updateUi();
    };

}