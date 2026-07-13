#include "checkkbdialog.h"
#include "ui_checkkbdialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QInputDialog>
#include <QFileDialog>
#include <QTimer>
#include <QDir>
QXLSX_USE_NAMESPACE

CheckKBDialog::CheckKBDialog(QWidget *parent, int type, QString gz, QString op) :
    QDialog(parent),
    ui(new Ui::CheckKBDialog)
{
    ui->setupUi(this);
    m_type = type;
    m_gz = gz;
    m_op = op;
    ui->zdButton->setChecked(true);
    initExcel();
    connect(ui->calButton2, SIGNAL(clicked()), this, SLOT(slotCalKB2()));
    connect(ui->setButton2, SIGNAL(clicked()), this, SLOT(slotSetKB2()));
    connect(ui->calButton3, SIGNAL(clicked()), this, SLOT(slotCalKB3()));
    connect(ui->setButton3, SIGNAL(clicked()), this, SLOT(slotSetKB3()));
    connect(ui->calButton4, SIGNAL(clicked()), this, SLOT(slotCalKB4()));
    connect(ui->setButton4, SIGNAL(clicked()), this, SLOT(slotSetKB4()));
    connect(ui->calButton5, SIGNAL(clicked()), this, SLOT(slotCalKB5()));
    connect(ui->setButton5, SIGNAL(clicked()), this, SLOT(slotSetKB5()));
    connect(ui->calButton6, SIGNAL(clicked()), this, SLOT(slotCalKB6()));
    connect(ui->setButton6, SIGNAL(clicked()), this, SLOT(slotSetKB6()));
    connect(ui->calButton7, SIGNAL(clicked()), this, SLOT(slotCalKB7()));
    connect(ui->setButton7, SIGNAL(clicked()), this, SLOT(slotSetKB7()));
    connect(ui->calButton8, SIGNAL(clicked()), this, SLOT(slotCalKB8()));
    connect(ui->setButton8, SIGNAL(clicked()), this, SLOT(slotSetKB8()));
    connect(ui->calButton9, SIGNAL(clicked()), this, SLOT(slotCalKB9()));
    connect(ui->setButton9, SIGNAL(clicked()), this, SLOT(slotSetKB9()));
    connect(ui->calButton10, SIGNAL(clicked()), this, SLOT(slotCalKB10()));
    connect(ui->setButton10, SIGNAL(clicked()), this, SLOT(slotSetKB10()));
    connect(ui->calButton11, SIGNAL(clicked()), this, SLOT(slotCalKB11()));
    connect(ui->setButton11, SIGNAL(clicked()), this, SLOT(slotSetKB11()));
    connect(ui->calButton12, SIGNAL(clicked()), this, SLOT(slotCalKB12()));
    connect(ui->setButton12, SIGNAL(clicked()), this, SLOT(slotSetKB12()));
    connect(ui->calButton13, SIGNAL(clicked()), this, SLOT(slotCalKB13()));
    connect(ui->setButton13, SIGNAL(clicked()), this, SLOT(slotSetKB13()));
    connect(ui->calButton14, SIGNAL(clicked()), this, SLOT(slotCalKB14()));
    connect(ui->setButton14, SIGNAL(clicked()), this, SLOT(slotSetKB14()));
    connect(ui->calButton15, SIGNAL(clicked()), this, SLOT(slotCalKB15()));
    connect(ui->setButton15, SIGNAL(clicked()), this, SLOT(slotSetKB15()));
    connect(ui->calButton16, SIGNAL(clicked()), this, SLOT(slotCalKB16()));
    connect(ui->setButton16, SIGNAL(clicked()), this, SLOT(slotSetKB16()));
}

CheckKBDialog::~CheckKBDialog()
{
    delete ui;
}

void CheckKBDialog::initExcel()
{
    QString path = QCoreApplication::applicationDirPath() + "/platforms/checkplatform.xlsx";
    qDebug() << "open excel as " << path;
    excelConnector = new QXlsx::Document(path);
}

