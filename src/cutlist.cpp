#include <QDebug>
#include <QGuiApplication>
#include <QMessageBox>
#include <QScreen>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "adjust_log_length_dialog.hpp"
#include "cutlist.hpp"
#include "standard_cut_dialog.hpp"
#include "ui_cutlist.h"
#include "validate_log_length_dialog.hpp"

const constexpr double DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO = 0.6;
const constexpr int MAX_UNDO_HISTORY = 10;

CutlistPage::CutlistPage(QWidget* parent)
    : QWidget(parent), ui(new Ui::CutlistPage),
      orderEntryModel(new QSqlQueryModel(this)),
      orderMarkerModel(new QSqlQueryModel(this)),
      partsModel(new QSqlQueryModel(this)), logsModel(new QSqlQueryModel(this))
{
    ui->setupUi(this);

    ResizeToDisplayPercentage(DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO, DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO);

    ui->orderEntryTable->setModel(orderEntryModel);
    ui->orderMarkerTable->setModel(orderMarkerModel);
    ui->customPartsTable->setModel(partsModel);
    ui->matchingLogsTable->setModel(logsModel);

    connect(ui->cookieCutButton, &QPushButton::clicked, this, &CutlistPage::OnCutCookie);
    connect(ui->slabCutButton, &QPushButton::clicked, this, &CutlistPage::OnCutSlab);
    connect(ui->lumberCutButton, &QPushButton::clicked, this, &CutlistPage::OnCutLumber);

    connect(ui->deleteProjectButton, &QPushButton::clicked, this, &CutlistPage::OnDeleteProject);
    connect(ui->projectSelectorParts, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CutlistPage::OnProjectSelectionChanged);

    connect(ui->completeCutButton, &QPushButton::clicked, this, &CutlistPage::OnCompletePartCut);
    connect(ui->undoCutButton, &QPushButton::clicked, this, &CutlistPage::OnUndoLastCut);
    connect(ui->adjustCutLengthButton, &QPushButton::clicked, this, &CutlistPage::OnAdjustCutLength);
    connect(ui->customPartsTable->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &CutlistPage::OnPartSelectionChanged);

    RefreshModels();
    RefreshProjectSelector();
}
CutlistPage::~CutlistPage() { delete ui; }

void CutlistPage::RefreshModels()
{
    orderEntryModel->setQuery("SELECT * FROM cutlist", QSqlDatabase::database());

    if (orderEntryModel->lastError().isValid())
        qDebug() << "Order entry model error:" << orderEntryModel->lastError().text();

    if (orderMarkerModel->lastError().isValid())
        qDebug() << "Order marker model error:" << orderMarkerModel->lastError().text();

    partsModel->setQuery("SELECT DISTINCT part, code, t, w, l, species, notes FROM cutlist", QSqlDatabase::database());
    logsModel->setQuery("SELECT * FROM logs_view", QSqlDatabase::database());

    if (partsModel->lastError().isValid()) 
        qDebug() << "Parts model query error:" << partsModel->lastError().text();

    if (logsModel->lastError().isValid())
        qDebug() << "Logs model query error:" << logsModel->lastError().text();
}

void CutlistPage::RefreshProjectSelector()
{
    ui->projectSelectorParts->clear();

    QSqlQuery query(QSqlDatabase::database());
    if (query.exec("SELECT DISTINCT project FROM cutlist ORDER BY project ASC"))
    {
        while (query.next())
            ui->projectSelectorParts->addItem(query.value(0).toString());
    }
    else
    {
        qDebug() << "[CutlistPage] Failed to refresh project selector:" << query.lastError().text();
    }
}

void CutlistPage::ResizeToDisplayPercentage(double width_ratio,
                                            double height_ratio)
{
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QSize screenSize = screen->availableGeometry().size();
        QSize windowSize(screenSize.width() * width_ratio, screenSize.height() * height_ratio);
        resize(windowSize);
    }
}

void CutlistPage::OnCutCookie()
{
    qDebug() << "[CutlistPage] Cookie cut dialog triggered.";

    StandardCutDialog dialog(StandardCutDialog::CutType::Cookie, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        double expectedLength = dialog.GetExpectedLengthUsed();

        // Launch the validation dialog
        ValidateLogLengthDialog confirmDialog(expectedLength, this);
        if (confirmDialog.exec() == QDialog::Accepted)
        {
            double actualLengthUsed = confirmDialog.GetActualLengthUsed();
            qDebug() << "[CutlistPage] Actual length confirmed:"
                     << actualLengthUsed;

            // TODO: Update database and UI to reflect this new length usage.
        }
        else
        {
            qDebug()
                << "[CutlistPage] User canceled actual length confirmation.";
        }
    }
    else
    {
        qDebug() << "[CutlistPage] Cookie cut canceled.";
    }
}

