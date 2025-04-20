#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class logInventoryWidget; }
QT_END_NAMESPACE

namespace woodworks::widgets {
    class LogInventory : public QWidget
    {
        Q_OBJECT

    public:
        explicit LogInventory(QWidget *parent = nullptr);
        ~LogInventory();

    private slots:
        void onDatabaseUpdate();

    private:
        void refreshModel();
        Ui::logInventoryWidget *ui;
    };
}