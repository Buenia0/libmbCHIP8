#include <QApplication>
#include "string"
#include "mainwindow.h"
using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    string guititle = string("mbCHIP8-Qt v0.1");
    QString guiapptitle = QString::fromStdString(guititle);

    MainWindow window;
    window.setWindowTitle(guiapptitle);
    window.show();

    return app.exec();
}
