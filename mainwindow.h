#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSpinBox>

#include "personset.h"
#include "operationhistory.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(PersonSet & set, const std::string & file, QWidget *parent = nullptr);
    ~MainWindow();

private:
    constexpr static int NIL_ROW = -1;
    Ui::MainWindow *ui;
    PersonSet & storage;
    OperationHistory operationHistory;
    bool prevAscending;
    int prevSortKeyColumn;
    bool personSetDirty;
    std::string prevFile;

private:
    void updateTableContents(std::function<bool(const Person &)> predicate = [](const Person & p) -> bool {
        return true;
    });
    void updateTableRow(const Person & p, int row, bool batched = false);
    Person* getPersonById(const std::string & id);
    void setupWidgets();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void openViewDialog(int row);
    void openModifyDialog(int row);
    void openAddDialog();
    void deleteSeclectedRows();
    bool openSaveConfirmationDialog();
    void openSaveDialog();
    void filter();
    bool shouldFilter();
    void sort(bool toggle = false);
    void pushOperation(const Operation * op);
    int indexOf(const std::string & id);
};
#endif // MAINWINDOW_H
