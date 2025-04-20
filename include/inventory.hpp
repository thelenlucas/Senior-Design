#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include <QSqlQueryModel>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class InventoryPage;
}
QT_END_NAMESPACE

class InventoryPage : public QWidget
{
    Q_OBJECT

  public:
    explicit InventoryPage(QWidget* parent = nullptr);
    ~InventoryPage();

  protected:
    void mousePressEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

  private slots:
    void onAddLogClicked();
    void onCookieButtonClicked();
    void onSpreadsheetImportClicked();
    void onImageButtonClicked();
    void refreshTableViews();

  private:
    void refreshModels();                               // Refreshes all models from the DB.

    Ui::InventoryPage* ui;

    // Models for each inventory view type
    QSqlQueryModel* individualLogsModel;
    QSqlQueryModel* groupedLogsModel;
    QSqlQueryModel* lumberModel;
    QSqlQueryModel* slabsModel;
    QSqlQueryModel* cookiesModel;
    QSqlQueryModel* firewoodModel;
};

#endif // INVENTORY_HPP
