#ifndef SALES_HPP
#define SALES_HPP

#include <QWidget>
#include <QSqlQueryModel>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class SalesPage; }
QT_END_NAMESPACE

class SalesPage : public QWidget {
    Q_OBJECT

public:
    explicit SalesPage(QWidget* parent = nullptr);
    ~SalesPage();

private:
    Ui::SalesPage* ui;

    QSqlQueryModel* inventoryModel;

    void RefreshInventoryModel();
    void ResizeToDisplayPercentage(double width_ratio, double height_ratio);
    void AddSelectedInventoryRow(const QString& labelText);
};

#endif // SALES_HPP
