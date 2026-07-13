#include "serialdialog.h"
#include "ui_serialdialog.h"
#include <QMessageBox>
#include <QIcon>

SerialDialog::SerialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
}

SerialDialog::~SerialDialog()
{
    delete ui;
}

void SerialDialog::accept()
{
    if("" == ui->lineEdit->text())
    {
        QMessageBox::information(this, "提示", "请填写序列号\n", QMessageBox::Ok);
    }
    else if(ui->lineEdit->text().length() != 12)
    {
        QMessageBox::information(this, "提示", "序列号格式错误\n请重新扫描", QMessageBox::Ok);
    }
    else
    {
        emit signalSerial(ui->lineEdit->text());
    }
}
