#include <QDebug>
#include <QGuiApplication>
#include <QMessageBox>
#include <QScreen>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QHBoxLayout>

#include "adjust_log_length_dialog.hpp"
#include "cutlist.hpp"
#include "standard_cut_dialog.hpp"
#include "ui_cutlist.h"
#include "domain/cutlist.hpp"
#include "infra/repository.hpp"

using namespace woodworks::domain;
using namespace woodworks::domain::imperial;
using namespace woodworks::domain::types;
using namespace woodworks::infra;

CutlistPage::CutlistPage(QWidget *parent) : QWidget(parent), ui(new Ui::CutlistPage)
{
    ui->setupUi(this);

    // Link slots
    connect(ui->addPartButton, &QPushButton::clicked, this, &CutlistPage::newPart);
    connect(ui->deleteProjectButton, &QPushButton::clicked, this, &CutlistPage::deleteProject);
    connect(ui->makeCutButton, &QPushButton::clicked, this, &CutlistPage::cutLog);
    connect(ui->markCompleteRoughButton, &QPushButton::clicked, this, &CutlistPage::partCompleteRough);
    connect(ui->markCompleteFinishedButton, &QPushButton::clicked, this, &CutlistPage::partCompleteFinished);
    connect(ui->projectSelectorCombo, &QComboBox::currentTextChanged, this, &CutlistPage::refreshModels);

    updateProjects();
    refreshModels();
}

