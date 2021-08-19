#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QFont>

int main(int argc, char *argv[])
{
    QFont f {"Microsoft YaHei"};
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication::setFont(f);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
