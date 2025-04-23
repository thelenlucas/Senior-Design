#pragma once
#include <QDialog>
#include <QString>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include "domain/cutlist.hpp"

namespace woodworks::ui {

class CustomCutDialog : public QDialog {
    Q_OBJECT
public:
    explicit CustomCutDialog(const QString& currentProject, QWidget* parent = nullptr);

    woodworks::domain::CustomCut getCustomCut() const;

private slots:
    void chooseImage();

private:
    QComboBox* projectCombo;
    QLineEdit* partEdit;
    QLineEdit* codeEdit;
    QSpinBox* quantitySpin;
    QDoubleSpinBox* thicknessSpin;
    QDoubleSpinBox* widthSpin;
    QDoubleSpinBox* lengthSpin;
    QLineEdit* speciesEdit;
    QSpinBox* progressRoughSpin;
    QSpinBox* progressFinishedSpin;
    QTextEdit* notesEdit;
    QLabel* imagePreview;
    QPushButton* imageButton;
    QPushButton* okButton;
    QPushButton* cancelButton;
};