void CheckKBDialog::slotTypeChange()
{
    ui->read75Edit->clear();
    ui->read0Edit->clear();
    ui->test75Edit->clear();
    ui->test0Edit->clear();
    ui->real1Edit->clear();
    ui->real2Edit->clear();
    ui->newkEdit->clear();
    ui->newbEdit->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        ui->label_3->hide();
        ui->read0Edit->hide();
        ui->read0Edit->hide();
        ui->read0Edit9->hide();
        ui->read0Edit13->hide();
        ui->label_4->hide();
        ui->test0Edit->hide();
        ui->test0Edit2->hide();
        ui->test0Edit3->hide();
        ui->test0Edit4->hide();
        ui->test0Edit5->hide();
        ui->test0Edit6->hide();
        ui->test0Edit7->hide();
        ui->test0Edit8->hide();
        ui->test0Edit9->hide();
        ui->test0Edit10->hide();
        ui->test0Edit11->hide();
        ui->test0Edit12->hide();
        ui->test0Edit13->hide();
        ui->test0Edit14->hide();
        ui->test0Edit15->hide();
        ui->test0Edit16->hide();
        ui->read2Button->hide();
        ui->real2Edit->hide();
        ui->real2Edit->hide();
        ui->real2Edit2->hide();
        ui->real2Edit3->hide();
        ui->real2Edit4->hide();
        ui->real2Edit5->hide();
        ui->real2Edit6->hide();
        ui->real2Edit7->hide();
        ui->real2Edit7->hide();
        ui->real2Edit8->hide();
        ui->real2Edit9->hide();
        ui->real2Edit10->hide();
        ui->real2Edit11->hide();
        ui->real2Edit12->hide();
        ui->real2Edit13->hide();
        ui->real2Edit14->hide();
        ui->real2Edit15->hide();
        ui->real2Edit16->hide();
        ui->label_6->hide();
    }
    else
    {
        ui->label_3->show();
        ui->read0Edit->show();
        ui->read0Edit->show();
        ui->read0Edit9->show();
        ui->read0Edit13->show();
        ui->label_4->show();
        ui->test0Edit->show();
        ui->test0Edit2->show();
        ui->test0Edit3->show();
        ui->test0Edit4->show();
        ui->test0Edit5->show();
        ui->test0Edit6->show();
        ui->test0Edit7->show();
        ui->test0Edit8->show();
        ui->test0Edit9->show();
        ui->test0Edit10->show();
        ui->test0Edit11->show();
        ui->test0Edit12->show();
        ui->test0Edit13->show();
        ui->test0Edit14->show();
        ui->test0Edit15->show();
        ui->test0Edit16->show();
        ui->read2Button->show();
        ui->real2Edit->show();
        ui->real2Edit->show();
        ui->real2Edit2->show();
        ui->real2Edit3->show();
        ui->real2Edit4->show();
        ui->real2Edit5->show();
        ui->real2Edit6->show();
        ui->real2Edit7->show();
        ui->real2Edit7->show();
        ui->real2Edit8->show();
        ui->real2Edit9->show();
        ui->real2Edit10->show();
        ui->real2Edit11->show();
        ui->real2Edit12->show();
        ui->real2Edit13->show();
        ui->real2Edit14->show();
        ui->real2Edit15->show();
        ui->real2Edit16->show();
        ui->label_6->show();
    }
    if(ui->chargeButton->isChecked() || ui->dischargeButton->isChecked())
    {
        ui->get1Button->setText("校准电流1");
        ui->get2Button->setText("校准电流2");
        ui->get3Button->setText("验证电流3");
    }
    else if(ui->zdButton->isChecked() || ui->preChargeButton->isChecked())
    {
        ui->get1Button->setText("校准电压1");
        ui->get2Button->setText("校准电压2");
        ui->get3Button->setText("验证电压3");
    }
}

