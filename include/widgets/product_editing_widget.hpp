#pragma once

#include <QWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "sales/product.hpp"

namespace woodworks::sales
{

    class ProductEditorWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit ProductEditorWidget(Product &product, QWidget *parent = nullptr)
            : QWidget(parent), m_product(product)
        {
            // --- Image on the left ---
            QLabel *imgLabel = new QLabel;
            QPixmap pix;
            pix.loadFromData(
                QByteArray::fromBase64(QByteArray::fromStdString(product.imageBase64.toStdString())),
                "PNG");
            imgLabel->setPixmap(pix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));

            QVBoxLayout *detailLayout = new QVBoxLayout;
            detailLayout->addWidget(new QLabel(
                QString("%1: %2")
                    .arg(QString::fromStdString(toString(product.type)))
                    .arg(QString::fromStdString(product.species))));
            for (auto &line : product.detailsLines)
            {
                detailLayout->addWidget(new QLabel(QString::fromStdString(line)));
            }

            // --- Price editor ---
            QHBoxLayout *priceLayout = new QHBoxLayout;
            auto *spin = new QDoubleSpinBox;
            spin->setRange(0.0, 10000.0);
            spin->setDecimals(2);
            spin->setValue(product.price);
            priceLayout->addWidget(new QLabel("Price: $"));
            priceLayout->addWidget(spin);
            priceLayout->addWidget(new QLabel(QString("/ %1")
                                                  .arg(QString::fromStdString(product.pricingUnits))));
            detailLayout->addLayout(priceLayout);

            // --- Main layout ---
            QHBoxLayout *main = new QHBoxLayout(this);
            main->addWidget(imgLabel);
            main->addLayout(detailLayout);
            main->addStretch();

            // --- Connect spin‐box back to product.price ---
            connect(spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                    this, [this](double val)
                    { m_product.price = static_cast<float>(val); });
        }

    private:
        Product &m_product;
    };

}