void CutlistPage::OnCutSlab()
{
    qDebug() << "[CutlistPage] Slab cut dialog triggered.";

    StandardCutDialog dialog(StandardCutDialog::CutType::Slab, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        double expectedLength = dialog.GetExpectedLengthUsed();

        ValidateLogLengthDialog confirmDialog(expectedLength, this);
        if (confirmDialog.exec() == QDialog::Accepted)
        {
            double actualLengthUsed = confirmDialog.GetActualLengthUsed();
            qDebug() << "[CutlistPage] Actual slab length confirmed:"
                     << actualLengthUsed;

            // TODO: Apply updates to the database or model here.
        }
        else
        {
            qDebug() << "[CutlistPage] User canceled actual slab length "
                        "confirmation.";
        }
    }
    else
    {
        qDebug() << "[CutlistPage] Slab cut canceled.";
    }
}

void CutlistPage::OnCutLumber()
{
    qDebug() << "[CutlistPage] Lumber cut dialog triggered.";

    StandardCutDialog dialog(StandardCutDialog::CutType::Lumber, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        double expectedLength = dialog.GetExpectedLengthUsed();

        ValidateLogLengthDialog confirmDialog(expectedLength, this);
        if (confirmDialog.exec() == QDialog::Accepted)
        {
            double actualLengthUsed = confirmDialog.GetActualLengthUsed();
            qDebug() << "[CutlistPage] Actual lumber length confirmed:"
                     << actualLengthUsed;

            // TODO: Apply updates to the database or model here
        }
        else
        {
            qDebug() << "[CutlistPage] User canceled actual lumber length "
                        "confirmation.";
        }
    }
    else
    {
        qDebug() << "[CutlistPage] Lumber cut canceled.";
    }
}

void CutlistPage::OnDeleteProject()
{
    int index = ui->projectSelectorParts->currentIndex();
    if (index < 0)
    {
        qDebug() << "[CutlistPage] No project selected for deletion.";
        return;
    }

    QString projectName = ui->projectSelectorParts->currentText();
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Are you sure you want to delete project '%1'? This will remove all associated cuts.").arg(projectName),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        QSqlQuery query(QSqlDatabase::database());
        query.prepare("DELETE FROM cutlist WHERE project = :project");
        query.bindValue(":project", projectName);

        if (!query.exec())
        {
            qDebug() << "[CutlistPage] Failed to delete project:" << query.lastError().text();
            QMessageBox::critical(this, "Deletion Failed", "Failed to delete project from database.");
            return;
        }

        qDebug() << "[CutlistPage] Project deleted:" << projectName;

        RefreshModels();          // Refresh tables.
        RefreshProjectSelector(); // Refresh dropdown.
    }
    else
    {
        qDebug() << "[CutlistPage] Project deletion canceled by user.";
    }
}

void CutlistPage::OnProjectSelectionChanged(int index)
{
    qDebug() << "[CutlistPage] Project selection changed. New index:" << index;
}

void CutlistPage::OnCompletePartCut()
{
    QModelIndex index = ui->customPartsTable->currentIndex();
    if (!index.isValid())
    {
        qDebug() << "[CutlistPage] No part selected to cut.";
        return;
    }

    QString part = partsModel->data(partsModel->index(index.row(), 0)).toString();
    QString code = partsModel->data(partsModel->index(index.row(), 1)).toString();

    QSqlQuery query(QSqlDatabase::database());

    // Retrieve the current quantity.
    query.prepare(
        "SELECT qty FROM cutlist WHERE part = :part AND code = :code");
    query.bindValue(":part", part);
    query.bindValue(":code", code);

    if (!query.exec() || !query.next())
    {
        qDebug() << "[CutlistPage] Failed to fetch qty for part:" << query.lastError().text();
        return;
    }

    int currentQty = query.value(0).toInt();
    if (currentQty <= 0)
    {
        qDebug() << "[CutlistPage] Cannot complete cut — quantity is already zero.";
        return;
    }

    // Save a snapshot of the row.
    QMap<QString, QVariant> partSnapshot;
    for (int column = 0; column < partsModel->columnCount(); ++column)
    {
        QString key = partsModel->headerData(column, Qt::Horizontal).toString();
        QVariant value =
            partsModel->data(partsModel->index(index.row(), column));
        partSnapshot.insert(key, value);
    }

    undoStack.append(partSnapshot);
    if (undoStack.size() > MAX_UNDO_HISTORY)
        undoStack.removeFirst();

    // Decrement the quantity in the database.
    query.prepare("UPDATE cutlist SET qty = qty - 1 WHERE part = :part AND code = :code");
    query.bindValue(":part", part);
    query.bindValue(":code", code);

    if (!query.exec())
    {
        qDebug() << "[CutlistPage] Failed to update qty:" << query.lastError().text();
        return;
    }

    qDebug() << "[CutlistPage] Completed cut for part:" << part << " New quantity:" << currentQty - 1;

    RefreshModels();
}

