#include "ui_validate_log_length_dialog.h"
#include "validate_log_length_dialog.hpp"

ValidateLogLengthDialog::ValidateLogLengthDialog(double expectedLength,
                                                 QWidget* parent)
    : QDialog(parent), ui(new Ui::ValidateLogLengthDialog)
{
    ui->setupUi(this);

    // Set expected length label to show the value passed in.
    ui->expectedLengthValueLabel->setText(QString::number(expectedLength, 'f', 2));

    // Hook up the buttons to the dialog standard slots.
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->okButton, &QPushButton::clicked, this, &QDialog::accept);
}

ValidateLogLengthDialog::~ValidateLogLengthDialog() { delete ui; }

double ValidateLogLengthDialog::GetActualLengthUsed() const
{
    return ui->ValidateLogLengthSpin->value();
}
