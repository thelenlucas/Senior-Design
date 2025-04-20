#include "standard_cut_dialog.hpp"

StandardCutDialog::StandardCutDialog(CutType type, QWidget* parent)
    : QDialog(parent), ui(new Ui::StandardCutDialog), cut_type(type)
{
    ui->setupUi(this);
    SetupForm(type);

    // Dialog controls.
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->okButton, &QPushButton::clicked, this, &QDialog::accept);

    // Live input validation.
    connect(ui->cookieThicknessSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &StandardCutDialog::ValidateInputs);
    connect(ui->slabThicknessSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &StandardCutDialog::ValidateInputs);

    connect(ui->lumberThicknessSpin,
            QOverload<int>::of(&QSpinBox::valueChanged), this,
            &StandardCutDialog::ValidateInputs);

    connect(ui->cookieCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &StandardCutDialog::ValidateInputs);
    connect(ui->slabCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &StandardCutDialog::ValidateInputs);
    connect(ui->lumberPieceCountSpin,
            QOverload<int>::of(&QSpinBox::valueChanged), this,
            &StandardCutDialog::ValidateInputs);

    ValidateInputs(); // Initialize the state.
}

StandardCutDialog::~StandardCutDialog() { delete ui; }

void StandardCutDialog::SetupForm(CutType type)
{
    switch (type)
    {
    case CutType::Cookie:
        setWindowTitle("Cut Cookies");
        ui->stackedWidget->setCurrentWidget(ui->cookiePage);
        break;

    case CutType::Slab:
        setWindowTitle("Cut Slabs");
        ui->stackedWidget->setCurrentWidget(ui->slabPage);
        break;

    case CutType::Lumber:
        setWindowTitle("Cut Lumber");
        ui->stackedWidget->setCurrentWidget(ui->lumberPage);
        break;
    }
}

// Existing calculations
double StandardCutDialog::GetExpectedLengthUsed() const
{
    switch (cut_type)
    {
    case CutType::Cookie:
        return ui->cookieCountSpin->value() * ui->cookieThicknessSpin->value();
    case CutType::Slab:
        return ui->slabLengthSpin->value();
    case CutType::Lumber:
        return ui->lumberLengthSpin->value();
    }
    return 0.0;
}

int StandardCutDialog::GetExpectedPieceCount() const
{
    switch (cut_type)
    {
    case CutType::Cookie:
        return ui->cookieCountSpin->value();
    case CutType::Slab:
        return ui->slabCountSpin->value();
    case CutType::Lumber:
        return ui->lumberPieceCountSpin->value();
    }
    return 0;
}

// New accessors
int StandardCutDialog::GetCookieCount() const
{
    return ui->cookieCountSpin->value();
}

double StandardCutDialog::GetCookieThickness() const
{
    return ui->cookieThicknessSpin->value();
}

int StandardCutDialog::GetSlabLength() const
{
    return ui->slabLengthSpin->value();
}

double StandardCutDialog::GetSlabThickness() const
{
    return ui->slabThicknessSpin->value();
}

int StandardCutDialog::GetLumberQuarters() const
{
    return ui->lumberThicknessSpin->value();
}

int StandardCutDialog::GetLumberWidth() const
{
    return ui->lumberWidthSpin->value();
}

int StandardCutDialog::GetLumberLength() const
{
    return ui->lumberLengthSpin->value();
}

QString StandardCutDialog::GetSurfacing() const
{
    return ui->surfacingDropdown->currentText();
}

// Certain inputs wouldn't make sense like having zero thickness or count so we want to block cut creation in those cases.
void StandardCutDialog::ValidateInputs()
{
    bool valid = true;

    switch (cut_type)
    {
    case CutType::Cookie:
        valid = ui->cookieCountSpin->value() > 0 && ui->cookieThicknessSpin->value() > 0.0;
        break;
    case CutType::Slab:
        valid = ui->slabCountSpin->value() > 0 && ui->slabThicknessSpin->value() > 0.0;
        break;
    case CutType::Lumber:
        valid = ui->lumberPieceCountSpin->value() > 0 && ui->lumberThicknessSpin->value() > 0;
        break;
    }

    ui->okButton->setEnabled(valid);
}
