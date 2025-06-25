#ifndef EDITPERSONINFODIALOG_H
#define EDITPERSONINFODIALOG_H

#include <QDialog>
#include <QDate>

#include "person.h"

namespace Ui {
class EditPersonInfoDialog;
}

class EditPersonInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditPersonInfoDialog(Person & person, bool editable, QWidget *parent = nullptr);
    ~EditPersonInfoDialog();

private:
    Ui::EditPersonInfoDialog *ui;

private:
    enum ValidationResult {
        OK,
        INVALID_ID,
        INCOMPLETE,
        LOW_SALARY
    };

private:
    void loadPersonInfo(const Person & person);
    void applyModifications(Person & person);
    ValidationResult validate();
};

#endif // EDITPERSONINFODIALOG_H
