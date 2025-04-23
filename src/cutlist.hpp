#ifndef CUTLISTPAGE_H
#define CUTLISTPAGE_H

#include <QWidget>

namespace Ui {
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
    void updateProjects();
    void refreshModels();

private slots:
    void newPart();
    void deleteProject();
    void cutLog();
    void partCompleteRough();
    void partCompleteFinished();
};

#endif // CUTLISTPAGE_H