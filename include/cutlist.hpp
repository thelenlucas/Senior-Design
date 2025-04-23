#pragma once

#include <QComboBox>
#include <QItemSelection>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QTableView>
#include <QWidget>
// These are for the undo history.
#include <QMap>
#include <QStack>
#include <QString>
#include <QVariant>
#include <QPoint>

namespace Ui
{
class CutlistPage;
}

class CutlistPage : public QWidget
{
    Q_OBJECT

  public:
    explicit CutlistPage(QWidget* parent = nullptr);
    ~CutlistPage();
    
  private slots:
    void newPart();
    void deleteProject();
    void cutLog();
    void partCompleteRough();
    void partCompleteFinished();
    void refreshModels();
    void cutsCustomContextMenu(const QPoint &pos);
    
    private:
    Ui::CutlistPage* ui;
    void updateProjects();
};