void CheckKBDialog::slotCalKB()
{
    ui->newkEdit->clear();
    ui->newbEdit->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit->text().isEmpty() ||
           ui->read75Edit->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit->text().toFloat();
            float test1 = ui->read75Edit->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit->text().isEmpty() ||
           ui->test0Edit->text().isEmpty()  ||
           ui->read75Edit->text().isEmpty() ||
           ui->read0Edit->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit->text().toFloat();
            float real2 = ui->test0Edit->text().toFloat();
            float test1 = ui->read75Edit->text().toFloat();
            float test2 = ui->read0Edit->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB2()
{

    ui->newkEdit2->clear();
    ui->newbEdit2->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit2->text().isEmpty() ||
           ui->read75Edit->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit2->text().toFloat();
            float test1 = ui->read75Edit->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit2->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit2->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit2->text().isEmpty() ||
           ui->test0Edit2->text().isEmpty()  ||
           ui->read75Edit->text().isEmpty() ||
           ui->read0Edit->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit2->text().toFloat();
            float real2 = ui->test0Edit2->text().toFloat();
            float test1 = ui->read75Edit->text().toFloat();
            float test2 = ui->read0Edit->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit2->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit2->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB3()
{
    ui->newkEdit3->clear();
    ui->newbEdit3->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit3->text().isEmpty() ||
           ui->read75Edit->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit3->text().toFloat();
            float test1 = ui->read75Edit->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit3->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit3->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit3->text().isEmpty() ||
           ui->test0Edit3->text().isEmpty()  ||
           ui->read75Edit->text().isEmpty() ||
           ui->read0Edit->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit3->text().toFloat();
            float real2 = ui->test0Edit3->text().toFloat();
            float test1 = ui->read75Edit->text().toFloat();
            float test2 = ui->read0Edit->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit3->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit3->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB4()
{
    ui->newkEdit4->clear();
    ui->newbEdit4->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit4->text().isEmpty() ||
           ui->read75Edit->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit4->text().toFloat();
            float test1 = ui->read75Edit->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit4->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit4->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit4->text().isEmpty() ||
           ui->test0Edit4->text().isEmpty()  ||
           ui->read75Edit->text().isEmpty() ||
           ui->read0Edit->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit4->text().toFloat();
            float real2 = ui->test0Edit4->text().toFloat();
            float test1 = ui->read75Edit->text().toFloat();
            float test2 = ui->read0Edit->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit4->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit4->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB5()
{
    ui->newkEdit5->clear();
    ui->newbEdit5->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit5->text().isEmpty() ||
           ui->read75Edit5->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit5->text().toFloat();
            float test1 = ui->read75Edit5->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit5->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit5->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit5->text().isEmpty() ||
           ui->test0Edit5->text().isEmpty()  ||
           ui->read75Edit5->text().isEmpty() ||
           ui->read0Edit5->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit5->text().toFloat();
            float real2 = ui->test0Edit5->text().toFloat();
            float test1 = ui->read75Edit5->text().toFloat();
            float test2 = ui->read0Edit5->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit5->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit5->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB6()
{
    ui->newkEdit6->clear();
    ui->newbEdit6->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit6->text().isEmpty() ||
           ui->read75Edit5->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit6->text().toFloat();
            float test1 = ui->read75Edit5->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit6->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit6->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit6->text().isEmpty() ||
           ui->test0Edit6->text().isEmpty()  ||
           ui->read75Edit5->text().isEmpty() ||
           ui->read0Edit5->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit6->text().toFloat();
            float real2 = ui->test0Edit6->text().toFloat();
            float test1 = ui->read75Edit5->text().toFloat();
            float test2 = ui->read0Edit5->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit6->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit6->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB7()
{
    ui->newkEdit7->clear();
    ui->newbEdit7->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit7->text().isEmpty() ||
           ui->read75Edit5->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit7->text().toFloat();
            float test1 = ui->read75Edit5->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit7->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit7->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit7->text().isEmpty() ||
           ui->test0Edit7->text().isEmpty()  ||
           ui->read75Edit5->text().isEmpty() ||
           ui->read0Edit5->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit7->text().toFloat();
            float real2 = ui->test0Edit7->text().toFloat();
            float test1 = ui->read75Edit5->text().toFloat();
            float test2 = ui->read0Edit5->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit7->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit7->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB8()
{
    ui->newkEdit8->clear();
    ui->newbEdit8->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit8->text().isEmpty() ||
           ui->read75Edit5->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit8->text().toFloat();
            float test1 = ui->read75Edit5->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit8->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit8->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit8->text().isEmpty() ||
           ui->test0Edit8->text().isEmpty()  ||
           ui->read75Edit5->text().isEmpty() ||
           ui->read0Edit5->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit8->text().toFloat();
            float real2 = ui->test0Edit8->text().toFloat();
            float test1 = ui->read75Edit5->text().toFloat();
            float test2 = ui->read0Edit5->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit8->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit8->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB9()
{
    ui->newkEdit9->clear();
    ui->newbEdit9->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit9->text().isEmpty() ||
           ui->read75Edit9->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit9->text().toFloat();
            float test1 = ui->read75Edit9->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit9->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit9->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit9->text().isEmpty() ||
           ui->test0Edit9->text().isEmpty()  ||
           ui->read75Edit9->text().isEmpty() ||
           ui->read0Edit9->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit9->text().toFloat();
            float real2 = ui->test0Edit9->text().toFloat();
            float test1 = ui->read75Edit9->text().toFloat();
            float test2 = ui->read0Edit9->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit9->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit9->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB10()
{
    ui->newkEdit10->clear();
    ui->newbEdit10->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit10->text().isEmpty() ||
           ui->read75Edit9->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit10->text().toFloat();
            float test1 = ui->read75Edit9->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit10->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit10->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit10->text().isEmpty() ||
           ui->test0Edit10->text().isEmpty()  ||
           ui->read75Edit9->text().isEmpty() ||
           ui->read0Edit9->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit10->text().toFloat();
            float real2 = ui->test0Edit10->text().toFloat();
            float test1 = ui->read75Edit9->text().toFloat();
            float test2 = ui->read0Edit9->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit10->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit10->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB11()
{
    ui->newkEdit11->clear();
    ui->newbEdit11->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit11->text().isEmpty() ||
           ui->read75Edit9->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit11->text().toFloat();
            float test1 = ui->read75Edit9->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit11->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit11->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit11->text().isEmpty() ||
           ui->test0Edit11->text().isEmpty()  ||
           ui->read75Edit9->text().isEmpty() ||
           ui->read0Edit9->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit11->text().toFloat();
            float real2 = ui->test0Edit11->text().toFloat();
            float test1 = ui->read75Edit9->text().toFloat();
            float test2 = ui->read0Edit9->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit11->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit11->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB12()
{
    ui->newkEdit12->clear();
    ui->newbEdit12->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit12->text().isEmpty() ||
           ui->read75Edit9->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit12->text().toFloat();
            float test1 = ui->read75Edit9->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit12->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit12->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit12->text().isEmpty() ||
           ui->test0Edit12->text().isEmpty()  ||
           ui->read75Edit9->text().isEmpty() ||
           ui->read0Edit9->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit12->text().toFloat();
            float real2 = ui->test0Edit12->text().toFloat();
            float test1 = ui->read75Edit9->text().toFloat();
            float test2 = ui->read0Edit9->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit12->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit12->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB13()
{
    ui->newkEdit13->clear();
    ui->newbEdit13->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit13->text().isEmpty() ||
           ui->read75Edit13->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit13->text().toFloat();
            float test1 = ui->read75Edit13->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit13->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit13->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit13->text().isEmpty() ||
           ui->test0Edit13->text().isEmpty()  ||
           ui->read75Edit13->text().isEmpty() ||
           ui->read0Edit13->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit13->text().toFloat();
            float real2 = ui->test0Edit13->text().toFloat();
            float test1 = ui->read75Edit13->text().toFloat();
            float test2 = ui->read0Edit13->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit13->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit13->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB14()
{
    ui->newkEdit14->clear();
    ui->newbEdit14->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit14->text().isEmpty() ||
           ui->read75Edit13->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit14->text().toFloat();
            float test1 = ui->read75Edit13->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit14->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit14->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit14->text().isEmpty() ||
           ui->test0Edit14->text().isEmpty()  ||
           ui->read75Edit13->text().isEmpty() ||
           ui->read0Edit13->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit14->text().toFloat();
            float real2 = ui->test0Edit14->text().toFloat();
            float test1 = ui->read75Edit13->text().toFloat();
            float test2 = ui->read0Edit13->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit14->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit14->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB15()
{
    ui->newkEdit15->clear();
    ui->newbEdit15->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit15->text().isEmpty() ||
           ui->read75Edit13->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit15->text().toFloat();
            float test1 = ui->read75Edit13->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit15->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit15->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit15->text().isEmpty() ||
           ui->test0Edit15->text().isEmpty()  ||
           ui->read75Edit13->text().isEmpty() ||
           ui->read0Edit13->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit15->text().toFloat();
            float real2 = ui->test0Edit15->text().toFloat();
            float test1 = ui->read75Edit13->text().toFloat();
            float test2 = ui->read0Edit13->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit15->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit15->setText(QString::number(m_b, 'f', 3));
        }
    }
}

void CheckKBDialog::slotCalKB16()
{
    ui->newkEdit16->clear();
    ui->newbEdit16->clear();
    if(ui->V5Button->isChecked() || ui->V12Button->isChecked() || ui->supplyButton->isChecked())
    {
        if(ui->test75Edit16->text().isEmpty() ||
           ui->read75Edit13->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit16->text().toFloat();
            float test1 = ui->read75Edit13->text().toFloat();

            if(std::abs(real1 - test1) > 20)
            {
                QMessageBox::information(this, "提示", "B值超出范围,请重新读取各参数", QMessageBox::Ok);
            }
            m_k = 1;
            m_b = test1 - m_k * real1;
            ui->newkEdit16->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit16->setText(QString::number(m_b, 'f', 3));
        }
    }
    else
    {
        if(ui->test75Edit16->text().isEmpty() ||
           ui->test0Edit16->text().isEmpty()  ||
           ui->read75Edit13->text().isEmpty() ||
           ui->read0Edit13->text().isEmpty())
        {
            QMessageBox::information(this, "提示", "请先获取完数据后再进行计算", QMessageBox::Ok);
        }
        else
        {
            float real1 = ui->test75Edit16->text().toFloat();
            float real2 = ui->test0Edit16->text().toFloat();
            float test1 = ui->read75Edit13->text().toFloat();
            float test2 = ui->read0Edit13->text().toFloat();
            m_k = (test1 - test2) / (real1 - real2);
            m_b = test2 - m_k * real2;
            ui->newkEdit16->setText(QString::number(m_k, 'f', 3));
            ui->newbEdit16->setText(QString::number(m_b, 'f', 3));
        }
    }
}


void CheckKBDialog::slotSetKB()
{
    float k = ui->newkEdit->text().toFloat();
    float b = ui->newbEdit->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 1);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 1);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 1);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 1);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 1);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 1);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 1);
    }
}

void CheckKBDialog::slotSetKB2()
{
    float k = ui->newkEdit2->text().toFloat();
    float b = ui->newbEdit2->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 2);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 2);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 2);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 2);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 2);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 2);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 2);
    }
}

