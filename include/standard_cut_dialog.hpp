#pragma once

#include <QDialog>
#include <QString>

#include "ui_standard_cut_dialog.h"

class QSpinBox;
class QDoubleSpinBox;

class StandardCutDialog : public QDialog
{
    Q_OBJECT

  public:
    enum class CutType
    {
        Cookie,
        Slab,
        Lumber
    };

    explicit StandardCutDialog(CutType type, QWidget* parent = nullptr);
    ~StandardCutDialog();

    double GetExpectedLengthUsed() const;
    int GetExpectedPieceCount() const;

    int GetCookieCount() const;
    double GetCookieThickness() const;

    int GetSlabLength() const;
    double GetSlabThickness() const;

    int GetLumberQuarters() const;
    int GetLumberWidth() const;
    int GetLumberLength() const;
    QString GetSurfacing() const;

  private:
    void SetupForm(CutType type);
    void ValidateInputs();

    Ui::StandardCutDialog* ui;
    CutType cut_type;
};
