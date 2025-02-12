#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtCore/QtCore>
#include <QtWidgets/QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = nullptr);
};

#endif // MAINWINDOW_HPP