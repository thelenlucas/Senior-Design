#ifndef CUTLIST_HPP
#define CUTLIST_HPP

#include <QWidget>
#include <QSqlTableModel>

namespace Ui 
{
    class CutlistPage;
}

class CutlistPage : public QWidget 
{
    Q_OBJECT
public:
    explicit CutlistPage(QWidget *parent = nullptr);
    ~CutlistPage();

private:
    Ui::CutlistPage *ui;
    QSqlTableModel *model;
    void refreshModel();
};

#endif // CUTLIST_HPP