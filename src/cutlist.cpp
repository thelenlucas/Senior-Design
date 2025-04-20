#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "cutlist.hpp"
#include "ui_cutlist.h"

const constexpr double DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO = 0.6;

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

    RefreshModels();
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

// Note: ResizeSubWindowsProportionally is now obsolete and removed.

void CutlistPage::OnCutCookie()
{
    qDebug() << "[CutlistPage] Cookie cut dialog triggered.";
}

void CutlistPage::OnCutSlab()
{
    qDebug() << "[CutlistPage] Slab cut dialog triggered.";
}

void CutlistPage::OnCutLumber()
{
    qDebug() << "[CutlistPage] Lumber cut dialog triggered.";
}

void CutlistPage::OnDeleteProject()
{
    qDebug() << "[CutlistPage] Delete project dialog triggered.";
}

void CutlistPage::OnProjectSelectionChanged(int index)
{
    qDebug() << "[CutlistPage] Project selection changed. New index:" << index;
}

void CutlistPage::OnCompletePartCut()
{
    qDebug() << "[CutlistPage] Completing a part cut.";
}

void CutlistPage::OnUndoLastCut()
{
    qDebug() << "[CutlistPage] Undoing the last cut.";
}

void CutlistPage::OnAdjustCutLength()
{
    qDebug() << "[CutlistPage] Adjusting the length of a cut.";
}

void CutlistPage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    ResizeToDisplayPercentage(DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO, DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO);
}
