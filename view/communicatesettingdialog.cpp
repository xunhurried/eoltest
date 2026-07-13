#include "communicatesettingdialog.h"
#include "ui_communicatesettingdialog.h"
#include <QStringList>
#include <QSettings>
#include <QDir>
#include <QSerialPortInfo>
#include <QTimer>

CommunicateSettingDialog::CommunicateSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommunicateSettingDialog)
{
    ui->setupUi(this);
    setWindowTitle("通讯设置");
    QStringList portList, serialportinfo;
    portList << "600" << "1200" << "2400" << "4800" << "9600" << "14400" << "19200" << "38400";
    ui->portBox->addItems(portList);
    foreach(QSerialPortInfo info,QSerialPortInfo::availablePorts())
    {
        serialportinfo << info.portName();
    }
    ui->SeriolNumberBox->addItems(serialportinfo);
    ui->SeriolNumberBox->setCurrentIndex(0);
    ui->portBox->setCurrentIndex(4);
    QSettings setting(QDir::currentPath() + "/version.ini", QSettings::IniFormat);
    QString port = setting.value("serialPort").toString();
    ui->SeriolNumberBox->setCurrentIndex(ui->SeriolNumberBox->findText(port));
    QTimer::singleShot(100, this, [=]()
    {
        accept();
    });
}


CommunicateSettingDialog::~CommunicateSettingDialog()
{
    delete ui;
}

void CommunicateSettingDialog::accept()
{
    emit signalStartSeriol(ui->SeriolNumberBox->currentText(), ui->portBox->currentText().toInt());
    hide();
}
