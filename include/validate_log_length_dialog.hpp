#pragma once

#include <QDialog>

namespace Ui
{
class ValidateLogLengthDialog;
}

class ValidateLogLengthDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit ValidateLogLengthDialog(double expectedLength, QWidget* parent = nullptr);
    ~ValidateLogLengthDialog();

    double GetActualLengthUsed() const;

  private:
    Ui::ValidateLogLengthDialog* ui;
};
