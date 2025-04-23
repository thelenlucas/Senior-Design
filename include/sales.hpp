#pragma once

#include "sales/product.hpp"

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

private slots:
    void onAddItemButtonClicked();

private:
    Ui::SalesPage* ui;
    void addProduct(woodworks::sales::Product product);
};
