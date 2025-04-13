#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "cutlist.hpp"
#include "ui_cutlist.h"

const constexpr double DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO = 0.6;
const constexpr unsigned int DEFAULT_SUBWINDOW_COUNT = 3U;

CutlistPage::CutlistPage(QWidget* parent)
    : QWidget(parent), ui(new Ui::CutlistPage),
      orderEntryModel(new QSqlQueryModel(this)),
      orderMarkerModel(new QSqlQueryModel(this)),
      partsModel(new QSqlQueryModel(this)), logsModel(new QSqlQueryModel(this))
{
    ui->setupUi(this);

    ui->orderEntrySubwindow->setWindowIcon(QIcon());
    ui->orderMarkerSubwindow->setWindowIcon(QIcon());
    ui->commonCutMarkerSubwindow->setWindowIcon(QIcon());

    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ResizeToDisplayPercentage(DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO,
                              DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO);
    ResizeSubWindowsProportionally(DEFAULT_SUBWINDOW_COUNT);

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

    refreshModels();
}

CutlistPage::~CutlistPage() { delete ui; }

void CutlistPage::refreshModels()
{
    orderEntryModel->setQuery("SELECT * FROM cutlist",
                              QSqlDatabase::database());

    if (orderEntryModel->lastError().isValid())
        qDebug() << "Order entry model error:"
                 << orderEntryModel->lastError().text();

    if (orderMarkerModel->lastError().isValid())
        qDebug() << "Order marker model error:"
                 << orderMarkerModel->lastError().text();

    partsModel->setQuery("SELECT DISTINCT part, code, t, w, l, species FROM cutlist", QSqlDatabase::database());
    logsModel->setQuery("SELECT * FROM logs_view", QSqlDatabase::database());

    if (partsModel->lastError().isValid())
        qDebug() << "Parts model query error:"
                 << partsModel->lastError().text();

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
        QSize windowSize(screenSize.width() * width_ratio,
                         screenSize.height() * height_ratio);
        resize(windowSize);
    }
}

void CutlistPage::ResizeSubWindowsProportionally(unsigned int count)
{
    if (count == 0)
        return;

    QSize mdiSize = ui->mdiArea->viewport()->size();
    int fullWidth = mdiSize.width();
    int totalHeight = mdiSize.height();
    int subHeight = totalHeight / count;

    qDebug() << "[Resize] QMdiArea viewport size:" << mdiSize;
    qDebug() << "[Resize] Calculated full width:" << fullWidth;
    qDebug() << "[Resize] Calculated total height:" << totalHeight;
    qDebug() << "[Resize] Calculated height per subwindow:" << subHeight;

    ui->orderEntrySubwindow->resize(fullWidth, subHeight);
    ui->orderMarkerSubwindow->resize(fullWidth, subHeight);
    ui->commonCutMarkerSubwindow->resize(fullWidth, subHeight);

    ui->orderEntrySubwindow->move(0, 0);
    ui->orderMarkerSubwindow->move(0, subHeight);
    ui->commonCutMarkerSubwindow->move(0, 2 * subHeight);

    qDebug() << "[Resize] Subwindow positions set to:";
    qDebug() << "  orderEntrySubwindow @ (0, 0)";
    qDebug() << "  orderMarkerSubwindow @ (0," << subHeight << ")";
    qDebug() << "  commonCutMarkerSubwindow @ (0," << 2 * subHeight << ")";

    ui->orderEntrySubwindow->show();
    ui->orderMarkerSubwindow->show();
    ui->commonCutMarkerSubwindow->show();

    // Force internal layouts to recalculate. This results in a core dump. Looking for alternative solutions.
    //ui->orderEntrySubwindow->widget()->adjustSize();
    //ui->orderMarkerSubwindow->widget()->adjustSize();
    //ui->commonCutMarkerSubwindow->widget()->adjustSize();
}

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

    // Defer resizing until after widget is fully shown and laid out. Otherwise we will get subwindows and their widgets overlapping.
    ResizeToDisplayPercentage(DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO, DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO);
    ResizeSubWindowsProportionally(DEFAULT_SUBWINDOW_COUNT);
}
