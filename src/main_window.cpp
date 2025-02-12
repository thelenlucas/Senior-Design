#include "main_window.hpp"
#include "ui_logs.hpp"
#include <qmainwindow.h>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    UI_LogsPage* logs_page = new UI_LogsPage(this); // Only one page for now.
    setCentralWidget(logs_page);
}