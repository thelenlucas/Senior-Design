#include "domain/live_edge_slab.hpp"
#include "domain/types.hpp"
#include "infra/repository.hpp"

#include <QDialog>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>

using namespace woodworks::infra;

// From a slab, allows modifying it's surfacing to a allowed slab surfacing, given its current, saves on confirm
namespace woodworks::domain {
    inline void slabSurfacingPopUp(LiveEdgeSlab toSurface) {
        // Allowed surfacing types
        std::vector<SlabSurfacing> allowedSurfacing = allowedTransitions(toSurface.surfacing);
        // Create a dialog to select the surfacing type
        QDialog dialog;
        dialog.setWindowTitle("Surface Slab");
        dialog.setModal(true);
        dialog.setMinimumWidth(300);
        dialog.setMinimumHeight(200);
        dialog.setLayout(new QVBoxLayout());
        // Dropdown for surfacing types
        QComboBox *comboBox = new QComboBox();
        for (const auto &surfacing : allowedSurfacing) {
            comboBox->addItem(QString::fromStdString(toString(surfacing)));
        }
        dialog.layout()->addWidget(comboBox);
        // Confirm button
        QPushButton *confirmButton = new QPushButton("Confirm");
        dialog.layout()->addWidget(confirmButton);
        // Cancel button
        QPushButton *cancelButton = new QPushButton("Cancel");
        dialog.layout()->addWidget(cancelButton);
        // Connect the confirm button to the dialog accept
        QObject::connect(confirmButton, &QPushButton::clicked, [&dialog, &toSurface, comboBox]() {
            // Get the selected surfacing type
            SlabSurfacing selectedSurfacing = static_cast<SlabSurfacing>(comboBox->currentIndex());
            // Update the surfacing type
            toSurface.surfacing = selectedSurfacing;
            // Accept the dialog
            dialog.accept();
        });
        // Connect the cancel button to the dialog reject
        QObject::connect(cancelButton, &QPushButton::clicked, [&dialog]() {
            // Reject the dialog
            dialog.reject();
        });

        // Show the dialog
        if (dialog.exec() == QDialog::Accepted) {
            // Update the surfacing type in the database
            QtSqlRepository<LiveEdgeSlab>::spawn().update(toSurface);
        } else {
            // User cancelled, do nothing
        }
    }
}