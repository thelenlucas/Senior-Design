#include "mainwindow.h"
#include "ui_main.h"  // This header is generated automatically from res/ui/main.ui

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // Set up the UI defined in main.ui
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
