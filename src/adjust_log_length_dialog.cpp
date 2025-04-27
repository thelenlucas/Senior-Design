#include "adjust_log_length_dialog.hpp"
#include "ui_adjust_log_length_dialog.h"

AdjustLogLengthDialog::AdjustLogLengthDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AdjustLogLengthDialog)
{
    ui->setupUi(this);

    // Cancel and OK button hookups
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->okButton, &QPushButton::clicked, this, &QDialog::accept);

    // Initialize button state based on spin box value
    connect(ui->lengthSpinBox,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            [this](double val)
            { ui->okButton->setEnabled(val > 0.0); });

    ui->okButton->setEnabled(ui->lengthSpinBox->value() > 0.0);
}

AdjustLogLengthDialog::~AdjustLogLengthDialog() { delete ui; }

double AdjustLogLengthDialog::GetLengthToSubtract() const
{
    return ui->lengthSpinBox->value();
}
