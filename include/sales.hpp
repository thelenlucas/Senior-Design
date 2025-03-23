#ifndef SALES_HPP
#define SALES_HPP

#include <QWidget>

namespace Ui 
{
    class SalesPage;
}

class SalesPage : public QWidget {
    Q_OBJECT
public:
    explicit SalesPage(QWidget *parent = nullptr);
    ~SalesPage();

private:
    Ui::SalesPage *ui;
};

#endif // SALES_HPP