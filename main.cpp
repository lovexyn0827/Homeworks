#include "mainwindow.h"

#include <QApplication>
#include <filesystem>

#include "personset.h"

#define DATA_FILE ("personnel.dat")
#define BACKUP_FILE ("personnel.bak")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PersonSet set;
    set.ReadFromFile(DATA_FILE);
    if (std::filesystem::exists(BACKUP_FILE)) {
        std::filesystem::remove(BACKUP_FILE);
    }

    std::filesystem::copy_file(DATA_FILE, BACKUP_FILE);
    MainWindow w(set);
    w.show();
    int status = a.exec();
    set.WriteToFile(DATA_FILE);
    return status;
}
