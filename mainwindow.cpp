#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QActionEvent>
#include <QFileDialog>
#include <QContextMenuEvent>

#include <cstring>
#include <algorithm>

#include "editpersoninfodialog.h"

MainWindow::MainWindow(PersonSet & set, const std::string & file, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    storage(set),
    operationHistory(storage),
    prevFile(file)
{
    ui->setupUi(this);

    this->prevAscending = true;
    this->prevSortKeyColumn = 0;
    this->personSetDirty = false;

    this->setupWidgets();
    this->setupMenuBar();
    this->setupToolBar();
    this->setupStatusBar();

    this->setWindowTitle(QString::fromUtf8("人事管理系统"));
    this->setWindowIcon(QIcon(":/lovexyn0827/personnel/res/icon/add.png"));
}

void MainWindow::setupWidgets() {
    QTableWidget * table = this->ui->personTable;
    table->setColumnCount(5);
    table->setRowCount(this->storage.size());
    QStringList labels;
    labels << QString::fromUtf8("身份证")
           << QString::fromUtf8("姓名")
           << QString::fromUtf8("性别")
           << QString::fromUtf8("生日")
           << QString::fromUtf8("工资");
    table->setHorizontalHeaderLabels(labels);
    table->setColumnWidth(0, 150);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setSortIndicatorShown(true);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(
        table,
        &QTableWidget::customContextMenuRequested,
        this,
        [this, table](const QPoint & pos) -> void {
            QMenu * contextMenu = new QMenu();
            connect(
                contextMenu->addAction(QString::fromUtf8("查看")),
                &QAction::triggered,
                this,
                [&pos, this, table](bool bl) -> void {
                    this->openViewDialog(pos.y() / table->rowHeight(0));
                }
            );
            connect(
                contextMenu->addAction(QString::fromUtf8("编辑")),
                &QAction::triggered,
                this,
                [&pos, this, table](bool bl) -> void {
                    this->openModifyDialog(pos.y() / table->rowHeight(0));
                }
            );
            connect(
                contextMenu->addAction(QString::fromUtf8("删除")),
                &QAction::triggered,
                this,
                [&pos, this, table](bool bl) -> void {
                    this->deleteSeclectedRows();
                }
            );
            connect(
                contextMenu->addAction(QString::fromUtf8("新建")),
                &QAction::triggered,
                this,
                [&pos, this, table](bool bl) -> void {
                    this->openAddDialog();
                }
            );
            contextMenu->exec(table->viewport()->mapToGlobal(pos));
        }
    );
    this->updateTableContents();
    connect(
        table->horizontalHeader(),
        &QHeaderView::sectionClicked,
        this,
        [this](int col) -> void {
            this->prevSortKeyColumn = col;
            this->sort(true);
        }
    );
    connect(
        table,
        &QTableWidget::doubleClicked,
        this,
        [this](const QModelIndex & idx) -> void {
            this->openViewDialog(idx.row());
        }
    );
    this->ui->filterKeyInput->setEnabled(false);
    this->ui->filterBtn->setEnabled(false);
    connect(
        this->ui->filterEnabled,
        &QCheckBox::checkStateChanged,
        this,
        [this](Qt::CheckState state) -> void {
            bool filterNeeded = this->shouldFilter();
            this->ui->filterKeyInput->setEnabled(filterNeeded);
            this->ui->filterBtn->setEnabled(filterNeeded);
            if (!filterNeeded) {
                this->updateTableContents();
            } else {
                this->filter();
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
    QMenuBar * menu = this->ui->menubar;
    QMenu * fileMenu = new QMenu(QString::fromUtf8("文件"));
    QAction * newAction = new QAction(QString::fromUtf8("新建"));
    QAction * openAction = new QAction(QString::fromUtf8("打开"));
    QAction * saveAction = new QAction(QString::fromUtf8("保存"));
    QAction * saveAsAction = new QAction(QString::fromUtf8("另存为"));
    QAction * closeAction = new QAction(QString::fromUtf8("关闭"));
    connect(
        newAction,
        &QAction::triggered,
        this,
        [this](bool bl) -> void {
            if (!this->personSetDirty || this->openSaveConfirmationDialog()) {
                this->personSetDirty = false;
                this->prevFile = "";
                this->storage.clear();
                this->updateTableContents();
            } else {
                this->openSaveDialog();
            }
        }
    );
    connect(
        openAction,
        &QAction::triggered,
        this,
        [this](bool bl) -> void {
            QFileDialog * fileChooser = new QFileDialog();
            fileChooser->setFileMode(QFileDialog::ExistingFile);
            if (fileChooser->exec() == QFileDialog::Accepted) {
                QList<QString> chosenFiles = fileChooser->selectedFiles();
                if (!chosenFiles.empty()) {
                    this->prevFile = chosenFiles.first().toStdString();
                    this->storage.ReadFromFile(chosenFiles.first().toStdString().c_str());
                    this->updateTableContents();
                    if (this->shouldFilter()) {
                        this->filter();
                    }
                }
            }
        }
    );
    connect(
        saveAction,
        &QAction::triggered,
        this,
        [this](bool bl) -> void {
            if (this->prevFile.empty()) {
                this->openSaveDialog();
            }

            this->storage.WriteToFile(this->prevFile.c_str());
            this->personSetDirty = false;
        }
    );
    connect(
        saveAsAction,
        &QAction::triggered,
        this,
        [this](bool bl) -> void {
            this->openSaveDialog();
        }
    );
    connect(
        closeAction,
        &QAction::triggered,
        this,
        [this](bool bl) -> void {
            if (!this->personSetDirty || this->openSaveConfirmationDialog()) {
                this->close();
            } else {
                this->openSaveDialog();
            }
        }
    );
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);
    menu->addMenu(fileMenu);
    QMenu * aboutMenu = new QMenu(QString::fromUtf8("关于"));
    QAction * versionAction = new QAction(QString::fromUtf8("版本信息"));
    connect(
        versionAction,
        &QAction::triggered,
        this,
        [this](bool bl) -> void {
            QMessageBox::about(this, QString::fromUtf8("关于"), QString::fromUtf8("人事管理系统 v20200625"));
        }
    );
    aboutMenu->addAction(versionAction);
    menu->addMenu(aboutMenu);
}

void MainWindow::setupToolBar() {
    this->ui->toolBar->setIconSize(QSize(48, 48));
    this->ui->toolBar->addWidget(this->ui->addBtn);
    connect(
        this->ui->addBtn,
        &QToolButton::clicked,
        this,
        &MainWindow::openAddDialog
    );
    this->ui->toolBar->addWidget(this->ui->deleteBtn);
    connect(
        this->ui->deleteBtn,
        &QToolButton::clicked,
        this,
        &MainWindow::deleteSeclectedRows
    );
    this->ui->toolBar->addWidget(this->ui->editBtn);
    connect(
        this->ui->editBtn,
        &QToolButton::clicked,
        this,
        [this]() -> void {
            this->openModifyDialog(this->ui->personTable->currentRow());
        }
    );
    this->ui->toolBar->addWidget(this->ui->viewBtn);
    connect(
        this->ui->viewBtn,
        &QToolButton::clicked,
        this,
        [this]() -> void {
            this->openViewDialog(this->ui->personTable->currentRow());
        }
    );
    this->ui->toolBar->addWidget(this->ui->undoBtn);
    this->ui->undoBtn->setEnabled(false);
    connect(
        this->ui->undoBtn,
        &QToolButton::clicked,
        this,
        [this]() -> void {
            if (!this->operationHistory.undo()) {
                this->ui->undoBtn->setEnabled(false);
            }

            this->ui->redoBtn->setEnabled(true);
            this->updateTableContents();
        }
    );
    this->ui->toolBar->addWidget(this->ui->redoBtn);
    this->ui->redoBtn->setEnabled(false);
    connect(
        this->ui->redoBtn,
        &QToolButton::clicked,
        this,
        [this]() -> void {
            if (!this->operationHistory.redo()) {
                this->ui->redoBtn->setEnabled(false);
            }

            this->ui->undoBtn->setEnabled(true);
            this->updateTableContents();
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


void MainWindow::openViewDialog(int row) {
    if (row == NIL_ROW) {
        return;
    }

    std::string id = this->ui->personTable->item(row, 0)->text().toStdString();
    Person * selected = this->getPersonById(id);
    if (selected == nullptr) {
        return;
    }

    EditPersonInfoDialog dialog(*selected, false, false, this);
    dialog.exec();
}

void MainWindow::openModifyDialog(int row) {
    if (row == NIL_ROW) {
        return;
    }

    std::string id = this->ui->personTable->item(row, 0)->text().toStdString();
    Person * selected = this->getPersonById(id);
    if (selected == nullptr) {
        return;
    }

    EditPersonInfoDialog dialog(*selected, true, false, this);
    Person original = *selected;
    if (dialog.exec() == QDialog::Accepted) {
        // Person object itself updated in `EditPersonInfoDialog`
        this->updateTableRow(*selected, row);
        this->pushOperation(new ModifyOperation(original, *selected, this->indexOf(selected->GetId())));
    }
}

void MainWindow::openAddDialog() {
    Person newPerson;
    EditPersonInfoDialog dialog(newPerson, true, true, this);
    if (dialog.exec() == QDialog::Accepted) {
        if (!this->storage.push_back(newPerson)) {
            QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("该用户已存在！"));
        }

        if (this->ui->filterEnabled->isChecked()) {
            this->filter();
        } else {
            this->updateTableContents();
        }

        this->pushOperation(new AddOperation(newPerson, this->storage.size() - 1));
    }
}

void MainWindow::deleteSeclectedRows() {
    QList<QTableWidgetItem*> selection = this->ui->personTable->selectedItems();
    QVector<int> selectedRows;
    for (QTableWidgetItem * item : std::as_const(selection)) {
        if (!selectedRows.contains(item->row())) {
            selectedRows.push_back(item->row());
        }
    }

    std::sort(selectedRows.begin(), selectedRows.end(), [](int x, int y) -> bool { return x > y; });
    QTableWidget * table = this->ui->personTable;
    std::list<int> * deletedIndices = new std::list<int>();
    std::list<Person> * deletedPersons = new std::list<Person>();
    for (int row : selectedRows) {
        int idx = 0;
        for (PersonSet::iterator itr = this->storage.begin(); itr < this->storage.end(); itr++, idx++) {
            if ((*itr).GetId() == table->item(row, 0)->text()) {
                deletedPersons->push_back(*itr);
                deletedIndices->push_back(idx);
                this->storage.erase(itr);
                break;
            }
        }

        table->removeRow(row);
    }

    this->pushOperation(new DeleteOperation(*deletedPersons, *deletedIndices));
}

bool MainWindow::openSaveConfirmationDialog() {
    QMessageBox::StandardButton result = QMessageBox::question(
        this,
        QString::fromUtf8("未保存的修改"),
        QString::fromUtf8("有修改未保存，是否放弃并退出？")
    );

    return result == QMessageBox::Yes;
}

void MainWindow::openSaveDialog() {
    QFileDialog * fileChooser = new QFileDialog();
    fileChooser->setFileMode(QFileDialog::AnyFile);
    if (fileChooser->exec() == QFileDialog::Accepted) {
        QList<QString> chosenFiles = fileChooser->selectedFiles();
        if (!chosenFiles.empty()) {
            this->prevFile = chosenFiles.first().toStdString();
            this->storage.WriteToFile(chosenFiles.first().toStdString().c_str());
            this->personSetDirty = false;
        }
    }
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
    table->setRowCount(0);
    for (Person & p : this->storage) {
        if (!predicate(p)) {
            continue;
        }

        table->setRowCount(table->rowCount() + 1);
        this->updateTableRow(p, curRow++, true);

    }

    this->sort();
}

void MainWindow::updateTableRow(const Person & p, int row, bool batched) {
    QTableWidget * table = this->ui->personTable;
    class QTableNumericItem : public QTableWidgetItem {
    private:
        double val;

    public:
        QTableNumericItem(double val) : val(val) {
            this->setText(QString::asprintf("%.0lf", val));
        }

        bool operator<(const QTableWidgetItem &other) const {
            if (typeid(other) == typeid(QTableNumericItem)) {
                return this->val < ((const QTableNumericItem&)other).val;
            } else {
                return this->text() < other.text();
            }
        }
    };

    table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(p.GetId())));
    table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(p.GetName())));
    table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(p.GetSex())));
    table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(p.GetBirthday().format())));
    table->setItem(row, 4, new QTableNumericItem(p.GetSalary()));
    if (!batched) {
        this->sort();
        if (this->shouldFilter()) {
            this->filter();
        }
    }
}

bool MainWindow::shouldFilter() {
    return this->ui->filterEnabled->isChecked();
}

void MainWindow::filter() {
    QString keyword = this->ui->filterKeyInput->text();
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
               person.GetSalary() == keyword.toDouble();
    });
}

void MainWindow::sort(bool toggle) {
    this->ui->personTable->sortByColumn(this->prevSortKeyColumn,
            (this->prevAscending ^= toggle) ? Qt::DescendingOrder : Qt::AscendingOrder);
}

void MainWindow::pushOperation(const Operation * op) {
    this->operationHistory.pushOperation(op);
    this->ui->undoBtn->setEnabled(true);
    this->ui->redoBtn->setEnabled(false);
    this->personSetDirty = true;
}

int MainWindow::indexOf(const std::string & id) {
    int curRow = 0;
    for (Person & p : this->storage) {
        if (p.GetId() == id) {
            return curRow;
        }

        curRow++;
    }

    return NIL_ROW;
}
