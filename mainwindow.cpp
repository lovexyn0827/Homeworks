#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QLabel>
#include <QTimer>
#include <QDateTime>

#include <cstring>
#include <algorithm>

#include "util.h"
#include "editpersoninfodialog.h"

MainWindow::MainWindow(PersonSet & set, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    storage(set)
{
    ui->setupUi(this);

    this->prevAscending = false;
    this->prevSortKeyColumn = 0;

    this->setupWidgets();
    this->setupMenuBar();
    this->setupToolBar();
    this->setupStatusBar();

    this->setWindowTitle(QString::fromUtf8("人事管理系统"));
}


void MainWindow::setupWidgets() {
    QTableWidget * table = this->ui->personTable;
    table->setColumnCount(5);
    table->setRowCount(this->storage.size());
    table->setColumnWidth(0, 150);
    QStringList labels;
    labels << QString::fromUtf8("身份证")
           << QString::fromUtf8("姓名")
           << QString::fromUtf8("性别")
           << QString::fromUtf8("生日")
           << QString::fromUtf8("工资");
    table->setHorizontalHeaderLabels(labels);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->updateTableContents();
    connect(
        table->horizontalHeader(),
        &QHeaderView::sectionClicked,
        this,
        [=](int col) {
            table->sortByColumn(col, (this->prevAscending ^= true) ? Qt::DescendingOrder : Qt::AscendingOrder);
        }
    );
    this->ui->filterKeyInput->setEnabled(false);
    this->ui->filterBtn->setEnabled(false);
    connect(
        this->ui->filterEnabled,
        &QCheckBox::checkStateChanged,
        this,
        [this](Qt::CheckState state) -> void {
            this->ui->filterKeyInput->setEnabled(state == Qt::Checked);
            this->ui->filterBtn->setEnabled(state == Qt::Checked);
            if (state != Qt::Checked) {
                this->updateTableContents();
            }
        }
    );
    connect(
        this->ui->filterBtn,
        &QPushButton::clicked,
        this,
        [this]() -> void {
            this->filter();
        }
    );
}

void MainWindow::setupMenuBar() {

}

void MainWindow::setupToolBar() {
    this->ui->toolBar->setIconSize(QSize(48, 48));
    this->ui->toolBar->addWidget(this->ui->addBtn);
    connect(
        this->ui->addBtn,
        &QToolButton::clicked,
        this,
        [this]() -> void {
            Person *newPerson = new Person();
            EditPersonInfoDialog * dialog = new EditPersonInfoDialog(*newPerson, true, this);
            dialog->show();
            connect(
                dialog,
                &QDialog::accepted,
                this,
                [this, newPerson]() -> void {
                    if (!this->storage.push_back(*newPerson)) {
                        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("该用户已存在！"));
                    }

                    if (this->ui->filterEnabled->isChecked()) {
                        this->filter();
                    } else {
                        this->updateTableContents();
                    }
                }
            );
        }
    );
    this->ui->toolBar->addWidget(this->ui->deleteBtn);
    connect(
        this->ui->deleteBtn,
        &QToolButton::clicked,
        this,
        [this]() -> void {
            QList<QTableWidgetItem*> selection = this->ui->personTable->selectedItems();
            QVector<int> selectedRows;
            for (QTableWidgetItem * item : std::as_const(selection)) {
                selectedRows.push_back(item->row());
            }

            std::sort(selectedRows.begin(), selectedRows.end(), [](int x, int y) -> bool { return x > y; });
            QTableWidget * table = this->ui->personTable;
            for (int row : selectedRows) {
                for (PersonSet::iterator itr = this->storage.begin(); itr < this->storage.end(); itr++) {
                    if ((*itr).GetId() == table->item(row, 0)->text()) {
                        this->storage.erase(itr);
                        break;
                    }
                }

                table->removeRow(row);
            }
        }
    );
    this->ui->toolBar->addWidget(this->ui->editBtn);
    connect(
        this->ui->editBtn,
        &QToolButton::clicked,
        this,
        [this]() -> void {
            int curRow = this->ui->personTable->currentRow();
            std::string id = this->ui->personTable->item(curRow, 0)->text().toStdString();
            Person * selected = this->getPersonById(id);
            if (selected == nullptr) {
                return;
            }

            EditPersonInfoDialog * dialog = new EditPersonInfoDialog(*selected, true, this);
            dialog->show();
            connect(
                dialog,
                &QDialog::accepted,
                this,
                [selected, curRow, this]() -> void {
                    this->updateTableRow(*selected, curRow);
                }
            );
        }
    );
    this->ui->toolBar->addWidget(this->ui->viewBtn);
    connect(
        this->ui->viewBtn,
        &QToolButton::clicked,
        this,
        [this]() -> void {
            int curRow = this->ui->personTable->currentRow();
            std::string id = this->ui->personTable->item(curRow, 0)->text().toStdString();
            Person * selected = this->getPersonById(id);
            if (selected == nullptr) {
                return;
            }

            EditPersonInfoDialog * dialog = new EditPersonInfoDialog(*selected, false, this);
            dialog->show();
            connect(dialog, &QDialog::accepted, this, []() -> void {});
        }
        );
    this->ui->toolBar->addWidget(this->ui->undoBtn);
    connect(
        this->ui->undoBtn,
        &QToolButton::clicked,
        this,
        [this]() -> void {
            // TODO
        }
    );
    this->ui->toolBar->addWidget(this->ui->redoBtn);
    connect(
        this->ui->redoBtn,
        &QToolButton::clicked,
        this,
        [this]() -> void {
            // TODO
        }
    );
}