void CheckKBDialog::slotSetKB3()
{
    float k = ui->newkEdit3->text().toFloat();
    float b = ui->newbEdit3->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 3);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 3);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 3);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 3);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 3);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 3);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 3);
    }
}

void CheckKBDialog::slotSetKB4()
{
    float k = ui->newkEdit4->text().toFloat();
    float b = ui->newbEdit4->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 4);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 4);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 4);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 4);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 4);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 4);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 4);
    }
}

void CheckKBDialog::slotSetKB5()
{
    float k = ui->newkEdit5->text().toFloat();
    float b = ui->newbEdit5->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 5);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 5);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 5);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 5);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 5);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 5);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 5);
    }
}

void CheckKBDialog::slotSetKB6()
{
    float k = ui->newkEdit6->text().toFloat();
    float b = ui->newbEdit6->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 6);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 6);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 6);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 6);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 6);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 6);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 6);
    }
}

void CheckKBDialog::slotSetKB7()
{
    float k = ui->newkEdit7->text().toFloat();
    float b = ui->newbEdit7->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 7);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 7);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 7);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 7);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 7);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 7);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 7);
    }
}

void CheckKBDialog::slotSetKB8()
{
    float k = ui->newkEdit8->text().toFloat();
    float b = ui->newbEdit8->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 8);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 8);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 8);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 8);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 8);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 8);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 8);
    }
}

void CheckKBDialog::slotSetKB9()
{
    float k = ui->newkEdit9->text().toFloat();
    float b = ui->newbEdit9->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 9);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 9);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 9);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 9);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 9);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 9);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 9);
    }
}

void CheckKBDialog::slotSetKB10()
{
    float k = ui->newkEdit10->text().toFloat();
    float b = ui->newbEdit10->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 10);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 10);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 10);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 10);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 10);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 10);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 10);
    }
}

void CheckKBDialog::slotSetKB11()
{
    float k = ui->newkEdit11->text().toFloat();
    float b = ui->newbEdit11->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 11);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 11);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 11);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 11);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 11);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 11);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 11);
    }
}

void CheckKBDialog::slotSetKB12()
{
    float k = ui->newkEdit12->text().toFloat();
    float b = ui->newbEdit12->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 12);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 12);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 12);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 12);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 12);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 12);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 12);
    }
}

void CheckKBDialog::slotSetKB13()
{
    float k = ui->newkEdit13->text().toFloat();
    float b = ui->newbEdit13->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 13);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 13);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 13);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 13);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 13);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 13);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 13);
    }
}

void CheckKBDialog::slotSetKB14()
{
    float k = ui->newkEdit14->text().toFloat();
    float b = ui->newbEdit14->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 14);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 14);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 14);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 14);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 14);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 14);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 14);
    }
}

void CheckKBDialog::slotSetKB15()
{
    float k = ui->newkEdit15->text().toFloat();
    float b = ui->newbEdit15->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 15);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 15);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 15);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 15);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 15);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 15);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 15);
    }
}

