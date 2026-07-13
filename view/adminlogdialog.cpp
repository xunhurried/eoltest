#include "adminlogdialog.h"
#include "ui_adminlogdialog.h"
#include <QMessageBox>

AdminLogDialog::AdminLogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminLogDialog)
{
    ui->setupUi(this);
    setWindowTitle("登录界面");
    ui->lineEdit->setEchoMode(QLineEdit::Password);
}

AdminLogDialog::~AdminLogDialog()
{
    delete ui;
}

void AdminLogDialog::accept()
{
    if(ui->lineEdit->text() == "gold2023")
    {
        emit signalSuccess();
        QMessageBox::information(this, "提示", "登录成功\n", QMessageBox::Ok);
        hide();
    }
    else
    {
        QMessageBox::warning(this, "提示", "密码错误\n", QMessageBox::Ok);
    }
}