void MainWindow::setupStatusBar() {
    QStatusBar * statusBar = this->ui->statusbar;
    QLabel * cnu = new QLabel("Capital Normal University");
    statusBar->addPermanentWidget(cnu);
    QTimer * timer = new QTimer();
    QLabel * time = new QLabel(QDateTime::currentDateTime().toString());
    statusBar->addWidget(time);
    timer->setInterval(1000);
    timer->start();
    connect(
        timer,
        &QTimer::timeout,
        this,
        [time]() -> void {
            time->setText(QDateTime::currentDateTime().toString());
        }
    );
}

MainWindow::~MainWindow()
{
    delete ui;
}

Person* MainWindow::getPersonById(const std::string & id) {
    int curRow = 0;
    for (Person & p : this->storage) {
        if (p.GetId() == id) {
            return &p;
        }
    }

    return nullptr;
}

void MainWindow::updateTableContents(std::function<bool(const Person &)> predicate) {
    int curRow = 0;
    QTableWidget * table = this->ui->personTable;
    table->clear();
    table->setRowCount(0);
    for (Person & p : this->storage) {
        if (!predicate(p)) {
            continue;
        }

        table->setRowCount(table->rowCount() + 1);
        this->updateTableRow(p, curRow++, false);

    }

    table->sortByColumn(this->prevSortKeyColumn, this->prevAscending ? Qt::DescendingOrder : Qt::AscendingOrder);
}

void MainWindow::updateTableRow(const Person & p, int row, bool sort) {
    QTableWidget * table = this->ui->personTable;
    table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(p.GetId())));
    table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(p.GetName())));
    table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(p.GetSex())));
    table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(p.GetBirthday().format())));
    table->setItem(row, 4, new QTableWidgetItem(QString::number(p.GetSalary())));
    if (sort) {
        table->sortByColumn(this->prevSortKeyColumn, this->prevAscending ? Qt::DescendingOrder : Qt::AscendingOrder);
    }
}

void MainWindow::filter() {
    std::string keyword = this->ui->filterKeyInput->text().toStdString();
    this->updateTableContents([&keyword](const Person & person) -> bool {
        return person.GetId() == keyword ||
               person.GetName() == keyword ||
               person.GetSex() == keyword ||
               person.GetPhoneNo() == keyword ||
               person.GetAddress() == keyword ||
               person.GetBirthday().format() == keyword ||
               person.GetEmployeeNo() == keyword ||
               person.GetDepartment() == keyword ||
               person.GetPost() == keyword ||
               std::to_string(person.GetSalary()) == keyword;
    });
}