void CutlistPage::newPart()
{
    QString currentProject = ui->projectSelectorCombo->currentText();
    QDialog dialog(this);
    QFormLayout form(&dialog);

    QComboBox *projectCombo = new QComboBox;
    projectCombo->setEditable(true);
    for (const auto &proj : CustomCut::allProjects())
        projectCombo->addItem(QString::fromStdString(proj));
    projectCombo->setCurrentText(currentProject);
    form.addRow("Project:", projectCombo);

    QLineEdit *partEdit = new QLineEdit;
    form.addRow("Part:", partEdit);
    QLineEdit *codeEdit = new QLineEdit;
    form.addRow("Code:", codeEdit);
    QSpinBox *qtySpin = new QSpinBox;
    qtySpin->setMinimum(0);
    form.addRow("Quantity:", qtySpin);

    // Thickness input
    QWidget *tWidget = new QWidget;
    auto *tLayout = new QHBoxLayout(tWidget);
    QSpinBox *tInSpin = new QSpinBox;
    tInSpin->setMinimum(0);
    tInSpin->setSuffix(" in");
    QSpinBox *tFracSpin = new QSpinBox;
    tFracSpin->setRange(0, 15);
    tFracSpin->setSuffix(" 16ths");
    tLayout->addWidget(tInSpin);
    tLayout->addWidget(tFracSpin);
    form.addRow("Thickness:", tWidget);

    // Width input
    QWidget *wWidget = new QWidget;
    auto *wLayout = new QHBoxLayout(wWidget);
    QSpinBox *wInSpin = new QSpinBox;
    wInSpin->setMinimum(0);
    wInSpin->setSuffix(" in");
    QSpinBox *wFracSpin = new QSpinBox;
    wFracSpin->setRange(0, 15);
    wFracSpin->setSuffix(" 16ths");
    wLayout->addWidget(wInSpin);
    wLayout->addWidget(wFracSpin);
    form.addRow("Width:", wWidget);

    // Length input
    QWidget *lWidget = new QWidget;
    auto *lLayout = new QHBoxLayout(lWidget);
    QSpinBox *lInSpin = new QSpinBox;
    lInSpin->setMinimum(0);
    lInSpin->setSuffix(" in");
    QSpinBox *lFracSpin = new QSpinBox;
    lFracSpin->setRange(0, 15);
    lFracSpin->setSuffix(" 16ths");
    lLayout->addWidget(lInSpin);
    lLayout->addWidget(lFracSpin);
    form.addRow("Length:", lWidget);

    QLineEdit *speciesEdit = new QLineEdit;
    form.addRow("Species:", speciesEdit);
    QLineEdit *notesEdit = new QLineEdit;
    form.addRow("Notes:", notesEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
        return;

    CustomCut cut = CustomCut::uninitialized();
    cut.project = projectCombo->currentText().toStdString();
    cut.part = partEdit->text().toStdString();
    cut.code = codeEdit->text().toStdString();
    cut.quantity = qtySpin->value();
    cut.t = Length::fromInches(tInSpin->value() + tFracSpin->value() / 16.0);
    cut.w = Length::fromInches(wInSpin->value() + wFracSpin->value() / 16.0);
    cut.l = Length::fromInches(lInSpin->value() + lFracSpin->value() / 16.0);
    cut.species = speciesEdit->text().toStdString();
    cut.notes = notesEdit->text().toStdString();

    QtSqlRepository<CustomCut>::spawn().add(cut);

    updateProjects();
    refreshModels();
}

void CutlistPage::deleteProject()
{
    QString currentProject = ui->projectSelectorCombo->currentText();
    if (currentProject.isEmpty())
        return;
    auto reply = QMessageBox::question(this, "Delete Project",
                                       QString("Are you sure you want to delete project: '%1'?").arg(currentProject),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes)
        return;
    auto repo = QtSqlRepository<CustomCut>::spawn();
    auto cuts = repo.list();
    for (const auto &cut : cuts)
    {
        if (QString::fromStdString(cut.project) == currentProject)
        {
            repo.remove(cut.id.id);
        }
    }
    updateProjects();
    refreshModels();
}

void CutlistPage::cutLog()
{
    auto log = QtSqlRepository<Log>::spawn().get(ui->spinBox->value());
    if (log)
    {
        // Popup to confirm the cut length in inches, with qtdialog
        QDialog dialog(this);
        QFormLayout form(&dialog);
        form.setSpacing(10);
        form.setContentsMargins(10, 10, 10, 10);
        auto *lengthEdit = new QDoubleSpinBox(&dialog);
        lengthEdit->setRange(0, log.value().length.toInches());
        lengthEdit->setSuffix(" in");
        form.addRow("Length:", lengthEdit);
        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
        if (dialog.exec() != QDialog::Accepted)
            return;
        // Get the length used
        double lengthUsed = lengthEdit->text().toDouble();
        // Update the log
        log.value().cut(Length::fromInches(lengthUsed));
        QtSqlRepository<Log>::spawn().update(log.value());

        refreshModels();
    }
}

void CutlistPage::partCompleteRough()
{
    // Save the current row so we can come back to it on refresh
    auto currentRow = ui->orderMarkerTable->currentIndex().row();

    // Complete one part rough for a custom cut (index 0 of current row)
    auto id = ui->orderMarkerTable->currentIndex().sibling(ui->orderMarkerTable->currentIndex().row(), 0).data().toInt();
    auto cut = QtSqlRepository<CustomCut>::spawn().get(id);
    if (cut)
    {
        cut.value().incrementRough();
        QtSqlRepository<CustomCut>::spawn().update(cut.value());
        refreshModels();

        // Go back to the row we were on
        auto index = ui->orderMarkerTable->model()->index(currentRow, 0);
        ui->orderMarkerTable->setCurrentIndex(index);
        ui->orderMarkerTable->scrollTo(index);
    }
}

void CutlistPage::partCompleteFinished()
{
    auto currentRow = ui->orderMarkerTable->currentIndex().row();
    // Complete one part finished
    auto id = ui->orderMarkerTable->currentIndex().sibling(ui->orderMarkerTable->currentIndex().row(), 0).data().toInt();
    auto cut = QtSqlRepository<CustomCut>::spawn().get(id);
    if (cut)
    {
        cut.value().incrementFinished();
        QtSqlRepository<CustomCut>::spawn().update(cut.value());
        refreshModels();

        auto index = ui->orderMarkerTable->model()->index(currentRow, 0);
        ui->orderMarkerTable->setCurrentIndex(index);
        ui->orderMarkerTable->scrollTo(index);
    }
}

void CutlistPage::refreshModels()
{
    // Get the current project
    QString currentProject = ui->projectSelectorCombo->currentText();

    // Query string
    auto query = CustomCut::cutlistViewSQLQuery(currentProject);
    // Set the model for the custom parts table
    auto model = new QSqlQueryModel(this);
    model->setQuery(query);
    ui->orderMarkerTable->setModel(model);

    ui->orderMarkerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->orderMarkerTable->hideColumn(0); // Hide ID column, it's entirely for internal use
}

void CutlistPage::updateProjects()
{
    // If we had a project selected, save that
    QString currentProject = ui->projectSelectorCombo->currentText();
    // Clear the project selector
    ui->projectSelectorCombo->clear();

    // Get all projects from the database
    auto projects = CustomCut::allProjects();
    // Add the projects to the project selector
    for (const auto &project : projects)
    {
        ui->projectSelectorCombo->addItem(QString::fromStdString(project));
    }

    // If we can find our old project, select it
    int index = ui->projectSelectorCombo->findText(currentProject);
    if (index != -1)
    {
        ui->projectSelectorCombo->setCurrentIndex(index);
    }
    else
    {
        // Otherwise, select the first project
        if (ui->projectSelectorCombo->count() > 0)
        {
            ui->projectSelectorCombo->setCurrentIndex(0);
        }
    }
    // Update the cutlist table
    refreshModels();
}

CutlistPage::~CutlistPage()
{
    delete ui;
}