void CutlistPage::OnUndoLastCut()
{
    if (undoStack.isEmpty())
    {
        qDebug() << "[CutlistPage] Undo stack is empty.";
        return;
    }

    QMap<QString, QVariant> lastCut = undoStack.takeLast();
    QString part = lastCut.value("part").toString();
    QString code = lastCut.value("code").toString();

    QSqlQuery query(QSqlDatabase::database());
    query.prepare("UPDATE cutlist SET qty = qty + 1 WHERE part = :part AND code = :code");
    query.bindValue(":part", part);
    query.bindValue(":code", code);

    if (!query.exec())
    {
        qDebug() << "[CutlistPage] Failed to undo cut in database:" << query.lastError().text();
        return;
    }

    qDebug() << "[CutlistPage] Undid last cut. Part:" << part;

    RefreshModels();
}

void CutlistPage::OnAdjustCutLength()
{
    QModelIndex selectedIndex = ui->matchingLogsTable->currentIndex();
    if (!selectedIndex.isValid())
    {
        qDebug() << "[CutlistPage] No log selected to adjust.";
        return;
    }

    AdjustLogLengthDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        double lengthToSubtract = dialog.GetLengthToSubtract();
        qDebug() << "[CutlistPage] Length to subtract from log:"
                 << lengthToSubtract;

        int logIdColumn = logsModel->record(0).indexOf("id");
        if (logIdColumn == -1)
        {
            qDebug() << "[CutlistPage] Could not find 'id' column in logs model.";
            return;
        }

        QVariant logId =
            logsModel->data(logsModel->index(selectedIndex.row(), logIdColumn));
        if (!logId.isValid())
        {
            qDebug() << "[CutlistPage] Selected log does not have a valid ID.";
            return;
        }

        // Apply update to the logs table in database.
        QSqlQuery query(QSqlDatabase::database());
        query.prepare("UPDATE logs SET length = length - :subtract WHERE id = :logId");
        query.bindValue(":subtract", lengthToSubtract);
        query.bindValue(":logId", logId);

        if (!query.exec())
        {
            qDebug() << "[CutlistPage] Failed to update log length:"
                     << query.lastError().text();
        }
        else
        {
            qDebug() << "[CutlistPage] Log length updated successfully.";
            logsModel->query().exec(); // Refresh current logs query
        }
    }
    else
    {
        qDebug() << "[CutlistPage] Adjust log length canceled.";
    }
}

void CutlistPage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    ResizeToDisplayPercentage(DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO, DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO);
}

void CutlistPage::OnPartSelectionChanged(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    UpdateMatchingLogsForSelectedPart(index);
}

void CutlistPage::UpdateMatchingLogsForSelectedPart(const QModelIndex &index)
{
    QString part = partsModel->data(partsModel->index(index.row(), 0)).toString();
    QString species = partsModel->data(partsModel->index(index.row(), 5)).toString();
    int thickness = partsModel->data(partsModel->index(index.row(), 2)).toInt(); // t
    int width = partsModel->data(partsModel->index(index.row(), 3)).toInt(); // w
    int length = partsModel->data(partsModel->index(index.row(), 4)).toInt(); // l

    QString queryStr = QString("SELECT * FROM logs_view "
                               "WHERE species = '%1' "
                               "AND diameter >= %2 "
                               "AND length >= %3").arg(species).arg(width).arg(length);

    logsModel->setQuery(queryStr, QSqlDatabase::database());

    if (logsModel->lastError().isValid())
    {
        qDebug() << "[CutlistPage] Failed to update matching logs:"
                 << logsModel->lastError().text();
    }
}
