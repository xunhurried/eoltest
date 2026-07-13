#include "choosedevicedialog.h"
#include "ui_choosedevicedialog.h"
#include <QDebug>
#include <QIcon>
#include <QTimer>

ChooseDeviceDialog::ChooseDeviceDialog(QWidget *parent, int type) :
    QDialog(parent), ui(new Ui::ChooseDeviceDialog), m_type(type)
{
    ui->setupUi(this);
    setWindowTitle("CAN卡选择");
    setWindowIcon(QIcon(":/image/logo.ico"));
    switch(type)
    {
    case 1:
    {
        ui->deviceTypeBox->addItem("老版分析仪");
        ui->deviceTypeBox->addItem("新版分析仪");
        ui->deviceTypeBox->addItem("周立功CAN卡");
        ui->deviceTypeBox->addItem("广成CAN卡");
        break;
    }
    case 2:
    {
        ui->deviceTypeBox->addItem("PCI-5121");
        ui->deviceTypeBox->addItem("PCI-9810I");
        ui->deviceTypeBox->addItem("USBCAN-I/I+");
        ui->deviceTypeBox->addItem("USBCAN-II/II+");
        ui->deviceTypeBox->addItem("PCI-9820");
        ui->deviceTypeBox->addItem("CAN232");
        ui->deviceTypeBox->addItem("PCI-5110");
        ui->deviceTypeBox->addItem("CANmini");
        ui->deviceTypeBox->addItem("ISA-9620");
        ui->deviceTypeBox->addItem("ISA-5420");
        ui->deviceTypeBox->addItem("PC104-CAN");
        ui->deviceTypeBox->addItem("CANET系列的UDP工作方式");
        ui->deviceTypeBox->addItem("DN-PCI9810");
        ui->deviceTypeBox->addItem("PCI-9840I");
        ui->deviceTypeBox->addItem("PC104-CAN2I");
        ui->deviceTypeBox->addItem("PCI-9820I");
        ui->deviceTypeBox->addItem("CANET系列的TCP工作方式");
        ui->deviceTypeBox->addItem("PEC-9920");
        ui->deviceTypeBox->addItem("PCIe-9220");
        ui->deviceTypeBox->addItem("PCI-5010-U");
        ui->deviceTypeBox->addItem("USBCAN-E-U");
        ui->deviceTypeBox->addItem("USBCAN-2E-U");
        ui->deviceTypeBox->addItem("PCI-5020-U");
        ui->deviceTypeBox->addItem("EG20T-CAN");
        ui->deviceTypeBox->addItem("PCIe-9221");
        ui->deviceTypeBox->addItem("CANWiFi-200T的TCP工作方式");
        ui->deviceTypeBox->addItem("CANWiFi-200T的UDP工作方式");
        ui->deviceTypeBox->addItem("PCIe-9120I");
        ui->deviceTypeBox->addItem("PCIe-9110I");
        ui->deviceTypeBox->addItem("PCIe-9140I");
        ui->deviceTypeBox->addItem("USBCAN-4E-U");
        ui->deviceTypeBox->addItem("CANDTU");
        ui->deviceTypeBox->addItem("USBCAN-8E-U");
        ui->deviceTypeBox->addItem("CANDTU-NET");
        break;
    }
    case 3:
    {
        ui->deviceTypeBox->addItem("USBCAN I");
        ui->deviceTypeBox->addItem("USBCAN II");
        break;
    }
    default:
        break;
    }
    QTimer::singleShot(100, this, [=]()
    {
        slotConfirm();
    });
}

ChooseDeviceDialog::~ChooseDeviceDialog()
{
    delete ui;
}

void ChooseDeviceDialog::hideLabel()
{
    ui->label->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->idBox->hide();
    ui->passBox->hide();
    ui->portBox->hide();
}

void ChooseDeviceDialog::slotConfirm()
{
    switch(m_type)
    {
    case 1:
    {
        emit signalChooseDevice(ui->deviceTypeBox->currentIndex(), ui->idBox->currentIndex(), ui->passBox->currentIndex()
                                , ui->portBox->currentIndex());
        break;
    }
    case 2:
    {
        emit signalChooseZLGType(ui->deviceTypeBox->currentIndex() + 1, ui->idBox->currentIndex(), ui->passBox->currentIndex()
                            , ui->portBox->currentIndex());
        break;
    }
    case 3:
    {
        emit signalChooseGCType(0 == ui->deviceTypeBox->currentIndex() ? 3 : 4, ui->idBox->currentIndex(), ui->passBox->currentIndex()
                           , ui->portBox->currentIndex());
        break;
    }
    default:
        break;
    }
    hide();
}
