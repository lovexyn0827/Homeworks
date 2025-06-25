#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSpinBox>

#include "personset.h"

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
    bool prevSortKeyColumn;

private:
    void updateTableContents(std::function<bool(const Person &)> predicate = [](const Person & p) -> bool {
        return true;
    });
    void updateTableRow(const Person & p, int row, bool sort = true);
    Person* getPersonById(const std::string & id);
    void setupWidgets();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void filter();
};
#endif // MAINWINDOW_H
