#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

class InventoryPage;
class CutlistPage;
class SalesPage;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onEnterLogButtonClicked();
    void onScrapLogButtonClicked();
    void onFirewoodButtonClicked();
    void onProjectEditActionTriggered();

    void showInventoryPage();
    void showCutlistPage();
    void showSalesPage();

private:
    void refreshModel();

    Ui::MainWindow *ui;
    InventoryPage *inventoryPage;
    CutlistPage *cutlistPage;
    SalesPage *salesPage;
};

#endif // MAINWINDOW_HPP
