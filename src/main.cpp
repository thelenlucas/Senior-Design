#include <string>
#include <iostream>
#include <vector>

#include <sqlite3.h>

#include <SQLiteCpp/SQLiteCpp.h>

#include <QApplication>
#include <QPushButton>
#include <QThread>

#include "logs.hpp"
#include "types.hpp"
#include "mainwindow.hpp"


using namespace std;

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
