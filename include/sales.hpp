#ifndef SALES_HPP
#define SALES_HPP

#include <QWidget>
#include <QSqlQueryModel>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class SalesPage; }
QT_END_NAMESPACE

class SalesPage : public QWidget
{
    Q_OBJECT

public:
    explicit SalesPage(QWidget* parent = nullptr);
    ~SalesPage();

private:
    Ui::SalesPage* ui;
    QSqlQueryModel* inventoryModel;

    QVector<QWidget*> itemWidgets;
    int selectedIndex;
    double runningTotal;

    void RefreshInventoryModel();
    void ResizeToDisplayPercentage(double width_ratio, double height_ratio);
    void AddSelectedInventoryRow(QString const& id, QString const& species, QString const& value);
    void ReapplyStripedBackgrounds();
    void UpdateTotal(double delta);
    bool eventFilter(QObject* obj, QEvent* event) override;

};

#endif // SALES_HPP
