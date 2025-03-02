#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void refreshModel();

private slots:
    void onEnterLogButtonClicked();
    void onScrapLogButtonClicked();
    void onFirewoodButtonClicked();
    void onProjectEditActionTriggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
