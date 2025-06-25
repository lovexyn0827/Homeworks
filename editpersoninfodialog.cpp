#include "editpersoninfodialog.h"
#include "ui_editpersoninfodialog.h"

#include <QMessageBox>
#include <regex>

EditPersonInfoDialog::EditPersonInfoDialog(Person & person, bool editable, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditPersonInfoDialog)
{
    ui->setupUi(this);

    //this->setWindowTitle("");
    this->ui->genderComboBox->addItems(QStringList() << QString::fromUtf8("男") << QString::fromUtf8("女"));
    this->loadPersonInfo(person);
    connect(
        this->ui->buttonBox,
        &QDialogButtonBox::accepted,
        this,
        [this, editable, &person]() -> void {
            if (editable) {
                switch (this->validate()) {
                case OK:
                    this->applyModifications(person);
                    this->accept();
                    return;
                case INVALID_ID:
                    QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("身份证号有误！"));
                    break;
                case INCOMPLETE:
                    QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("必填字段为空！"));
                    break;
                case LOW_SALARY:
                    QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("你就给人家这点工资吗[doge]！"));
                    break;
                default:
                    throw "AssertionError";
                }
            }
        }
    );
    connect(
        this->ui->buttonBox,
        &QDialogButtonBox::rejected,
        this,
        [this, editable, &person]() -> void {
            this->reject();
        }
    );
    if (!editable) {
        this->ui->addressLineEdit->setEnabled(false);
        this->ui->departmentLineEdit->setEnabled(false);
        this->ui->idLineEdit->setEnabled(false);
        this->ui->nameLineEdit->setEnabled(false);
        this->ui->phongNoLineEdit->setEnabled(false);
        this->ui->empolyeeNoLineEdit->setEnabled(false);
        this->ui->posLineEdit->setEnabled(false);
        this->ui->dobEdit->setEnabled(false);
        this->ui->genderComboBox->setEnabled(false);
        this->ui->salarySpinner->setEnabled(false);
        this->ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok);
    } else {
        this->ui->buttonBox->setStandardButtons(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    }
}

EditPersonInfoDialog::~EditPersonInfoDialog() {
    delete ui;
}

void EditPersonInfoDialog::applyModifications(Person & person) {
    person.SetId(this->ui->idLineEdit->text().toStdString());
    person.SetName(this->ui->nameLineEdit->text().toStdString());
    person.SetAddress(this->ui->addressLineEdit->text().toStdString());
    person.SetDepartment(this->ui->departmentLineEdit->text().toStdString());
    person.SetEmployeeNo(this->ui->empolyeeNoLineEdit->text().toStdString());
    person.SetPhoneNo(this->ui->phongNoLineEdit->text().toStdString());
    person.SetPost(this->ui->posLineEdit->text().toStdString());
    person.SetSex(this->ui->genderComboBox->currentText().toStdString());
    person.SetSalary(this->ui->salarySpinner->value());
    const QDate &dob = this->ui->dobEdit->date();
    person.SetBirthday(Date(dob.year(), dob.month(), dob.day()));
}

void EditPersonInfoDialog::loadPersonInfo(const Person & person) {
    this->ui->idLineEdit->setText(QString::fromStdString(person.GetId()));
    this->ui->nameLineEdit->setText(QString::fromStdString(person.GetName()));
    this->ui->addressLineEdit->setText(QString::fromStdString(person.GetAddress()));
    this->ui->departmentLineEdit->setText(QString::fromStdString(person.GetDepartment()));
    this->ui->empolyeeNoLineEdit->setText(QString::fromStdString(person.GetEmployeeNo()));
    this->ui->phongNoLineEdit->setText(QString::fromStdString(person.GetPhoneNo()));
    this->ui->posLineEdit->setText(QString::fromStdString(person.GetPost()));
    this->ui->genderComboBox->setCurrentText(QString::fromStdString(person.GetSex()));
    this->ui->salarySpinner->setValue(person.GetSalary());
    const Date &dob = person.GetBirthday();
    this->ui->dobEdit->setDate(QDate(dob.GetYear(), dob.GetMonth(), dob.GetDay()));
}

EditPersonInfoDialog::ValidationResult EditPersonInfoDialog::validate() {
    if (this->ui->nameLineEdit->text().length() == 0) {
        return INCOMPLETE;
    }

    if (this->ui->idLineEdit->text().length() == 0) {
        return INCOMPLETE;
    }

    if (this->ui->addressLineEdit->text().length() == 0) {
        return INCOMPLETE;
    }

    if (this->ui->departmentLineEdit->text().length() == 0) {
        return INCOMPLETE;
    }

    if (this->ui->empolyeeNoLineEdit->text().length() == 0) {
        return INCOMPLETE;
    }

    if (this->ui->phongNoLineEdit->text().length() == 0) {
        return INCOMPLETE;
    }

    if (this->ui->posLineEdit->text().length() == 0) {
        return INCOMPLETE;
    }

    if (this->ui->salarySpinner->value() < 5000) {
        return LOW_SALARY;
    }

    const std::string & id = this->ui->idLineEdit->text().toStdString();
    std::regex idRegex("^[1-9][0-9]{16}[0-9X]$");
    if (!std::regex_match(id.c_str(), idRegex)) {
        return INVALID_ID;
    }

    if ((this->ui->genderComboBox->currentIndex() == 1) ^ (id[16] % 2 == 0)) {
        return INVALID_ID;
    }

    const QDate &dob = this->ui->dobEdit->date();
    if (id.substr(6, 4) != std::to_string(dob.year())) {
        //return INVALID_ID;
    }

    if (QString::fromStdString(id.substr(10, 2)).toInt() != dob.month()) {
        //return INVALID_ID;
    }

    if (QString::fromStdString(id.substr(12, 2)).toInt() != dob.day()) {
        //return INVALID_ID;
    }

    int checksum = 0;
    int w = 1;
    for (int i = 0; i < 18; i++) {
        char c = id[17 - i];
        int digit = ((c == 'x') ? 10 : c - '0');
        checksum += w * digit;
        w = (w * 2) % 11;
    }

    if (checksum % 11 != 1) {
        return INVALID_ID;
    }

    return OK;
}
