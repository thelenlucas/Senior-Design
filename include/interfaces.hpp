/**
 * @file interfaces.hpp
 * @brief Defines common interfaces used throughout the application, such as persistence.
 */
#ifndef INTERFACES_HPP
#define INTERFACES_HPP

#include <optional>
#include "types.hpp" // Assuming types.hpp defines necessary types, otherwise remove/change
#include <vector>
#include <QPixmap>
#include <QSqlQuery>
#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QCoreApplication>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

/**
 * @brief Interface for classes that can be persisted to and retrieved from a database.
 * @details This uses the Curiously Recurring Template Pattern (CRTP). Derived classes
 *          must implement the static methods get_by_id and get_all.
 * @tparam Derived The derived class that implements this interface (e.g., `class MyData : public Persistent<MyData>`).
 */
template <typename Derived>
class Persistent {
public:
    /**
     * @brief Default virtual destructor.
     */
    virtual ~Persistent() = default;

    /**
     * @brief Get the unique identifier of the object.
     * @return The integer ID of the object.
     */
    virtual int get_id() const = 0;

    /**
     * @brief Insert the current state of the object into the database as a new entry.
     * @return True if the insertion was successful, false otherwise.
     */
    virtual bool insert() = 0;

    /**
     * @brief Update the existing database entry corresponding to this object's ID
     *        with the current state of the object.
     * @return True if the update was successful, false otherwise.
     */
    virtual bool update() = 0;

    /**
     * @brief Load pixmap from 'media' blob column in the database.
     * @return QPixmap loaded from data or blank pixmap if not found.
     */
    virtual QPixmap loadPixmap() const = 0;

    /**
     * @brief Save pixmap to 'media' blob column in the database.
     * @param pixmap The QPixmap to save.
     * @return True if the save was successful, false otherwise.
     */
    virtual bool savePixmap(const QPixmap& pixmap) const = 0;

    /**
     * @brief Retrieve an object from the database by its unique identifier.
     * @param id The ID of the object to retrieve.
     * @return An std::optional containing the object if found, or std::nullopt otherwise.
     */
    static std::optional<Derived> get_by_id(int id) {
        // Static methods in the base class template delegate to the derived class's static methods.
        return Derived::get_by_id(id);
    }

    /**
     * @brief Retrieve all objects of this type from the database.
     * @return A std::vector containing all objects of the derived type.
     */
    static std::vector<Derived> get_all() {
        // Static methods in the base class template delegate to the derived class's static methods.
        return Derived::get_all();
    }

    /**
     * @brief Pop up a simple dialog showing the loaded pixmap, with a “Save As…” button.
     * @param parent Optional parent widget for the dialog.
     */
    void viewPixmap(QWidget* parent = nullptr) const
    {
        // Load the pixmap (calls the derived override)
        QPixmap pix = loadPixmap();
        if (pix.isNull()) {
            QMessageBox::information(parent,
                                    QCoreApplication::translate("Persistent", "No Image"),
                                    QCoreApplication::translate("Persistent", "There is no image to display."));
            return;
        }

        // Scale pixmap to reasonable display size
        QPixmap displayPix = pix;
        {
            int maxW = 800, maxH = 600, minW = 200, minH = 200;
            QSize orig = pix.size();
            if (orig.width() > maxW || orig.height() > maxH) {
                displayPix = pix.scaled(maxW, maxH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            } else if (orig.width() < minW && orig.height() < minH) {
                displayPix = pix.scaled(minW, minH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        }

        // Build dialog
        QDialog dlg(parent);
        dlg.setWindowTitle(QCoreApplication::translate("Persistent", "Image Viewer"));
        QVBoxLayout* layout = new QVBoxLayout(&dlg);

        // Image label
        QLabel* imgLabel = new QLabel(&dlg);
        imgLabel->setPixmap(displayPix);
        imgLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(imgLabel);

        // Add Replace and Save buttons in a horizontal layout
        QHBoxLayout* btnLayout = new QHBoxLayout();
        QPushButton* replaceBtn = new QPushButton(QCoreApplication::translate("Persistent", "Replace Image"), &dlg);
        QPushButton* saveBtn = new QPushButton(QCoreApplication::translate("Persistent", "Save As…"), &dlg);
        btnLayout->addWidget(replaceBtn);
        btnLayout->addWidget(saveBtn);
        layout->addLayout(btnLayout);

        // Wire up Replace Image: open file, save, and refresh view
        QObject::connect(replaceBtn, &QPushButton::clicked, [this, &dlg, imgLabel]() {
            QString fn = QFileDialog::getOpenFileName(&dlg,
                                                      QObject::tr("Open Image"),
                                                      QString(),
                                                      QObject::tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
            if (!fn.isEmpty()) {
                QPixmap newPix(fn);
                if (!newPix.isNull() && savePixmap(newPix)) {
                    // scale to display limits
                    QPixmap scaled = newPix;
                    int maxW = 800, maxH = 600, minW = 200, minH = 200;
                    QSize orig = newPix.size();
                    if (orig.width() > maxW || orig.height() > maxH)
                        scaled = newPix.scaled(maxW, maxH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    else if (orig.width() < minW && orig.height() < minH)
                        scaled = newPix.scaled(minW, minH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    imgLabel->setPixmap(scaled);
                    dlg.adjustSize();
                } else {
                    QMessageBox::warning(&dlg,
                                         QCoreApplication::translate("Persistent", "Replace Failed"),
                                         QCoreApplication::translate("Persistent", "Could not load or save the selected image."));
                }
            }
        });

        // Wire up “Save As…” button to file‑dialog + QPixmap::save()
        QObject::connect(saveBtn, &QPushButton::clicked, [&dlg, displayPix]() {
            QString fn = QFileDialog::getSaveFileName(&dlg,
                                                      QObject::tr("Save Image"),
                                                      QString(),
                                                      QObject::tr("PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;Bitmap Files (*.bmp)"));
            if (!fn.isEmpty())
                displayPix.save(fn);
        });

        dlg.exec();
    }

};
#endif // INTERFACES_HPP
