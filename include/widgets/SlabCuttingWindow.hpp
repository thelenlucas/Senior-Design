#include "domain/log.hpp"
#include "domain/slab_cutter.hpp"
#include "domain/live_edge_slab.hpp"

using namespace woodworks::domain;

#include <QWidget>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class slabCuttingWindow;
}
QT_END_NAMESPACE

namespace woodworks::widgets
{
    class SlabCuttingWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit SlabCuttingWindow(Log log, QWidget *parent = nullptr);
        ~SlabCuttingWindow();

    private slots:
        void onSquareOffButtonClicked();
        void onAddCutButtonClicked();
        void onSlabThicknessChanged();
        void onFinishCutButtonClicked();

    private:
        Ui::slabCuttingWindow *ui;
        Log log;                  // The log being cut
        slabs::SlabCutter cutter; // The cutter object for the log

        // Ui update
        void updateUi();
    };
}