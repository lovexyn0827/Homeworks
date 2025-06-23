#include "mainwindow.h"

#include <QApplication>
#include "personset.h"

#define DATA_FILE ("personnel.dat")

void load(PersonSet & set) {
    set.ReadFromFile(DATA_FILE);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
