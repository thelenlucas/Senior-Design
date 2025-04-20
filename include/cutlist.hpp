#ifndef CUTLIST_HPP
#define CUTLIST_HPP

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
    void OnPartSelectionChanged(const QModelIndex &index);

  private:
    void UpdateMatchingLogsForSelectedPart(const QModelIndex &index);
    void RefreshModels();
    void RefreshProjectSelector();
    void ResizeSubWindowsProportionally(unsigned int count);
    void ResizeToDisplayPercentage(double widthRatio, double heightRatio);

    void InitializeStandardCutButtons();
    void InitializeProjectSelector();
    void RefreshProjectList();
    void RefreshPartsTable();
    void RefreshMatchingLogsTable();
    void PopulateMatchingLogsForSelectedPart();

    Ui::CutlistPage* ui;

    int selectedPartId;

    QSqlQueryModel* orderEntryModel;
    QSqlQueryModel* orderMarkerModel;
    QSqlQueryModel* partsModel;
    QSqlQueryModel* logsModel;

    QString selectedProjectName;
    QModelIndex lastCutPartIndex;
    QVariantMap lastCutPartData;

    QStack<QMap<QString, QVariant>> undoHistory;
    QStack<int> undoRowHistory;

    QList<QMap<QString, QVariant>> undoStack;
};

#endif // CUTLIST_HPP
