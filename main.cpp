#include "mainwindow.h"

#include <QApplication>
#include "personset.h"

#define DATA_FILE ("personnel.dat")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PersonSet set;
    set.ReadFromFile(DATA_FILE);
    MainWindow w(set);
    w.show();
    return a.exec();
}
