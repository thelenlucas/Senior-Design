#ifndef UI_LOGS_HPP
#define UI_LOGS_HPP

#include <QtWidgets/QWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtGui/QPixmap>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QFormLayout>

class UI_LogsPage : public QWidget
{
    Q_OBJECT

public:
    explicit UI_LogsPage(QWidget *parent = nullptr);

private:
    QTableView*     log_table;              // Table for the log inventory.
    QLabel*         image_label;            // Placeholder for log images.
    QLineEdit*      cost_field;             // Placeholder for line edit for the cost input.
    QLineEdit*      size_field;             // Placeholder for the size input.
    QLineEdit*      species_field;          // Placeholder for the species input.
    QLineEdit*      price_field;            // Placehold for price calculation output.
    QPushButton*    post_for_sale_button;   // Button to generate the HTML listing on our website.
    QPushButton*    update_price_button;    // Button to update the price calculation after all inputs.

    void Setup();
};

#endif // UI_LOGS_HPP