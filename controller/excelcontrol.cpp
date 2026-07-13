#include "excelcontrol.h"
#include <QThread>
#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QCoreApplication>
QXLSX_USE_NAMESPACE

ExcelControl::ExcelControl(QObject *parent, int type) : QObject(parent), excelConnector(NULL), m_type(type)
{
    qRegisterMetaType<std::vector<ExcelItem>>("std::vector<ExcelItem>");
    initExcel();
}

ExcelControl::~ExcelControl()
{

}

void ExcelControl::initExcel()
{
    if(excelConnector != NULL)
    {
        delete excelConnector;
        excelConnector = NULL;
    }
    switch(m_type)
    {
    case 0:
    {
        QString path = QCoreApplication::applicationDirPath() + "/platforms/1.xlsx";
        qDebug() << "open excel as " << path;
        excelConnector = new QXlsx::Document(path);
        break;
    }
    case 1:
    {
        QString path = QCoreApplication::applicationDirPath() + "/platforms/2.xlsx";
        qDebug() << "open excel as " << path;
        excelConnector = new  QXlsx::Document(path);
        break;
    }
    case 2:
    {
        QString path = QCoreApplication::applicationDirPath() + "/platforms/3.xlsx";
        qDebug() << "open excel as " << path;
        excelConnector = new  QXlsx::Document(path);
        break;
    }
    case 3:
    {
        QString path = QCoreApplication::applicationDirPath() + "/platforms/4.xlsx";
        qDebug() << "open excel as " << path;
        excelConnector = new  QXlsx::Document(path);
        break;
    }
    case 4:
    {
        QString path = QCoreApplication::applicationDirPath() + "/platforms/5.xlsx";
        qDebug() << "open excel as " << path;
        excelConnector = new  QXlsx::Document(path);
        break;
    }
    case 5:
    {
        QString path = QCoreApplication::applicationDirPath() + "/platforms/6.xlsx";
        qDebug() << "open excel as " << path;
        excelConnector = new  QXlsx::Document(path);
        break;
    }
    case 6:
    {
        QString path = QCoreApplication::applicationDirPath() + "/platforms/7.xlsx";
        qDebug() << "open excel as " << path;
        excelConnector = new  QXlsx::Document(path);
        break;
    }
    case 7:
    {
        QString path = QCoreApplication::applicationDirPath() + "/platforms/8.xlsx";
        qDebug() << "open excel as " << path;
        excelConnector = new  QXlsx::Document(path);
        break;
    }
    case 8:
    {
        QString path = QCoreApplication::applicationDirPath() + "/platforms/9.xlsx";
        qDebug() << "open excel as " << path;
        excelConnector = new  QXlsx::Document(path);
        break;
    }
    case 9:
    {
        QString path = QCoreApplication::applicationDirPath() + "/platforms/10.xlsx";
        qDebug() << "open excel as " << path;
        excelConnector = new  QXlsx::Document(path);
        break;
    }
    case 10:
    {
        QString path = QCoreApplication::applicationDirPath() + "/platforms/11.xlsx";
        qDebug() << "open excel as " << path;
        excelConnector = new  QXlsx::Document(path);
        break;
    }
    case 11:
    {
        QString path = QCoreApplication::applicationDirPath() + "/platforms/12.xlsx";
        qDebug() << "open excel as " << path;
        excelConnector = new  QXlsx::Document(path);
        break;
    }
    case 12:
    {
        QString path = QCoreApplication::applicationDirPath() + "/platforms/13.xlsx";
        qDebug() << "open excel as " << path;
        excelConnector = new  QXlsx::Document(path);
        break;
    }
    default:
        break;
    }
}

void ExcelControl::saveExcel()
{
    QTimer *saveTimer = new QTimer();
    saveTimer->setSingleShot(true);
    connect(saveTimer, SIGNAL(timeout()), this, SLOT(slotSave()));
    saveTimer->start(5000);
}

void ExcelControl::slotStartExcel(const QString &path)
{
    savePath = path;
    saveExcel();
}

void ExcelControl::slotSetExcel(std::vector<ExcelItem> items)
{
    Format successFormat;
    successFormat.setHorizontalAlignment(Format::AlignHCenter);  // 水平居中
    successFormat.setVerticalAlignment(Format::AlignVCenter);    // 垂直居中
    successFormat.setFontSize(10);
    successFormat.setFontBold(true);
    successFormat.setFontColor(QColor(0, 255, 0));
    Format failFormat;
    failFormat.setHorizontalAlignment(Format::AlignHCenter);  // 水平居中
    failFormat.setVerticalAlignment(Format::AlignVCenter);    // 垂直居中
    failFormat.setFontSize(10);
    failFormat.setFontBold(true);
    failFormat.setFontColor(QColor(255, 0, 0));
    Format format;
    format.setHorizontalAlignment(Format::AlignHCenter);  // 水平居中
    format.setVerticalAlignment(Format::AlignVCenter);    // 垂直居中
    format.setFontSize(10);
    successFormat.setBorderColor(QColor("#000000"));
    successFormat.setBorderStyle(QXlsx::Format::BorderThin);
    failFormat.setBorderColor(QColor("#000000"));
    failFormat.setBorderStyle(QXlsx::Format::BorderThin);
    format.setBorderColor(QColor("#000000"));
    format.setBorderStyle(QXlsx::Format::BorderThin);
    for(auto item : items)
    {
        if("通过" == item.word)
        {
            excelConnector->write(item.row, item.col, item.word, successFormat);
        }
        else if("未通过" == item.word || "超时未应答" == item.word)
        {
            ifSuccess = false;
            excelConnector->write(item.row, item.col, item.word, failFormat);
        }
        else
        {
            excelConnector->write(item.row, item.col, item.word, format);
        }
    }
}

void ExcelControl::slotSave()
{
    excelConnector->saveAs(savePath);
    qDebug() << "excel save as " << savePath;
    QThread::msleep(1000);
    initExcel();
}
