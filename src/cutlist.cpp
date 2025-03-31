#include "cutlist.hpp"
#include "ui_cutlist.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QScreen>
#include <QGuiApplication>

// TODO: Make these setable and memoized through config files. (YAML maybe? Or we could literally have it in the SQLiteDatabase as a table.)
const constexpr double DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO = 0.6;
const constexpr unsigned int DEFAULT_SUBWINDOW_COUNT = 3U;

CutlistPage::CutlistPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CutlistPage)
    , orderEntryModel(new QSqlQueryModel(this))
    , orderMarkerModel(new QSqlQueryModel(this))
{
    ui->setupUi(this);

    // Disable the QT icon that is standard on QMdiSubWindows as it is overlapping horrible and looks very bad.
    ui->orderEntrySubwindow->setWindowIcon(QIcon());
    ui->orderMarkerSubwindow->setWindowIcon(QIcon());
    ui->commonCutMarkerSubwindow->setWindowIcon(QIcon());

    // Set scroll behavior for MDI area.
    ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Resize window and child subwindows.
    ResizeToDisplayPercentage(DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO, DEFAULT_WINDOW_RESIZE_TO_DISPLAY_SIZE_RATIO);
    ResizeSubWindowsProportionally(DEFAULT_SUBWINDOW_COUNT);

    // Connect and Bind the models (logic) to the views (displays/gui) in our MVC.
    ui->orderEntryTable->setModel(orderEntryModel);
    ui->orderMarkerTable->setModel(orderMarkerModel);
    refreshModels();

}

CutlistPage::~CutlistPage()
{
    delete ui;
}

void CutlistPage::refreshModels()
{
    // TODO: Create a more complete set of live views based on all available inventory including wood cuts.
    //orderEntryModel->setQuery("SELECT * FROM current_order_view", QSqlDatabase::database());
    //orderMarkerModel->setQuery("SELECT * FROM pending_orders_view", QSqlDatabase::database());

    orderEntryModel->setQuery("SELECT * FROM cutlist", QSqlDatabase::database());


    if (orderEntryModel->lastError().isValid())
        qDebug() << "Order entry model error:" << orderEntryModel->lastError().text();

    if (orderMarkerModel->lastError().isValid())
        qDebug() << "Order marker model error:" << orderMarkerModel->lastError().text();
}

void CutlistPage::ResizeToDisplayPercentage(double width_ratio, double height_ratio)
{
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QSize screenSize = screen->availableGeometry().size();
        QSize windowSize(screenSize.width() * width_ratio, screenSize.height() * height_ratio);
        resize(windowSize);
    }
}

void CutlistPage::ResizeSubWindowsProportionally(unsigned int count)
{
    if (count <= 0)
        return;

    QSize mdiSize = ui->mdiArea->viewport()->size();
    int fullWidth = mdiSize.width();
    int totalHeight = mdiSize.height();
    int subHeight = totalHeight / count;

    ui->orderEntrySubwindow->resize(fullWidth, subHeight);
    ui->orderMarkerSubwindow->resize(fullWidth, subHeight);
    ui->commonCutMarkerSubwindow->resize(fullWidth, subHeight);

    ui->orderEntrySubwindow->move(0, 0);
    ui->orderMarkerSubwindow->move(0, subHeight);
    ui->commonCutMarkerSubwindow->move(0, 2 * subHeight);
}

