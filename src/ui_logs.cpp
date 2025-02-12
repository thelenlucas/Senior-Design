#include <QHBoxLayout>
#include <QHeaderView>

#include "ui_logs.hpp"

UI_LogsPage::UI_LogsPage(QWidget* parent) : QWidget(parent)
{
    Setup();
}

void UI_LogsPage::Setup()
{
    QVBoxLayout* main_layout = new QVBoxLayout(this);

    // Log Inventory Table.
    log_table = new QTableView(this);
    log_table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    log_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    main_layout->addWidget(new QLabel("Logs Inventory"));
    main_layout->addWidget(log_table);

    // Log Details Form.
    // This is where we let the user (Sawyers) enter their item details before posting for sale. In this case it's logs.
    QGroupBox* details_box = new QGroupBox("Log Details");
    QFormLayout* details_layout = new QFormLayout(details_box);
    species_field = new QLineEdit("Oak");
    size_field = new QLineEdit("12 ft x 24 in");
    cost_field = new QLineEdit("$150");
    price_field = new QLineEdit("$200"); // Recalculated price will eventually go here.

    details_layout->addRow("Species:", species_field);
    details_layout->addRow("Size:", size_field);
    details_layout->addRow("Cost:", cost_field);
    details_layout->addRow("Current Price:", price_field);

    main_layout->addWidget(details_box);

    // Images Section. Maybe we'll allow them to add more than one? Only one for now though.
    image_label = new QLabel(this);
    image_label->setPixmap(QPixmap(":/resource/images/log_placeholder.jpg").scaled(200, 150, Qt::KeepAspectRatio));
    image_label->setAlignment(Qt::AlignCenter);
    main_layout->addWidget(image_label);

    // Action Buttons.
    QHBoxLayout* buttons_layout = new QHBoxLayout();
    post_for_sale_button = new QPushButton("Post for Sale");
    update_price_button = new QPushButton("Update Price");

    buttons_layout->addWidget(post_for_sale_button);
    buttons_layout->addWidget(update_price_button);

    main_layout->addLayout(buttons_layout);

    setLayout(main_layout);
}