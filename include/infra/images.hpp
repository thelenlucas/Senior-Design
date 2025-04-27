/**
 * @file images.hpp
 * @brief Provides templated functions for handling images in the application.
 * 
 * This file includes functions to load, save, and display images with additional
 * functionality for adding notes and replacing images. It uses Qt framework components.
 */

#include <QPixmap>
#include <QBuffer>
#include <QIODevice>
#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QObject>
#include <QSize>
#include <QInputDialog>
#include <QTextEdit>
#include "repository.hpp"
#include <QCoreApplication>

namespace woodworks::infra
{
    /**
     * @brief Loads a QPixmap from a domain item.
     * 
     * @tparam T The type of the domain item.
     * @param item The domain item containing the image buffer.
     * @return QPixmap The loaded QPixmap.
     */
    template <typename T>
    QPixmap loadImage(const T &item)
    {
        QPixmap pixmap;
        pixmap.loadFromData(item.imageBuffer);
        return pixmap;
    }

    /**
     * @brief Saves a QPixmap to a domain item.
     * 
     * @tparam T The type of the domain item.
     * @param item The domain item to save the image buffer to.
     * @param pixmap The QPixmap to save.
     */
    template <typename T>
    void saveImage(T &item, const QPixmap &pixmap)
    {
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        pixmap.save(&buffer, "PNG");
        item.imageBuffer = byteArray;
    }

    /**
     * @brief Displays an image popup with options to replace or save the image.
     * 
     * @tparam T The type of the domain item.
     * @param item The domain item containing the image and notes.
     * @param parent The parent widget for the popup dialog (optional).
     */
    template <typename T>
    void viewImagePopup(T &item, QWidget *parent = nullptr)
    {
        QPixmap pix = loadImage(item);

        QPixmap displayPix = pix;
        {
            int maxW = 800, maxH = 600, minW = 200, minH = 200;
            QSize orig = pix.size();
            if (orig.width() > maxW || orig.height() > maxH)
            {
                displayPix = pix.scaled(maxW, maxH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            else if (orig.width() < minW && orig.height() < minH)
            {
                displayPix = pix.scaled(minW, minH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        }

        QDialog dlg(parent);
        dlg.setWindowTitle(QCoreApplication::translate("ImageViewer", "Image Viewer"));
        QVBoxLayout *layout = new QVBoxLayout(&dlg);

        QLabel *imgLabel = new QLabel(&dlg);
        if (pix.isNull()) {
            imgLabel->setText(QCoreApplication::translate("ImageViewer", "No Image"));
        } else {
            imgLabel->setPixmap(displayPix);
        }
        imgLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(imgLabel);

        QLabel *notesLabel = new QLabel(QCoreApplication::translate("ImageViewer", "Notes:"), &dlg);
        QTextEdit *notesEdit = new QTextEdit(QString::fromStdString(item.notes), &dlg);
        notesEdit->setMinimumHeight(100);
        layout->addWidget(notesLabel);
        layout->addWidget(notesEdit);

        QHBoxLayout *btnLayout = new QHBoxLayout();
        QPushButton *replaceBtn = new QPushButton(QCoreApplication::translate("ImageViewer", "Add/Replace Image"), &dlg);
        QPushButton *closeBtn = new QPushButton(QCoreApplication::translate("ImageViewer", "Close"), &dlg);
        btnLayout->addWidget(replaceBtn);
        btnLayout->addWidget(closeBtn);
        layout->addLayout(btnLayout);

        QObject::connect(replaceBtn, &QPushButton::clicked, [&dlg, &imgLabel, &item]()
                         {
            QString fn = QFileDialog::getOpenFileName(&dlg,
                                                      QObject::tr("Open Image"),
                                                      QString(),
                                                      QObject::tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
            if (!fn.isEmpty()) {
                QPixmap newPix(fn);
                if (!newPix.isNull()) {
                    saveImage(item, newPix);
                    QtSqlRepository<T>::spawn().update(item);
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
                                         QCoreApplication::translate("ImageViewer", "Replace Failed"),
                                         QCoreApplication::translate("ImageViewer", "Could not load or save the selected image."));
                }
            } });

        QObject::connect(closeBtn, &QPushButton::clicked, [&dlg]() { dlg.accept(); });

        QObject::connect(&dlg, &QDialog::finished, [&item, notesEdit](int) {
            item.notes = notesEdit->toPlainText().toStdString();
            QtSqlRepository<T>::spawn().update(item);
        });

        dlg.exec();
    }
}