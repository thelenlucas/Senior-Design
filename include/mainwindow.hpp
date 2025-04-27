#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
  class MainWindow;
}
QT_END_NAMESPACE

class InventoryPage;
class CutlistPage;
class SalesPage;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void showInventoryPage();
  void showCutlistPage();
  void showSalesPage();

private:
  Ui::MainWindow *ui;

  InventoryPage *inventoryPage;
  CutlistPage *cutlistPage;
  SalesPage *salesPage;
};

#endif // MAINWINDOW_HPP
