#pragma once
#include "domain/types.hpp"
#include "infra/repository.hpp"

#include <QDialog>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>

namespace woodworks::domain
{
    template <typename T>
    inline void dryingPopUp(T &toDry)
    {
        using woodworks::infra::QtSqlRepository;
        using namespace woodworks::domain::types;

        std::vector<Drying> allowed = allowedTransitions(toDry.drying);

        QDialog dialog;
        dialog.setWindowTitle("Set Drying State");
        dialog.setModal(true);
        dialog.setMinimumSize(300, 200);
        dialog.setLayout(new QVBoxLayout());

        QComboBox *comboBox = new QComboBox();
        for (const auto &state : allowed)
        {
            comboBox->addItem(QString::fromStdString(toString(state)));
        }
        dialog.layout()->addWidget(comboBox);

        QPushButton *confirmButton = new QPushButton("Confirm");
        dialog.layout()->addWidget(confirmButton);
        QPushButton *cancelButton = new QPushButton("Cancel");
        dialog.layout()->addWidget(cancelButton);

        QObject::connect(confirmButton, &QPushButton::clicked, [&dialog, &toDry, &allowed, comboBox]()
                         {
        toDry.drying = allowed.at(comboBox->currentIndex());
        dialog.accept(); });
        QObject::connect(cancelButton, &QPushButton::clicked, [&dialog]()
                         { dialog.reject(); });

        if (dialog.exec() == QDialog::Accepted)
        {
            QtSqlRepository<T>::spawn().update(toDry);
        }
    }
}
