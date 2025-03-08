#include <sqlite3.h>
#include <string>
#include <iostream>
#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include "logs.hpp"
#include "types.hpp"

#include "mainwindow.h"

#include <QApplication>
#include <QPushButton>
#include <QThread>

using namespace std;

/* Main C++ File:
 *  - Runs the QT UI container
*/
class Container : public QThread {
private:
    QApplication *app;
public:
    Container(QApplication *app) {
        this->app = app;
    }
public:
    void run() {
        MainWindow window;
        window.show();
        app->exec();
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Container *container = new Container(&app);

    container->run();
}
