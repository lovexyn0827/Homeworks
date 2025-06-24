#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSpinBox>

#include "personset.h"
#include "util.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(PersonSet & set, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PersonSet & storage;
    bool prevAscending;

private:
    void updateTableContents(std::function<bool(const Person &)> predicate = [](const Person & p) -> bool {
        return true;
    });
    void setupWidgets();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
};
#endif // MAINWINDOW_H
