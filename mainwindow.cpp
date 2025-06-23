#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <cstdio>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*connect(this->ui->Btn_20250622, &QPushButton::clicked, this, [=]() -> void {
        QMessageBox::information(this, "Title", "boobar");
    });
    this->ui->Btn_20250622->setIcon(QIcon("C:/Users/angca/Pictures/icon_480.png"));
    connect(this->ui->press_me, &QPushButton::pressed, this, [this]() -> void {
        if (!this->ui->loadEnCheckBox->isChecked()) {
            QMessageBox::information(this, "Title", this->ui->filterKeyInput->text());
        } else {
            int rnd = rand() & 167777215;
            char buf[8];
            sprintf(buf, "%x", rnd);
            this->ui->filterKeyInput->setText(buf);
        }
    });*/
    this->ui->personTable->clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}
