#include "choosetypedialog.h"
#include "ui_choosetypedialog.h"
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QTimer>

ChooseTypeDialog::ChooseTypeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chooseTypeDialog)
{
    ui->setupUi(this);
    setWindowTitle("请选择测试类型");
    setWindowIcon(QIcon(":/image/logo.ico"));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup1(int)));
    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup1(int)));
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup1(int)));
    connect(ui->comboBox_4, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup1(int)));
    connect(ui->comboBox_5, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup2(int)));
    connect(ui->comboBox_6, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup2(int)));
    connect(ui->comboBox_7, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup2(int)));
    connect(ui->comboBox_8, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup2(int)));
    connect(ui->comboBox_9, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup3(int)));
    connect(ui->comboBox_10, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup3(int)));
    connect(ui->comboBox_11, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup3(int)));
    connect(ui->comboBox_12, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup3(int)));
    connect(ui->comboBox_13, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup4(int)));
    connect(ui->comboBox_14, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup4(int)));
    connect(ui->comboBox_15, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup4(int)));
    connect(ui->comboBox_16, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGroup4(int)));
    QSettings setting(QDir::currentPath() + "/version.ini", QSettings::IniFormat);
    ui->radioButton->setChecked(setting.value("ifmes").toBool());
    ui->comboBox->setCurrentIndex(setting.value("type1").toInt());
    ui->comboBox_2->setCurrentIndex(setting.value("type2").toInt());
    ui->comboBox_3->setCurrentIndex(setting.value("type3").toInt());
    ui->comboBox_4->setCurrentIndex(setting.value("type4").toInt());
    ui->comboBox_5->setCurrentIndex(setting.value("type5").toInt());
    ui->comboBox_6->setCurrentIndex(setting.value("type6").toInt());
    ui->comboBox_7->setCurrentIndex(setting.value("type7").toInt());
    ui->comboBox_8->setCurrentIndex(setting.value("type8").toInt());
    ui->comboBox_9->setCurrentIndex(setting.value("type9").toInt());
    ui->comboBox_10->setCurrentIndex(setting.value("type10").toInt());
    ui->comboBox_11->setCurrentIndex(setting.value("type11").toInt());
    ui->comboBox_12->setCurrentIndex(setting.value("type12").toInt());
    ui->comboBox_13->setCurrentIndex(setting.value("type13").toInt());
    ui->comboBox_14->setCurrentIndex(setting.value("type14").toInt());
    ui->comboBox_15->setCurrentIndex(setting.value("type15").toInt());
    ui->comboBox_16->setCurrentIndex(setting.value("type16").toInt());
    QTimer::singleShot(100, this, [=]()
    {
        accept();
    });
}

ChooseTypeDialog::~ChooseTypeDialog()
{
    delete ui;
}

void ChooseTypeDialog::accept()
{
    emit signalStartType(ui->radioButton->isChecked(), ui->comboBox->currentIndex()
                         , ui->comboBox_2->currentIndex(), ui->comboBox_3->currentIndex()
                         , ui->comboBox_4->currentIndex(), ui->comboBox_5->currentIndex()
                         , ui->comboBox_6->currentIndex(), ui->comboBox_7->currentIndex()
                         , ui->comboBox_8->currentIndex(), ui->comboBox_9->currentIndex()
                         , ui->comboBox_10->currentIndex(), ui->comboBox_11->currentIndex()
                         , ui->comboBox_12->currentIndex(), ui->comboBox_13->currentIndex()
                         , ui->comboBox_14->currentIndex(), ui->comboBox_15->currentIndex()
                         , ui->comboBox_16->currentIndex());
}

void ChooseTypeDialog::reject()
{
    emit signalQuit();
}

void ChooseTypeDialog::slotChangeGroup1(int index)
{
    ui->comboBox->setCurrentIndex(index);
    ui->comboBox_2->setCurrentIndex(index);
    ui->comboBox_3->setCurrentIndex(index);
    ui->comboBox_4->setCurrentIndex(index);
}

void ChooseTypeDialog::slotChangeGroup2(int index)
{
    ui->comboBox_5->setCurrentIndex(index);
    ui->comboBox_6->setCurrentIndex(index);
    ui->comboBox_7->setCurrentIndex(index);
    ui->comboBox_8->setCurrentIndex(index);
}

void ChooseTypeDialog::slotChangeGroup3(int index)
{
    ui->comboBox_9->setCurrentIndex(index);
    ui->comboBox_10->setCurrentIndex(index);
    ui->comboBox_11->setCurrentIndex(index);
    ui->comboBox_12->setCurrentIndex(index);
}

void ChooseTypeDialog::slotChangeGroup4(int index)
{
    ui->comboBox_13->setCurrentIndex(index);
    ui->comboBox_14->setCurrentIndex(index);
    ui->comboBox_15->setCurrentIndex(index);
    ui->comboBox_16->setCurrentIndex(index);
}
