#include "macdialog.h"
#include "ui_macdialog.h"
#include <QMessageBox>
#include <QStringList>
#include <QIcon>

MACDialog::MACDialog(QWidget *parent, int type) :
    QDialog(parent),
    ui(new Ui::MACDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    if(0 == type)
    {
        ui->label_2->hide();
        ui->lineEdit_2->hide();
    }
    else
    {
        ui->label->setText("MAC地址");
    }
    m_type = type;
}

MACDialog::~MACDialog()
{
    delete ui;
}

void MACDialog::accept()
{
    if(0 == m_type)
    {
        if("" == ui->lineEdit->text())
        {
            QMessageBox::information(this, "提示", "请填写MAC地址\n", QMessageBox::Ok);
        }
        else
        {
            QStringList totalList = ui->lineEdit->text().split('/');
            if(totalList.size() != 2)
            {
                QMessageBox::information(this, "提示", "MAC地址与序列号不符合规范\n请重新扫描", QMessageBox::Ok);
            }
            else
            {
                QStringList macList = totalList.at(1).split(':');
                if(macList.size() != 6)
                {
                    QMessageBox::information(this, "提示", "MAC地址不符合规范\n请重新扫描", QMessageBox::Ok);
                }
                else
                {
                    if(totalList.at(0).size() != 12)
                    {
                        QMessageBox::information(this, "提示", "序列号格式错误\n请重新扫描", QMessageBox::Ok);
                    }
                    else
                    {
                        emit signalMAC(totalList.at(1));
                        emit signalSerial(totalList.at(0));
                    }
                }
            }
        }
    }
    else
    {
        if("" == ui->lineEdit->text())
        {
            QMessageBox::information(this, "提示", "请填写MAC地址\n", QMessageBox::Ok);
        }
        else
        {
            QStringList macList = ui->lineEdit->text().split(':');
            if(macList.size() != 6)
            {
                QMessageBox::information(this, "提示", "MAC地址不符合规范\n请重新扫描", QMessageBox::Ok);
            }
            else
            {
                if(ui->lineEdit_2->text().size() != 12)
                {
                    if(if_first)
                    {
                        ui->lineEdit_2->setFocus();
                        if_first = false;
                    }
                    else
                    {
                        QMessageBox::information(this, "提示", "序列号格式错误\n请重新扫描", QMessageBox::Ok);
                    }
                }
                else
                {
                    if_first = false;
                    emit signalMAC(ui->lineEdit->text());
                    emit signalSerial(ui->lineEdit_2->text());
                }
            }
        }
    }
}