void CheckKBDialog::slotSetKB16()
{
    float k = ui->newkEdit16->text().toFloat();
    float b = ui->newbEdit16->text().toFloat();
    if(ui->chargeButton->isChecked())
    {
        emit signalSetKB(1, k, b, 16);
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalSetKB(2, k, b, 16);
    }
    else if(ui->zdButton->isChecked())
    {
        emit signalSetKB(3, k, b, 16);
    }
    else if(ui->V5Button->isChecked())
    {
        emit signalSetKB(4, k, b, 16);
    }
    else if(ui->V12Button->isChecked())
    {
        emit signalSetKB(5, k, b, 16);
    }
    else if(ui->supplyButton->isChecked())
    {
        emit signalSetKB(6, k, b, 16);
    }
    else if(ui->preChargeButton->isChecked())
    {
        emit signalSetKB(7, k, b, 16);
    }
}

void CheckKBDialog::slotGetKB(float k, float b)
{
}

void CheckKBDialog::slotReadInfo1()
{
    if(ui->chargeButton->isChecked())
    {
        int *index = new int(1);  // 使用堆分配，配合智能删除
        m_currentRead = 1;
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, timer, index]()
        {
            if (*index >= 17)
            {
                timer->stop();
                timer->deleteLater();
                delete index;
                return;
            }

            emit signalReadInfo(m_currentRead, *index);
            (*index)++;
        });
        emit signalReadInfo(m_currentRead, *index);
        (*index)++;
        timer->start(100);
    }
    else if(ui->dischargeButton->isChecked())
    {
        int *index = new int(1);  // 使用堆分配，配合智能删除
        m_currentRead = 2;
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, timer, index]()
        {
            if (*index >= 17)
            {
                timer->stop();
                timer->deleteLater();
                delete index;
                return;
            }

            emit signalReadInfo(m_currentRead, *index);
            (*index)++;
        });
        emit signalReadInfo(m_currentRead, *index);
        (*index)++;
        timer->start(100);
    }
    else if(ui->zdButton->isChecked())
    {
        int *index = new int(1);  // 使用堆分配，配合智能删除
        m_currentRead = 3;
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, timer, index]()
        {
            if (*index >= 17)
            {
                timer->stop();
                timer->deleteLater();
                delete index;
                return;
            }

            emit signalReadInfo(m_currentRead, *index);
            (*index)++;
        });
        emit signalReadInfo(m_currentRead, *index);
        (*index)++;
        timer->start(100);
    }
    else if(ui->V5Button->isChecked())
    {
        int *index = new int(1);  // 使用堆分配，配合智能删除
        m_currentRead = 7;
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, timer, index]()
        {
            if (*index >= 17) {
                timer->stop();
                timer->deleteLater();
                delete index;
                return;
            }

            emit signalReadInfo(m_currentRead, *index);
            (*index)++;
        });
        emit signalReadInfo(m_currentRead, *index);
        (*index)++;
        timer->start(100);
    }
    else if(ui->V12Button->isChecked())
    {
        int *index = new int(1);  // 使用堆分配，配合智能删除
        m_currentRead = 8;
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, timer, index]()
        {
            if (*index >= 17) {
                timer->stop();
                timer->deleteLater();
                delete index;
                return;
            }

            emit signalReadInfo(m_currentRead, *index);
            (*index)++;
        });
        emit signalReadInfo(m_currentRead, *index);
        (*index)++;
        timer->start(100);
    }
    else if(ui->supplyButton->isChecked())
    {
        int *index = new int(1);  // 使用堆分配，配合智能删除
        m_currentRead = 9;
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, timer, index]()
        {
            if (*index >= 17) {
                timer->stop();
                timer->deleteLater();
                delete index;
                return;
            }

            emit signalReadInfo(m_currentRead, *index);
            (*index)++;
        });
        emit signalReadInfo(m_currentRead, *index);
        (*index)++;
        timer->start(100);
    }
    else if(ui->preChargeButton->isChecked())
    {
        int *index = new int(1);  // 使用堆分配，配合智能删除
        m_currentRead = 10;
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, timer, index]()
        {
            if (*index >= 17)
            {
                timer->stop();
                timer->deleteLater();
                delete index;
                return;
            }

            emit signalReadInfo(m_currentRead, *index);
            (*index)++;
        });
        emit signalReadInfo(m_currentRead, *index);
        (*index)++;
        timer->start(100);
    }
}

void CheckKBDialog::slotReadInfo2()
{
    if(ui->chargeButton->isChecked())
    {
        int *index = new int(1);  // 使用堆分配，配合智能删除
        m_currentRead = 4;
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, timer, index]() {
            if (*index >= 17) {
                timer->stop();
                timer->deleteLater();
                delete index;
                return;
            }

            emit signalReadInfo(m_currentRead, *index);
            (*index)++;
        });
        emit signalReadInfo(m_currentRead, *index);
        (*index)++;
        timer->start(100);
    }
    else if(ui->dischargeButton->isChecked())
    {
        int *index = new int(1);  // 使用堆分配，配合智能删除
        m_currentRead = 5;
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, timer, index]() {
            if (*index >= 17) {
                timer->stop();
                timer->deleteLater();
                delete index;
                return;
            }

            emit signalReadInfo(m_currentRead, *index);
            (*index)++;
        });
        emit signalReadInfo(m_currentRead, *index);
        (*index)++;
        timer->start(100);
    }
    else if(ui->zdButton->isChecked())
    {
        int *index = new int(1);  // 使用堆分配，配合智能删除
        m_currentRead = 6;
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, timer, index]() {
            if (*index >= 17) {
                timer->stop();
                timer->deleteLater();
                delete index;
                return;
            }

            emit signalReadInfo(m_currentRead, *index);
            (*index)++;
        });
        emit signalReadInfo(m_currentRead, *index);
        (*index)++;
        timer->start(100);
    }
    else if(ui->preChargeButton->isChecked())
    {
        int *index = new int(1);  // 使用堆分配，配合智能删除
        m_currentRead = 11;
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, timer, index]() {
            if (*index >= 17) {
                timer->stop();
                timer->deleteLater();
                delete index;
                return;
            }

            emit signalReadInfo(m_currentRead, *index);
            (*index)++;
        });
        emit signalReadInfo(m_currentRead, *index);
        (*index)++;
        timer->start(100);
    }
}

