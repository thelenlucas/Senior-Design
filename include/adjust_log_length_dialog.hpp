#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AdjustLogLengthDialog;
}
QT_END_NAMESPACE

class AdjustLogLengthDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit AdjustLogLengthDialog(QWidget* parent = nullptr);
    ~AdjustLogLengthDialog();

    void SetLogInfoText(QString const &text);
    double GetLengthToSubtract() const;

  private:
    Ui::AdjustLogLengthDialog* ui;
};
