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
#include "repository.hpp"
#include <QCoreApplication>

namespace woodworks::infra
{
    // Templated function to load a qpixmap from a domain item
    template <typename T>
    QPixmap loadImage(const T &item)
    {
        QPixmap pixmap;
        pixmap.loadFromData(item.imageBuffer);
        return pixmap;
    }

    // Templated function to save a qpixmap to a domain item
    template <typename T>
    void saveImage(T &item, const QPixmap &pixmap)
    {
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        pixmap.save(&buffer, "PNG");
        item.imageBuffer = byteArray;
    }

    // Templated function to show image popup with replace/save functionality
    template <typename T>
    void viewImagePopup(T &item, QWidget *parent = nullptr)
    {
        QPixmap pix = loadImage(item);
        if (pix.isNull())
        {
            QDialog dlg(parent);
            dlg.setWindowTitle(QCoreApplication::translate("ImageViewer", "No Image"));
            QVBoxLayout *layout2 = new QVBoxLayout(&dlg);
            QLabel *lbl = new QLabel(QCoreApplication::translate("ImageViewer", "There is no image to display."), &dlg);
            lbl->setAlignment(Qt::AlignCenter);
            layout2->addWidget(lbl);
            QHBoxLayout *btnLayout2 = new QHBoxLayout();
            QPushButton *addBtn = new QPushButton(QCoreApplication::translate("ImageViewer", "Add Image"), &dlg);
            QPushButton *cancelBtn = new QPushButton(QCoreApplication::translate("ImageViewer", "Cancel"), &dlg);
            btnLayout2->addWidget(addBtn);
            btnLayout2->addWidget(cancelBtn);
            layout2->addLayout(btnLayout2);
            QObject::connect(addBtn, &QPushButton::clicked, [&dlg, &item]() {
                QString fn = QFileDialog::getOpenFileName(&dlg,
                                                          QObject::tr("Open Image"),
                                                          QString(),
                                                          QObject::tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
                if (!fn.isEmpty()) {
                    QPixmap newPix(fn);
                    if (!newPix.isNull()) {
                        saveImage(item, newPix);
                        QtSqlRepository<T>::spawn().update(item);
                        dlg.accept();
                    } else {
                        QMessageBox::warning(&dlg,
                                             QCoreApplication::translate("ImageViewer", "Add Failed"),
                                             QCoreApplication::translate("ImageViewer", "Could not load the selected image."));
                    }
                }
            });
            QObject::connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);
            if (dlg.exec() != QDialog::Accepted)
                return;
            pix = loadImage(item);
        }

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
        imgLabel->setPixmap(displayPix);
        imgLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(imgLabel);

        QHBoxLayout *btnLayout = new QHBoxLayout();
        QPushButton *replaceBtn = new QPushButton(QCoreApplication::translate("ImageViewer", "Replace Image"), &dlg);
        QPushButton *saveBtn = new QPushButton(QCoreApplication::translate("ImageViewer", "Save As…"), &dlg);
        btnLayout->addWidget(replaceBtn);
        btnLayout->addWidget(saveBtn);
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

        QObject::connect(saveBtn, &QPushButton::clicked, [&dlg, displayPix]()
                         {
            QString fn = QFileDialog::getSaveFileName(&dlg,
                                                      QObject::tr("Save Image"),
                                                      QString(),
                                                      QObject::tr("PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;Bitmap Files (*.bmp)"));
            if (!fn.isEmpty())
                displayPix.save(fn); });

        dlg.exec();
    }
}