void CheckKBDialog::slotGetInfo(int type, float a, int locate)
{
    switch(type)
    {
    case 1:
    case 2:
    case 3:
    case 7:
    case 8:
    case 9:
    case 10:
    {
        if(locate > 1)
        {
            QString name = "test75Edit" + QString::number(locate);
            QLineEdit* edit = this->findChild<QLineEdit*>(name);
            edit->setText(QString::number(a, 'f', 1));
            if(locate == 16)
            {
                QTimer::singleShot(1000, [this, type]()
                {int *index = new int(1);  // 使用堆分配，配合智能删除
                    m_currentRead = type + 100;
                    QTimer *timer = new QTimer(this);
                    connect(timer, &QTimer::timeout, this, [this, timer, index]()
                    {
                        if (*index >= 17)
                        {
                            timer->stop();
                            timer->deleteLater();
                            delete index;
                            return;
                        }
                        emit signalReadInfo(m_currentRead, *index);
                        (*index)++;
                    });
                    emit signalReadInfo(m_currentRead, *index);
                    (*index)++;
                    timer->start(100);});
            }
        }
        else
        {
            ui->test75Edit->setText(QString::number(a, 'f', 1));
        }
        break;
    }
    case 4:
    case 5:
    case 6:
    case 11:
    {
        if(locate > 1)
        {
            QString name = "test0Edit" + QString::number(locate);
            QLineEdit* edit = this->findChild<QLineEdit*>(name);
            edit->setText(QString::number(a, 'f', 1));
            if(locate == 16)
            {
                QTimer::singleShot(1000, [this, type]()
                {int *index = new int(1);  // 使用堆分配，配合智能删除
                    m_currentRead = type + 100;
                    QTimer *timer = new QTimer(this);
                    connect(timer, &QTimer::timeout, this, [this, timer, index]()
                    {
                        if (*index >= 17)
                        {
                            timer->stop();
                            timer->deleteLater();
                            delete index;
                            return;
                        }
                        emit signalReadInfo(m_currentRead, *index);
                        (*index)++;
                    });
                    emit signalReadInfo(m_currentRead, *index);
                    (*index)++;
                    timer->start(100);});
            }
        }
        else
        {
            ui->test0Edit->setText(QString::number(a, 'f', 1));
        }
        break;
    }
    case 101:
    case 102:
    case 103:
    case 107:
    case 108:
    case 109:
    case 110:
    {
        if(locate > 1)
        {
            QString name = "real1Edit" + QString::number(locate);
            QLineEdit* edit = this->findChild<QLineEdit*>(name);
            edit->setText(QString::number(a, 'f', 1));
        }
        else
        {
            ui->real1Edit->setText(QString::number(a, 'f', 1));
        }
        break;
    }
    case 104:
    case 105:
    case 106:
    case 111:
    {
        if(locate > 1)
        {
            QString name = "real2Edit" + QString::number(locate);
            QLineEdit* edit = this->findChild<QLineEdit*>(name);
            edit->setText(QString::number(a, 'f', 1));
        }
        else
        {
            ui->real2Edit->setText(QString::number(a, 'f', 1));
        }
        break;
    }
    case 99:
    {
        QString path = QCoreApplication::applicationDirPath() + "/check_results";
        Format format;
        format.setHorizontalAlignment(Format::AlignHCenter);  // 水平居中
        format.setVerticalAlignment(Format::AlignVCenter);    // 垂直居中
        format.setFontSize(10);
        format.setBorderColor(QColor("#000000"));
        format.setBorderStyle(QXlsx::Format::BorderThin);
        bool ok;
        double value = QInputDialog::getDouble(nullptr, tr("供电电压保存"), tr("请输入供电电压实测值:"), 24.0, -10000.0, 10000.0, 1, &ok);
        if (ok)
        {
            excelConnector->write(17, 2, QString::number(value), format);
        }
        excelConnector->write(17, 3, QString::number(a / 10.0, 'f', 1), format);
        excelConnector->write(17, 4, QString::number(value - a / 10.0, 'f', 1), format);
        excelConnector->write(21, 2, "通过", format);
        switch(m_type)
        {
        case 0:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L HV5.0.1          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            excelConnector->write(21, 4, "板卡初调", format);
            path = path + "/F133-L-chu_tiao-HV5.0.1/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            qDebug() << "save as " << path;
            break;
        }
        case 1:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L HV5.0.1          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-zhi_jian-HV5.0.1/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "质检", format);
            qDebug() << "save as " << path;
            break;
        }
        case 2:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-S HV5.0.1          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-S HV5.0.1          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            excelConnector->write(21, 4, "板卡初调", format);
            path = path + "/F133-L-S-chu_tiao-HV5.0.1/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            qDebug() << "save as " << path;
            break;
        }
        case 3:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-S HV5.0.1          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-S-zhi_jian-HV5.0.1/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "质检", format);
            qDebug() << "save as " << path;
            break;
        }
        case 4:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L HV5.0.1          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-zheng_ji-HV5.0.1/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "整机测试", format);
            qDebug() << "save as " << path;
            break;
        }
        case 5:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-S HV5.0.1          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-S-zheng_ji-HV5.0.1/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "整机测试", format);
            qDebug() << "save as " << path;
            break;
        }
        case 6:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-ZC HV5.0.2          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-ZC-chu_tiao-HV5.0.2/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "板卡初调", format);
            qDebug() << "save as " << path;
            break;
        }
        case 7:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-ZC HV5.0.2          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-ZC-zheng_ji-HV5.0.2/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "整机测试", format);
            qDebug() << "save as " << path;
            break;
        }
        case 8:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-ZC HV5.0.2          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-ZC-zhi_jian-HV5.0.2/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "质检", format);
            qDebug() << "save as " << path;
            break;
        }
        case 9:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L HV4.0.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-chu_tiao-HV4.0.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "板卡初调", format);
            qDebug() << "save as " << path;
            break;
        }
        case 10:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L HV4.0.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-zheng_ji-HV4.0.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "整机测试", format);
            qDebug() << "save as " << path;
            break;
        }
        case 11:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L HV4.0.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-zhi_jian-HV4.0.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "质检", format);
            qDebug() << "save as " << path;
            break;
        }
        case 12:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-S HV6.0.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-S-chu_tiao-HV6.0.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "板卡初调", format);
            qDebug() << "save as " << path;
            break;
        }
        case 13:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-S HV6.0.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-S-zheng_ji-HV6.0.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "整机测试", format);
            qDebug() << "save as " << path;
            break;
        }
        case 14:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-S HV6.0.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-S-zhi_jian-HV6.0.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "质检", format);
            qDebug() << "save as " << path;
            break;
        }
        case 15:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：A133 HV5.0.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/A133-chu_tiao-HV5.0.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "板卡初调", format);
            qDebug() << "save as " << path;
            break;
        }
        case 16:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：A133 HV5.0.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/A133-zheng_ji-HV5.0.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "整机测试", format);
            qDebug() << "save as " << path;
            break;
        }
        case 17:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：A133 HV5.0.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/A133-zhi_jian-HV5.0.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "质检", format);
            qDebug() << "save as " << path;
            break;
        }
        case 18:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-F-S HV6.1.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-F-S-chu_tiao-HV6.1.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "板卡初调", format);
            qDebug() << "save as " << path;
            break;
        }
        case 19:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-F-S HV6.1.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-F-S-zheng_ji-HV6.1.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "整机测试", format);
            qDebug() << "save as " << path;
            break;
        }
        case 20:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-F-S HV6.1.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-F-S-zhi_jian-HV6.1.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "质检", format);
            qDebug() << "save as " << path;
            break;
        }
        case 21:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：H133-L-S HV6.2.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/H133-L-S-chu_tiao-HV6.2.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "板卡初调", format);
            qDebug() << "save as " << path;
            break;
        }
        case 22:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：H133-L-S HV6.2.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/H133-L-S-zheng_ji-HV6.2.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "整机测试", format);
            qDebug() << "save as " << path;
            break;
        }
        case 23:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：H133-L-S HV6.2.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/H133-L-S-zhi_jian-HV6.2.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "质检", format);
            qDebug() << "save as " << path;
            break;
        }
        case 24:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-S-XY1 HV5.0.1          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-S-chu_tiao-XY1-HV5.0.1/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "板卡初调", format);
            qDebug() << "save as " << path;
            break;
        }
        case 25:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-S-XY1 HV5.0.1          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-S-zheng_ji-XY1-HV5.0.1/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "整机测试", format);
            qDebug() << "save as " << path;
            break;
        }
        case 26:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-S-XY1 HV5.0.1          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-S-zhi_jian-XY1-HV5.0.1/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "质检", format);
            qDebug() << "save as " << path;
            break;
        }
        case 27:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-S HV7.0.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-S-chu_tiao-HV7.0.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "板卡初调", format);
            qDebug() << "save as " << path;
            break;
        }
        case 28:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-S HV7.0.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-S-zheng_ji-HV7.0.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "整机测试", format);
            qDebug() << "save as " << path;
            break;
        }
        case 29:
        {
            excelConnector->write(2, 1, "工装编号 " + m_gz + "    测试产品：F133-L-S HV7.0.0          测试员："
                                  + m_op + "          日期："
                                  + QDateTime::currentDateTime().toString("yyyy-MM-dd"), format);
            path = path + "/F133-L-S-zhi_jian-HV7.0.0/" + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
            excelConnector->write(21, 4, "质检", format);
            qDebug() << "save as " << path;
            break;
        }
        default:
            break;
        }
        if(!ui->locateEdit->text().isNull())
        {
            path = ui->locateEdit->text() + m_gz + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".xlsx";
        }
        excelConnector->saveAs(path);
        delete excelConnector;
        excelConnector = NULL;
        initExcel();
        break;
    }
    default:
        break;
    }
}

