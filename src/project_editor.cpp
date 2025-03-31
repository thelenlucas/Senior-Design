#include <SQLiteCpp/SQLiteCpp.h>

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

#include "project_editor.hpp"
#include "ui_projectEditor.h"
#include "types.hpp"

ProjectEditorWindow::ProjectEditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProjectsWindow)
{
    ui->setupUi(this);
    updateProjects();

    // Attatch slot
    connect(ui->projectsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onProjectSelected()));
}

void ProjectEditorWindow::updateProjects() {
    ui->projectsCombo->clear();

    // Get all "Name" under "projects" view
    SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READONLY);
    SQLite::Statement query(db, "SELECT Name FROM projects");
    while (query.executeStep()) {
        ui->projectsCombo->addItem(query.getColumn(0).getText());
    }
    // Set the first project as the default
    if (ui->projectsCombo->count() > 0) {
        ui->projectsCombo->setCurrentIndex(0);
    }
    onProjectSelected();
}

void ProjectEditorWindow::onProjectSelected() {
    updateDisplay();
}

void ProjectEditorWindow::updateDisplay() {
    // Get project name
    QString projectName = ui->projectsCombo->currentText();

    // For "General", we would do "SELECT * FROM cutlist WHERE project = General AND done = 0"
    // for only not done cuts
    QString query = "SELECT * FROM cutlist_view WHERE Project = '" + projectName + "'";
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(query, QSqlDatabase::database());
    ui->cutlistView->setModel(model);

    if (model->lastError().isValid()) {
        qDebug() << "Error: " << model->lastError().text();
        return;
    }

    // Stretch
    ui->cutlistView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Hide the first column
    ui->cutlistView->hideColumn(0);
}

ProjectEditorWindow::~ProjectEditorWindow() {
}