#include "project_editor.h"
#include "ui_projectEditor.h"
#include <QMainWindow>

ProjectEditorWindow::ProjectEditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProjectsWindow)
{
    ui->setupUi(this);   
}

ProjectEditorWindow::~ProjectEditorWindow() {
}