void CheckKBDialog::slotSaveData()
{
    Format format;
    format.setHorizontalAlignment(Format::AlignHCenter);  // 水平居中
    format.setVerticalAlignment(Format::AlignVCenter);    // 垂直居中
    format.setFontSize(10);
    format.setBorderColor(QColor("#000000"));
    format.setBorderStyle(QXlsx::Format::BorderThin);
    if(ui->chargeButton->isChecked())
    {
        excelConnector->write(11, 2, ui->read75Edit->text(), format);
        excelConnector->write(11, 3, ui->test75Edit->text(), format);
        excelConnector->write(11, 4, QString::number(ui->test75Edit->text().toFloat() - ui->read75Edit->text().toFloat()), format);
        excelConnector->write(12, 2, ui->read0Edit->text(), format);
        excelConnector->write(12, 3, ui->test0Edit->text(), format);
        excelConnector->write(12, 4, QString::number(ui->test0Edit->text().toFloat() - ui->read0Edit->text().toFloat()), format);
    }
    else if(ui->dischargeButton->isChecked())
    {
        excelConnector->write(13, 2, ui->read75Edit->text(), format);
        excelConnector->write(13, 3, ui->test75Edit->text(), format);
        excelConnector->write(13, 4, QString::number(ui->test75Edit->text().toFloat() - ui->read75Edit->text().toFloat()), format);
        excelConnector->write(14, 2, ui->read0Edit->text(), format);
        excelConnector->write(14, 3, ui->test0Edit->text(), format);
        excelConnector->write(14, 4, QString::number(ui->test0Edit->text().toFloat() - ui->read0Edit->text().toFloat()), format);
    }
    else if(ui->zdButton->isChecked())
    {
        excelConnector->write(5, 2, ui->read75Edit->text(), format);
        excelConnector->write(5, 3, ui->test75Edit->text(), format);
        excelConnector->write(5, 4, QString::number(ui->test75Edit->text().toFloat() - ui->read75Edit->text().toFloat()), format);
        excelConnector->write(6, 2, ui->read0Edit->text(), format);
        excelConnector->write(6, 3, ui->test0Edit->text(), format);
        excelConnector->write(6, 4, QString::number(ui->test0Edit->text().toFloat() - ui->read0Edit->text().toFloat()), format);
    }
}

