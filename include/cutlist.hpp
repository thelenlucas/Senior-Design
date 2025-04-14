#ifndef CUTLIST_HPP
#define CUTLIST_HPP

#include <QComboBox>
#include <QItemSelection>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QTableView>
#include <QWidget>

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

  protected:
    void showEvent(QShowEvent* event) override;

  private slots:
    // Standard cut actions
    void OnCutCookie();
    void OnCutSlab();
    void OnCutLumber();

    // Project-based actions
    void OnDeleteProject();
    void OnProjectSelectionChanged(int index);
    void OnCompletePartCut();
    void OnUndoLastCut();
    void OnAdjustCutLength();

  private:
    void refreshModels();
    void ResizeToDisplayPercentage(double width_ratio, double height_ratio);
    void ResizeSubWindowsProportionally(unsigned int count);

    void InitializeStandardCutButtons();
    void InitializeProjectSelector();
    void RefreshProjectList();
    void RefreshPartsTable();
    void RefreshMatchingLogsTable();
    void PopulateMatchingLogsForSelectedPart();

    Ui::CutlistPage* ui;

    QSqlQueryModel* orderEntryModel;
    QSqlQueryModel* orderMarkerModel;
    QSqlQueryModel* partsModel;
    QSqlQueryModel* logsModel;

    QString selectedProjectName;
    int selectedPartId;
};

#endif // CUTLIST_HPP
