#ifndef PROJECT_EDITOR_H
#define PROJECT_EDITOR_H

#include <QMainWindow>

namespace Ui
{
    class ProjectsWindow;
}

class ProjectEditorWindow : public QMainWindow
{
    Q_OBJECT
public:
    ProjectEditorWindow(QWidget *parent = 0);
    ~ProjectEditorWindow();

private:
    Ui::ProjectsWindow *ui;

    void updateProjects();
    void updateDisplay();

private slots:
    void onProjectSelected();
};

#endif // PROJECT_EDITOR_H