void CheckKBDialog::slotSaveExcel()
{

}

void CheckKBDialog::slotSetDIr()
{
    QString m_dir = QFileDialog::getExistingDirectory(this, tr("打开文件夹"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->locateEdit->setText(m_dir);
}

void CheckKBDialog::slotChange1()
{
    if(ui->chargeButton->isChecked())
    {
        emit signalChange(1, 1);
        QTimer::singleShot(1000, [this]() {emit signalChange(1, 5);});
        QTimer::singleShot(2000, [this]() {emit signalChange(1, 9);});
        QTimer::singleShot(3000, [this]() {emit signalChange(1, 13);});
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalChange(2, 1);
        QTimer::singleShot(1000, [this]() {emit signalChange(2, 5);});
        QTimer::singleShot(2000, [this]() {emit signalChange(2, 9);});
        QTimer::singleShot(3000, [this]() {emit signalChange(2, 13);});
    }
    else if(ui->zdButton->isChecked() || ui->preChargeButton->isChecked())
    {
        emit signalChange(3, 1);
        QTimer::singleShot(1000, [this]() {emit signalChange(3, 5);});
        QTimer::singleShot(2000, [this]() {emit signalChange(3, 9);});
        QTimer::singleShot(3000, [this]() {emit signalChange(3, 13);});
    }
}

void CheckKBDialog::slotChange2()
{
    if(ui->chargeButton->isChecked())
    {
        emit signalChange(4, 1);
        QTimer::singleShot(1000, [this]() {emit signalChange(4, 5);});
        QTimer::singleShot(2000, [this]() {emit signalChange(4, 9);});
        QTimer::singleShot(3000, [this]() {emit signalChange(4, 13);});
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalChange(5, 1);
        QTimer::singleShot(1000, [this]() {emit signalChange(5, 5);});
        QTimer::singleShot(2000, [this]() {emit signalChange(5, 9);});
        QTimer::singleShot(3000, [this]() {emit signalChange(5, 13);});
    }
    else if(ui->zdButton->isChecked() || ui->preChargeButton->isChecked())
    {
        emit signalChange(6, 1);
        QTimer::singleShot(1000, [this]() {emit signalChange(6, 5);});
        QTimer::singleShot(2000, [this]() {emit signalChange(6, 9);});
        QTimer::singleShot(3000, [this]() {emit signalChange(6, 13);});
    }
}

void CheckKBDialog::slotChange3()
{
    if(ui->chargeButton->isChecked())
    {
        emit signalChange(7, 1);
        QTimer::singleShot(1000, [this]() {emit signalChange(7, 5);});
        QTimer::singleShot(2000, [this]() {emit signalChange(7, 9);});
        QTimer::singleShot(3000, [this]() {emit signalChange(7, 13);});
    }
    else if(ui->dischargeButton->isChecked())
    {
        emit signalChange(8, 1);
        QTimer::singleShot(1000, [this]() {emit signalChange(8, 5);});
        QTimer::singleShot(2000, [this]() {emit signalChange(8, 9);});
        QTimer::singleShot(3000, [this]() {emit signalChange(8, 13);});
    }
    else if(ui->zdButton->isChecked() || ui->preChargeButton->isChecked())
    {
        emit signalChange(9, 1);
        QTimer::singleShot(1000, [this]() {emit signalChange(9, 5);});
        QTimer::singleShot(2000, [this]() {emit signalChange(9, 9);});
        QTimer::singleShot(3000, [this]() {emit signalChange(9, 13);});
    }
}

void CheckKBDialog::slotOpenDO()
{
    int *index = new int(1);  // 使用堆分配，配合智能删除
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, timer, index]() {
        if (*index >= 17) {
            timer->stop();
            timer->deleteLater();
            delete index;
            return;
        }

        emit signalOpenDO(*index);
        (*index)++;
    });
    emit signalOpenDO(*index);
    (*index)++;
    timer->start(100);
}
