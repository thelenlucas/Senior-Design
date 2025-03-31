#ifndef CUTLIST_HPP
#define CUTLIST_HPP

#include <QWidget>
#include <QSqlQueryModel>

namespace Ui
{
    class CutlistPage;
}

class CutlistPage : public QWidget {
    Q_OBJECT

public:
    explicit CutlistPage(QWidget* parent = nullptr);
    ~CutlistPage();

private:
    Ui::CutlistPage* ui;

    QSqlQueryModel* orderEntryModel;
    QSqlQueryModel* orderMarkerModel;

    void refreshModels();
    void ResizeToDisplayPercentage(double width_ratio, double height_ratio);
    void ResizeSubWindowsProportionally(unsigned int count);
};

#endif // CUTLIST_HPP
