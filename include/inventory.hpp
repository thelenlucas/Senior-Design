#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include <QWidget>
#include <QSqlQueryModel>

namespace Ui 
{
    class InventoryPage;
}

class InventoryPage : public QWidget {
    Q_OBJECT

public:
    explicit InventoryPage(QWidget *parent = nullptr);
    ~InventoryPage();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onAddLogClicked();
    void onSpreadsheetImportClicked();

private:
    Ui::InventoryPage *ui;

    QSqlQueryModel* individualLogsModel;
    QSqlQueryModel* groupedLogsModel;

    void refreshModels();
};

#endif // INVENTORY_HPP
