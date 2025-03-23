#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include <QWidget>
#include <QSqlTableModel>

namespace Ui 
{
    class InventoryPage;
}

class InventoryPage : public QWidget {
    Q_OBJECT
public:
    explicit InventoryPage(QWidget *parent = nullptr);
    ~InventoryPage();
    
signals:
    void requestDock();

private:
    Ui::InventoryPage *ui;
    QSqlTableModel *model;
    void refreshModel();
};

#endif // INVENTORY_HPP