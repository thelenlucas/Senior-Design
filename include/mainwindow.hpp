#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

// Forward declarations of our UI classes, not the auto generated QT wrapper classe.
// These are necessary to accomodate QT's structure. Don't remove these.
class InventoryPage;
class CutlistPage;
class SalesPage;

namespace Ui 
{
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

    // Added UI Slot declarations.
    void showInventoryPage();
    void showCutlistPage();
    void showSalesPage();

private:
    Ui::MainWindow *ui;
    //InventoryPage* inventoryPage;
    CutlistPage* cutlistPage;
    SalesPage* salesPage;
    QDockWidget* inventoryDock;

};

#endif // MAINWINDOW_HPP

