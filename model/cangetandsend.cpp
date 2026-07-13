#include "cangetandsend.h"
#include "ping.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QSettings>
#include <QString>
#include <QThread>
#include <QTimer>
#include <string>
#include <strstream>
#include <vector>
#include <cmath>
#include <QCollator>

bool isNum(QString data)
{
    bool ok;
    data.toInt(&ok, 10);
    return ok;
}

CANGetAndSend::CANGetAndSend(int type, int locate)
{
    m_type = type;
    m_locate = locate;
    setFailTimer = new QTimer(this);
    connect(setFailTimer, SIGNAL(timeout()), this, SLOT(slotSetFail()));
    setFailTimer->setSingleShot(true);
    failTimer = new QTimer(this);
    failTimer->setSingleShot(true);
    connect(failTimer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    retryTimer = new QTimer(this);
    connect(retryTimer, SIGNAL(timeout()), this, SLOT(slotRetrySend()));
    retryTimer->setSingleShot(false);
    connect(this, SIGNAL(startTimerSignal(int)), this, SLOT(startTimerSlot(int)));
    connect(this, SIGNAL(stopTimerSignal()), this, SLOT(stopTimerSlot()));
    connect(this, SIGNAL(startFailTimerSignal(int)), this, SLOT(startFailTimerSlot(int)));
    connect(this, SIGNAL(stopFailTimerSignal()), this, SLOT(stopFailTimerSlot()));
    connect(&timeoutTimer, SIGNAL(timeout()), this, SLOT(slotTimeOut()));
    timeoutTimer.setSingleShot(true);
    ping       = new Ping(this);
    QSettings setting(QDir::currentPath() + "/version.ini", QSettings::IniFormat);
    voltageTime = setting.value("voltageTime").toInt();
    if(voltageTime == 0)
    {
        voltageTime = 10000;
    }
    currentTime = setting.value("currentTime").toInt();
    if(currentTime == 0)
    {
        currentTime = 5000;
    }
    maxReadTime = setting.value("maxReadTime").toInt();
    if(maxReadTime == 0)
    {
        maxReadTime = 5;
    }
}

void CANGetAndSend::updateTestItem()
{
    qDebug() << "locate at " << m_locate << " step is " << step << " testitem is " << lastTestType;
    if(!timeoutTimer.isActive())
    {
        if(lastTestType == TestSetBatteryVoltage)
        {
            timeoutTimer.start(60000);
        }
        else
        {
            timeoutTimer.start(30000);
        }
    }
    switch (lastTestType)
    {
    case StartTest:
    {
        openDO7();
        step = 0;
        break;
    }
    case TestSetBatteryVoltage:
    {
        if (0 == step)
        {
            ifCanPass    = true;
            ifVPass      = true;
            ifRPass      = true;
            ifCPass      = true;
            ifSetSuccess = true;
            hallTime     = 0;
            canTime      = 0;
            for (auto key : testResults.keys())
            {
                testResults.remove(key);
            }
            for(auto key : ZDResistance.keys())
            {
                ZDResistance.remove(key);
            }
            for (auto key : HALLSignal.keys())
            {
                HALLSignal.remove(key);
            }
            for (auto key : HALLCurrent.keys())
            {
                HALLCurrent.remove(key);
            }
            for (auto key : SystemTemperature.keys())
            {
                SystemTemperature.remove(key);
            }
            for (auto key : DOTest.keys())
            {
                DOTest.remove(key);
            }
            for (auto key : DITest.keys())
            {
                DITest.remove(key);
            }
            for (auto key : CANCommunication.keys())
            {
                CANCommunication.remove(key);
            }
            for (auto key : Temperature.keys())
            {
                Temperature.remove(key);
            }
            openDO7();
            items.clear();
            qDebug()  << " step at " << step << " start send batteryVoltage";
            ifVPass = true;
            m_gzvz1 = 0.0;
            m_gzvz2 = 0.0;
            m_gzvz3 = 0.0;
            m_gzvy1 = 0.0;
            m_gzvy2 = 0.0;
            m_gzvy3 = 0.0;
            m_dcvz1 = 0.0;
            m_dcvz2 = 0.0;
            m_dcvz3 = 0.0;
            m_dcvy1 = 0.0;
            m_dcvy2 = 0.0;
            m_dcvy3 = 0.0;
            QTimer *tempTimer = new QTimer(this);
            connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSetMod()));
            tempTimer->setSingleShot(true);

            QSettings setting(QDir::currentPath() + "/version.ini",
                              QSettings::IniFormat);
            int time  = setting.value("powerupTime").toInt();
            tempTimer->start(time);
            step = 1;
        }
        else if (1 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板组端电压读取中。。。");
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            emit startTimerSignal(1000);
            step = 2;
        }
        else if (2 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板组端电压读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            step = 3;
        }
        else if (3 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板组端电压读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            step = 4;
        }
        else if (4 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板预充电压读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            step = 5;
        }
        else if (5 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            QByteArray temp;
            switch(m_locate)
            {
            case 3:
            {
                temp = QByteArray("\x01\x10\x10\x00\x00\x01\x02\x05\x78", 9);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 7:
            {
                temp = QByteArray("\x05\x10\x10\x00\x00\x01\x02\x05\x78", 9);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                            break;
            }
            case 11:
            {
                temp = QByteArray("\x09\x10\x10\x00\x00\x01\x02\x05\x78", 9);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 15:
            {
                temp = QByteArray("\x0D\x10\x10\x00\x00\x01\x02\x05\x78", 9);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            default:
                break;
            }
            emit signalReceiveStatus("高压源设置中。。。");
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            step = 6;
            QTimer *tempTimer = new QTimer(this);
            connect(tempTimer, SIGNAL(timeout()), this,
                    SLOT(slotCheckIfUpdate()));
            tempTimer->setSingleShot(true);
            tempTimer->start(voltageTime);
        }
        else if (6 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板组端电压读取中。。。");
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            emit startTimerSignal(1000);
            step = 7;
        }
        else if (7 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板组端电压读取中。。。");
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            emit startTimerSignal(1000);
            step = 8;
        }
        else if (8 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板组端电压读取中。。。");
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            emit startTimerSignal(1000);
            step = 9;
        }
        else if (9 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板预充电压读取中。。。");
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            emit startTimerSignal(1000);
            step = 10;
        }
        else if (10 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            float k = (m_gzvy1 - m_gzvy2) / (m_dcvy1 - m_dcvy2);
            float b = m_gzvy1 - k * m_dcvy1;
            unsigned char data[8];
            data[0] = 0x0D;
            data[1] = 2;
            data[2]  = int(k * 1000) >> 8;
            data[3]  = int(k * 1000);
            int temp = b * 1000;
            if (b > 0)
            {
                data[4] = temp >> 8;
                data[5] = temp;
            }
            else if (b < 0)
            {
                data[4] = (65535 + temp) >> 8;
                data[5] = (65535 + temp);
            }
            else
            {
                data[4] = 0x00;
                data[5] = 0x00;
            }
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            k = (m_gzvz1 - m_gzvz2) / (m_dcvz1 - m_dcvz2);
            b = m_gzvz1 - k * m_dcvz1;
            data[0] = 0x0D;
            data[1] = 1;
            data[2]  = int(k * 1000) >> 8;
            data[3]  = int(k * 1000);
            temp = b * 1000;
            if (b > 0)
            {
                data[4] = temp >> 8;
                data[5] = temp;
            }
            else if (b < 0)
            {
                data[4] = (65535 + temp) >> 8;
                data[5] = (65535 + temp);
            }
            else
            {
                data[4] = 0x00;
                data[5] = 0x00;
            }
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("KB值下设中。。。");
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            //failTimer->start(voltageTime);
            step = 11;
            QTimer *tempTimer = new QTimer(this);
            connect(tempTimer, SIGNAL(timeout()), this,
                    SLOT(slotCheckIfUpdate()));
            tempTimer->setSingleShot(true);
            tempTimer->start(1000);
        }
        else if (11 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            QByteArray temp;
            switch(m_locate)
            {
            case 3:
            {
                temp = QByteArray("\x01\x10\x10\x00\x00\x01\x02\x03\xE8", 9);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 7:
            {
                temp = QByteArray("\x05\x10\x10\x00\x00\x01\x02\x03\xE8", 9);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 11:
            {
                temp = QByteArray("\x09\x10\x10\x00\x00\x01\x02\x03\xE8", 9);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 15:
            {
                temp = QByteArray("\x0D\x10\x10\x00\x00\x01\x02\x03\xE8", 9);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            default:
                break;
            }
            emit signalReceiveStatus("高压源设置中。。。");
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            step = 12;
            QTimer *tempTimer = new QTimer(this);
            connect(tempTimer, SIGNAL(timeout()), this,
                    SLOT(slotCheckIfUpdate()));
            tempTimer->setSingleShot(true);
            tempTimer->start(voltageTime);
        }
        else if (12 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板组端电压读取中。。。");
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            emit startTimerSignal(1000);
            step = 13;
        }
        else if (13 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板组端电压读取中。。。");
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            emit startTimerSignal(1000);
            step = 14;
        }
        else if (14 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板组端电压读取中。。。");
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            emit startTimerSignal(1000);
            step = 15;
        }
        else if (15 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 2;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板预充电压读取中。。。");
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            emit startTimerSignal(1000);
            step = 16;
        }
        break;
    }
    case TestSetHALLCurrent:
    {
        if (0 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            m_gzaz11 = 0.0;
            m_gzaz21 = 0.0;
            m_gzaz31 = 0.0;
            m_gzaf11 = 0.0;
            m_gzaf21 = 0.0;
            m_gzaf31 = 0.0;
            m_dcaz11 = 0.0;
            m_dcaz21 = 0.0;
            m_dcaz31 = 0.0;
            m_dcaf11 = 0.0;
            m_dcaf21 = 0.0;
            m_dcaf31 = 0.0;

            m_gzaz12 = 0.0;
            m_gzaz22 = 0.0;
            m_gzaz32 = 0.0;
            m_gzaf12 = 0.0;
            m_gzaf22 = 0.0;
            m_gzaf32 = 0.0;
            m_dcaz12 = 0.0;
            m_dcaz22 = 0.0;
            m_dcaz32 = 0.0;
            m_dcaf12 = 0.0;
            m_dcaf22 = 0.0;
            m_dcaf32 = 0.0;

            m_gzaz13 = 0.0;
            m_gzaz23 = 0.0;
            m_gzaz33 = 0.0;
            m_gzaf13 = 0.0;
            m_gzaf23 = 0.0;
            m_gzaf33 = 0.0;
            m_dcaz13 = 0.0;
            m_dcaz23 = 0.0;

            m_dcaz33 = 0.0;
            m_dcaf13 = 0.0;
            m_dcaf23 = 0.0;
            m_dcaf33 = 0.0;
            QByteArray temp;
            switch (m_locate)
            {
            case 3:
            {
                temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0", 27);

                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 7:
            {
                temp = QByteArray("\x06\x10\x00\x00\x00\x0A\x14\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 11:
            {
                temp = QByteArray("\x0A\x10\x00\x00\x00\x0A\x14\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 15:
            {
                temp = QByteArray("\x0E\x10\x00\x00\x00\x0A\x14\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            default:
                break;
            }
            emit signalReceiveStatus("电池电流测试中。。。");
            emit signalReceiveStatus("电流设置中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 1;
            QTimer *tempTimer = new QTimer(this);
            connect(tempTimer, SIGNAL(timeout()), this,
                    SLOT(slotCheckIfUpdate()));
            tempTimer->setSingleShot(true);
            tempTimer->start(currentTime);
        }
        else if (1 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 2;
        }
        else if (2 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流1读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 3;
        }
        else if (3 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 4;
        }
        else if (4 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流2读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 5;
        }
        else if (5 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 6;
        }
        else if (6 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 3;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流3读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 7;
        }
        else if(7 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            QByteArray temp;

            switch (m_locate)
            {
            case 3:
            {
                temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 7:
            {
                temp = QByteArray("\x06\x10\x00\x00\x00\x0A\x14\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 11:
            {
                temp = QByteArray("\x0A\x10\x00\x00\x00\x0A\x14\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 15:
            {
                temp = QByteArray("\x0E\x10\x00\x00\x00\x0A\x14\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            default:
                break;
            }
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit signalReceiveStatus("电流设置中。。。");
            step = 8;
            QTimer *tempTimer = new QTimer(this);
            connect(tempTimer, SIGNAL(timeout()), this,
                    SLOT(slotCheckIfUpdate()));
            tempTimer->setSingleShot(true);
            tempTimer->start(currentTime);
        }
        else if (8 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 9;
        }
        else if (9 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流1读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 10;
        }
        else if (10 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 11;
        }
        else if (11 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流2读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 12;
        }
        else if (12 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 13;
        }
        else if (13 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 3;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流3读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 14;
        }
        else if (14 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            QByteArray temp;
            switch (m_locate)
            {
            case 3:
            {
                temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 7:
            {
                temp = QByteArray("\x06\x10\x00\x00\x00\x0A\x14\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 11:
            {
                temp = QByteArray("\x0A\x10\x00\x00\x00\x0A\x14\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 15:
            {
                temp = QByteArray("\x0E\x10\x00\x00\x00\x0A\x14\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            default:
                break;
            }
            emit signalReceiveStatus("电流设置中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 15;
            QTimer *tempTimer = new QTimer(this);
            connect(tempTimer, SIGNAL(timeout()), this,
                    SLOT(slotCheckIfUpdate()));
            tempTimer->setSingleShot(true);
            tempTimer->start(currentTime);
        }
        else if (15 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 16;
        }
        else if (16 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 17;
        }
        else if (17 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 18;
        }
        else if (18 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流2读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 19;
        }
        else if (19 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 20;
        }
        else if (20 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 3;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流3读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 21;
        }
        else if (21 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            QByteArray temp;
            switch (m_locate)
            {
            case 3:
            {
                temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x0D\xAC\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 7:
            {
                temp = QByteArray("\x06\x10\x00\x00\x00\x0A\x14\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 11:
            {
                temp = QByteArray("\x0A\x10\x00\x00\x00\x0A\x14\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 15:
            {
                temp = QByteArray("\x0E\x10\x00\x00\x00\x0A\x14\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            default:
                break;
            }
            emit signalReceiveStatus("电流设置中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 22;
            QTimer *tempTimer = new QTimer(this);
            connect(tempTimer, SIGNAL(timeout()), this,
                    SLOT(slotCheckIfUpdate()));
            tempTimer->setSingleShot(true);
            tempTimer->start(currentTime);
        }
        else if (22 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 23;
        }
        else if (23 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 24;
        }
        else if (24 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 25;
        }
        else if (25 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流2读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 26;
        }
        else if (26 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 27;
        }
        else if (27 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 3;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流3读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 28;
        }
        else if (28 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            float k = (m_gzaf11 - m_gzaf12) / (m_dcaf11 - m_dcaf12);
            float b = m_gzaf11 - k * m_dcaf11;
            unsigned char data[8];
            data[0] = 0x0F;
            data[1] = 1;
            data[2] = 2;
            data[3]  = int(k * 1000) >> 8;
            data[4]  = int(k * 1000);
            int temp = b * 1000;
            if (b > 0)
            {
                data[5] = temp >> 8;
                data[6] = temp;
            }
            else if (b < 0)
            {
                data[5] = (65535 + temp) >> 8;
                data[6] = (65535 + temp);
            }
            else
            {
                data[5] = 0x00;
                data[6] = 0x00;
            }
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            k = (m_gzaf21 - m_gzaf22) / (m_dcaf21 - m_dcaf22);
            b = m_gzaf21 - k * m_dcaf21;
            data[0] = 0x0F;
            data[1] = 2;
            data[2] = 2;
            data[3]  = int(k * 1000) >> 8;
            data[4]  = int(k * 1000);
            temp = b * 1000;
            if (b > 0)
            {
                data[5] = temp >> 8;
                data[6] = temp;
            }
            else if (b < 0)
            {
                data[5] = (65535 + temp) >> 8;
                data[6] = (65535 + temp);
            }
            else
            {
                data[5] = 0x00;
                data[6] = 0x00;
            }
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            k = (m_gzaf31 - m_gzaf32) / (m_dcaf31 - m_dcaf32);
            b = m_gzaf31 - k * m_dcaf31;
            data[0] = 0x0F;
            data[1] = 3;
            data[2] = 2;
            data[3]  = int(k * 1000) >> 8;
            data[4]  = int(k * 1000);
            temp = b * 1000;
            if (b > 0)
            {
                data[5] = temp >> 8;
                data[6] = temp;
            }
            else if (b < 0)
            {
                data[5] = (65535 + temp) >> 8;
                data[6] = (65535 + temp);
            }
            else
            {
                data[5] = 0x00;
                data[6] = 0x00;
            }
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);

            k = (m_gzaz11 - m_gzaz12) / (m_dcaz11 - m_dcaz12);
            b = m_gzaz11 - k * m_dcaz11;
            data[0] = 0x0F;
            data[1] = 1;
            data[2] = 1;
            data[3]  = int(k * 1000) >> 8;
            data[4]  = int(k * 1000);
            temp = b * 1000;
            if (b > 0)
            {
                data[5] = temp >> 8;
                data[6] = temp;
            }
            else if (b < 0)
            {
                data[5] = (65535 + temp) >> 8;
                data[6] = (65535 + temp);
            }
            else
            {
                data[5] = 0x00;
                data[6] = 0x00;
            }
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            k = (m_gzaz21 - m_gzaz22) / (m_dcaz21 - m_dcaz22);
            b = m_gzaz21 - k * m_dcaz21;
            data[0] = 0x0F;
            data[1] = 2;
            data[2] = 1;
            data[3]  = int(k * 1000) >> 8;
            data[4]  = int(k * 1000);
            temp = b * 1000;
            if (b > 0)
            {
                data[5] = temp >> 8;
                data[6] = temp;
            }
            else if (b < 0)
            {
                data[5] = (65535 + temp) >> 8;
                data[6] = (65535 + temp);
            }
            else
            {
                data[5] = 0x00;
                data[6] = 0x00;
            }
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            k = (m_gzaz31 - m_gzaz32) / (m_dcaz31 - m_dcaz32);
            b = m_gzaz31 - k * m_dcaz31;
            data[0] = 0x0F;
            data[1] = 3;
            data[2] = 1;
            data[3]  = int(k * 1000) >> 8;
            data[4]  = int(k * 1000);
            temp = b * 1000;
            if (b > 0)
            {
                data[5] = temp >> 8;
                data[6] = temp;
            }
            else if (b < 0)
            {
                data[5] = (65535 + temp) >> 8;
                data[6] = (65535 + temp);
            }
            else
            {
                data[5] = 0x00;
                data[6] = 0x00;
            }
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("KB值下设中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            QTimer *tempTimer = new QTimer(this);
            connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotCheckIfUpdate()));
            tempTimer->setSingleShot(true);
            tempTimer->start(2000);
            step = 29;
        }
        else if(29 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            QByteArray temp;
            switch (m_locate)
            {
            case 3:
            {
                temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 7:
            {
                temp = QByteArray("\x06\x10\x00\x00\x00\x0A\x14\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 11:
            {
                temp = QByteArray("\x0A\x10\x00\x00\x00\x0A\x14\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 15:
            {
                temp = QByteArray("\x0E\x10\x00\x00\x00\x0A\x14\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            default:
                break;
            }
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 30;
            QTimer *tempTimer = new QTimer(this);
            connect(tempTimer, SIGNAL(timeout()), this,
                    SLOT(slotCheckIfUpdate()));
            tempTimer->setSingleShot(true);
            tempTimer->start(currentTime);
            emit signalReceiveStatus("电流设置中。。。");
        }
        else if (30 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 31;
        }
        else if (31 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 32;
        }
        else if (32 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 33;
        }
        else if (33 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 2;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流2读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 34;
        }
        else if (34 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 35;
        }
        else if (35 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 3;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流3读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 36;
        }
        else if(36 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            QByteArray temp;
            switch (m_locate)
            {
            case 3:
            {
                temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 7:
            {
                temp = QByteArray("\x06\x10\x00\x00\x00\x0A\x14\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 11:
            {
                temp = QByteArray("\x0A\x10\x00\x00\x00\x0A\x14\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 15:
            {
                temp = QByteArray("\x0E\x10\x00\x00\x00\x0A\x14\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            default:
                break;
            }
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 37;
            QTimer *tempTimer = new QTimer(this);
            connect(tempTimer, SIGNAL(timeout()), this,
                    SLOT(slotCheckIfUpdate()));
            tempTimer->setSingleShot(true);
            tempTimer->start(currentTime);
            emit signalReceiveStatus("电流设置中。。。");
        }
        else if (37 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 38;
        }
        else if (38 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 39;
        }
        else if (39 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 40;
        }
        else if (40 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 2;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流2读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 41;
        }
        else if (41 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 42;
        }
        else if (42 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 3;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流3读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 43;
        }
        break;
    }
    case TestZDResistance:
    {
        if(step == 0)
        {
            ifCanPass    = true;
            ifVPass      = true;
            ifRPass      = true;
            ifCPass      = true;
            ifSetSuccess = true;
            hallTime     = 0;
            canTime      = 0;
            if_zdPass = true;
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 07;
            data[2] = 00;
            data[3] = 00;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit stopTimerSignal();
            emit startTimerSignal(1000);
            emit signalReceiveStatus("终端电阻测试中。。。");
            step = 1;
            emit signalReceiveStatus("DO7关闭中。。。");
        }
        else if(step == 1)
        {
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 01;
            data[2] = 01;
            data[3] = 00;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit stopTimerSignal();
            emit startTimerSignal(1000);
            step = 2;
            emit signalReceiveStatus("DO1开启中。。。");
        }
        else if(step == 2)
        {
            QByteArray temp;
            switch(m_locate)
            {
            case 2:
            {
                temp = QByteArray("\x03\x04\x00\x00\x00\x0C", 6);
                Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                emit signalReceiveStatus("读取CAN阻值中。。。");
                break;
            }
            case 6:
            {
                temp = QByteArray("\x07\x04\x00\x00\x00\x0C", 6);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                emit signalReceiveStatus("读取CAN阻值中。。。");
                break;
            }
            case 10:
            {
                temp = QByteArray("\x0B\x04\x00\x00\x00\x0C", 6);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                emit signalReceiveStatus("读取CAN阻值中。。。");
                break;
            }
            case 14:
            {
                temp = QByteArray("\x0F\x04\x00\x00\x00\x0C", 6);
                Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                emit signalReceiveStatus("读取CAN阻值中。。。");
                break;
            }
            default:
                break;
            }
            emit stopTimerSignal();
            emit startTimerSignal(5000);
            step = 3;
        }
        else if(step == 3)
        {
            QByteArray temp;
            switch(m_locate)
            {
            case 4:
            {
                temp = QByteArray("\x04\x04\x00\x00\x00\x0C", 6);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
    }
                emit signalReceiveStatus("读取CAN阻值中。。。");
                break;
            }
            case 8:
            {
                temp = QByteArray("\x08\x04\x00\x00\x00\x0C", 6);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                emit signalReceiveStatus("读取CAN阻值中。。。");
                break;
            }
            case 12:
            {
                temp = QByteArray("\x0C\x04\x00\x00\x00\x0C", 6);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                emit signalReceiveStatus("读取CAN阻值中。。。");
                break;
            }
            case 16:
            {
                temp = QByteArray("\x10\x04\x00\x00\x00\x0C", 6);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                emit signalReceiveStatus("读取CAN阻值中。。。");
                break;
            }
            default:
                break;
            }
            emit stopTimerSignal();
            emit startTimerSignal(5000);
            step = 4;
        }
        break;
    }
    case TestBatteryVoltage:
    {
        if (0 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            openDO7();
            ifVPass = true;
            m_gzvz1 = 0.0;
            m_gzvz2 = 0.0;
            m_gzvz3 = 0.0;
            m_gzvy1 = 0.0;
            m_gzvy2 = 0.0;
            m_gzvy3 = 0.0;
            m_dcvz1 = 0.0;
            m_dcvz2 = 0.0;
            m_dcvz3 = 0.0;
            m_dcvy1 = 0.0;
            m_dcvy2 = 0.0;
            m_dcvy3 = 0.0;
            QByteArray temp;
            switch(m_locate)
            {
            case 3:
            {
                temp = QByteArray("\x01\x10\x10\x04\x00\x01\x02\x00\x01", 9);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 7:
            {
                temp = QByteArray("\x05\x10\x10\x04\x00\x01\x02\x00\x01", 9);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 11:
            {
                temp = QByteArray("\x09\x10\x10\x04\x00\x01\x02\x00\x01", 9);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 15:
            {
                temp = QByteArray("\x0D\x10\x10\x04\x00\x01\x02\x00\x01", 9);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            default:
                break;
            }
            emit signalReceiveStatus("电池总压测试中。。。");
            emit signalReceiveStatus("高压源开启中。。。");
            QTimer *tempTimer = new QTimer(this);
            connect(tempTimer, SIGNAL(timeout()), this,
                    SLOT(slotSetGYY()));
            tempTimer->setSingleShot(true);
            tempTimer->start(2000);
            qDebug()  << " step at " << step << " finish send batteryVoltage";
        }
        else if (17 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板组端电压读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            step = 18;
        }
        else if (18 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板组端电压读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            step = 19;
        }
        else if (19 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板组端电压读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            step = 20;
        }
        else if (20 == step)
        {
            qDebug()  << " step at " << step << " start send batteryVoltage";
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 2;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板预充电压读取中。。。");
            emit startTimerSignal(1000);
            qDebug()  << " step at " << step << " finish send batteryVoltage";
            step = 21;
        }
        break;
    }
    case TestSupplyVoltage:
    {
        if (0 == step)
        {
            m_dcgd = 0.0;
            m_gzgd = 0.0;
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 3;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            step = 1;
            emit startTimerSignal(1000);
            emit signalReceiveStatus("供电电压测试中。。。");
            emit signalReceiveStatus("待测板供电电压读取中。。。");
        }
        else if (1 == step)
        {
            unsigned char data[8];
            data[0] = 0x0C;
            data[1] = 3;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板供电电压读取中。。。");
            emit startTimerSignal(1000);
        }
        break;
    }
    case TestHALLCurrent:
    {
        if(0 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            m_gzaz11 = 0.0;
            m_gzaz21 = 0.0;
            m_gzaz31 = 0.0;
            m_gzaf11 = 0.0;
            m_gzaf21 = 0.0;
            m_gzaf31 = 0.0;
            m_dcaz11 = 0.0;
            m_dcaz21 = 0.0;
            m_dcaz31 = 0.0;
            m_dcaf11 = 0.0;
            m_dcaf21 = 0.0;
            m_dcaf31 = 0.0;

            m_gzaz12 = 0.0;
            m_gzaz22 = 0.0;
            m_gzaz32 = 0.0;
            m_gzaf12 = 0.0;
            m_gzaf22 = 0.0;
            m_gzaf32 = 0.0;
            m_dcaz12 = 0.0;
            m_dcaz22 = 0.0;
            m_dcaz32 = 0.0;
            m_dcaf12 = 0.0;
            m_dcaf22 = 0.0;
            m_dcaf32 = 0.0;

            m_gzaz13 = 0.0;
            m_gzaz23 = 0.0;
            m_gzaz33 = 0.0;
            m_gzaf13 = 0.0;
            m_gzaf23 = 0.0;
            m_gzaf33 = 0.0;
            m_dcaz13 = 0.0;
            m_dcaz23 = 0.0;
            m_dcaz33 = 0.0;
            m_dcaf13 = 0.0;
            m_dcaf23 = 0.0;
            m_dcaf33 = 0.0;
            QByteArray temp;
            switch (m_locate)
            {
            case 3:
            {
                temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 7:
            {
                temp = QByteArray("\x06\x10\x00\x00\x00\x0A\x14\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 11:
            {
                temp = QByteArray("\x0A\x10\x00\x00\x00\x0A\x14\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 15:
            {
                temp = QByteArray("\x0E\x10\x00\x00\x00\x0A\x14\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            default:
                break;
            }
            emit signalReceiveStatus("电流设置中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 44;
            QTimer *tempTimer = new QTimer(this);
            connect(tempTimer, SIGNAL(timeout()), this,
                    SLOT(slotCheckIfUpdate()));
            tempTimer->setSingleShot(true);
            tempTimer->start(currentTime);
        }
        else if (44 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 45;
        }
        else if (45 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 46;
        }
        else if (46 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 47;
        }
        else if (47 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 2;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流2读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 48;
        }
        else if (48 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 49;
        }
        else if (49 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 3;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流3读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 50;
        }
        else if(50 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            QByteArray temp;
            switch (m_locate)
            {
            case 3:
            {
                temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 7:
            {
                temp = QByteArray("\x06\x10\x00\x00\x00\x0A\x14\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 11:
            {
                temp = QByteArray("\x0A\x10\x00\x00\x00\x0A\x14\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            case 15:
            {
                temp = QByteArray("\x0E\x10\x00\x00\x00\x0A\x14\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC", 27);
                    Rs485Frame req;
                req.sendData = temp;
                req.sendTick = QDateTime::currentMSecsSinceEpoch();

                // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                if (!ok)
                {
                    qWarning() << "485发送缓冲区已满";
                }
                break;
            }
            default:
                break;
            }
            emit signalReceiveStatus("电流设置中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            step = 51;
            QTimer *tempTimer = new QTimer(this);
            connect(tempTimer, SIGNAL(timeout()), this,
                    SLOT(slotCheckIfUpdate()));
            tempTimer->setSingleShot(true);
            tempTimer->start(currentTime);
        }
        else if (51 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 52;
        }
        else if (52 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 53;
        }
        else if (53 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 54;
        }
        else if (54 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 2;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流2读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 55;
        }
        else if (55 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流1读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 56;
        }
        else if (56 == step)
        {
            qDebug()  << " step at " << step << " start send hallcurrent";
            ifVPass = true;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 3;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板电流3读取中。。。");
            qDebug()  << " step at " << step << " finish send hallcurrent";
            emit startTimerSignal(1000);
            step = 57;
        }
        break;
    }
    case TestInsulationResistance:
    {
        emit startFailTimerSignal(20000);
        m_dcjyz = 0.0;
        m_dcjyf = 0.0;
        unsigned char data[8];
        data[0] = 0x0A;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        step = 1;
        emit signalReceiveStatus("绝缘电阻测试中。。。");
        emit signalReceiveStatus("待测板绝缘电阻读取中。。。");
        emit startTimerSignal(1000);
        break;
    }
    case TestHALLSignal:
    {
        if (0 == step)
        {
            step = 1;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 2;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("霍尔信号测试中。。。");
            emit signalReceiveStatus("工装板电流2读取中。。。");
            emit startTimerSignal(1000);
        }
        else if (1 == step)
        {
            step = 2;
            unsigned char data[8];
            data[0] = 0x0E;
            data[1] = 3;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("工装板电流3读取中。。。");
            emit startTimerSignal(1000);
        }
        break;
    }
    case TestTemperature:
    {
        emit startFailTimerSignal(20000);
        if(0 == step)
        {
            m_temp1 = 0.0;
            m_temp2 = 0.0;
            m_temp3 = 0.0;
            m_temp4 = 0.0;
            unsigned char data[8];
            data[0] = 0x09;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("温度采集测试中。。。");
            emit signalReceiveStatus("第一路温度读取中。。。");
            emit startTimerSignal(1000);
            step = 1;
        }
        else if(1 == step)
        {
            unsigned char data[8];
            data[0] = 0x09;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("第二路温度读取中。。。");
            emit startTimerSignal(1000);
            step = 2;
        }
        else if(2 == step)
        {
            unsigned char data[8];
            data[0] = 0x09;
            data[1] = 3;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("第三路温度读取中。。。");
            emit startTimerSignal(1000);
            step = 3;
        }
        else if(3 == step)
        {
            unsigned char data[8];
            data[0] = 0x09;
            data[1] = 4;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("第四路温度读取中。。。");
            emit startTimerSignal(1000);
            step = 4;
        }
        break;
    }
    case TestEncodeInterface:
    {
        emit startFailTimerSignal(20000);
        if(0 == step)
        {
            unsigned char data[8];
            data[0] = 0x04;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("IO接口测试中。。。");
            emit signalReceiveStatus("IO1设置中。。。");
            emit startTimerSignal(1000);
            step = 1;
        }
        else if(1 == step)
        {
            unsigned char data[8];
            data[0] = 0x04;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("IO2设置中。。。");
            emit startTimerSignal(1000);
            step = 2;
        }
        else if(2 == step)
        {
            unsigned char data[8];
            data[0] = 0x04;
            data[1] = 1;
            data[2] = 1;
            data[3] = 1;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("IO1设置高电平中。。。");
            emit startTimerSignal(1000);
            step = 3;
        }
        else if(3 == step)
        {
            unsigned char data[8];
            data[0] = 0x05;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("IO2读取中。。。");
            emit startTimerSignal(1000);
            step = 4;
        }
        else if(4 == step)
        {
            unsigned char data[8];
            data[0] = 0x04;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("IO1设置低电平中。。。");
            emit startTimerSignal(1000);
            step = 5;
        }
        else if(5 == step)
        {
            unsigned char data[8];
            data[0] = 0x05;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("IO2读取中。。。");
            emit startTimerSignal(1000);
            step = 6;
        }
        break;
    }
    case TestDO:
    {
        if (0 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 1;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO测试中。。。");
            emit signalReceiveStatus("DO1闭合中。。。");
        }
        else if (1 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 2;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO1状态中。。。");
        }
        else if (2 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 3;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO1断开中。。。");
        }
        else if (3 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 4;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO1状态中。。。");
        }
        else if (4 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 2;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 5;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO2闭合中。。。");
        }
        else if (5 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 6;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO2状态中。。。");
        }
        else if (6 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 7;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO2断开中。。。");
        }
        else if (7 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 8;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO2状态中。。。");
        }
        else if (8 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 3;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 9;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO3闭合中。。。");
        }
        else if (9 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 3;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 10;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO3状态中。。。");
        }
        else if (10 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 3;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 11;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO3断开中。。。");
        }
        else if (11 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 3;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 12;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO3状态中。。。");
        }
        else if (12 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 4;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 13;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO4闭合中。。。");
        }
        else if (13 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 4;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 14;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO4状态中。。。");
        }
        else if (14 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 4;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 15;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO4断开中。。。");
        }
        else if (15 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 4;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 16;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO4状态中。。。");
        }
        else if (16 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 5;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 17;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO5闭合中。。。");
        }
        else if (17 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 5;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 18;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO5状态中。。。");
        }
        else if (18 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 5;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 19;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO5断开中。。。");
        }
        else if (19 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 5;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 20;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO5状态中。。。");
        }
        else if (20 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 6;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 21;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO6闭合中。。。");
        }
        else if (21 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 6;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 22;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO6状态中。。。");
        }
        else if (22 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 6;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 23;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO6断开中。。。");
        }
        else if (23 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 6;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 24;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO6状态中。。。");
        }
        else if (24 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 7;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 25;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO7闭合中。。。");
        }
        else if (25 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 7;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 26;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO7状态中。。。");
        }
        else if (26 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 7;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 27;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO7断开中。。。");
        }
        else if (27 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 7;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 28;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO7状态中。。。");
        }
        else if (28 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 8;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 29;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO8闭合中。。。");
        }
        else if (29 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 8;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 30;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO8状态中。。。");
        }
        else if (30 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 8;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 31;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO8断开中。。。");
        }
        else if (31 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x02;
            data[1] = 8;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 32;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DO8状态中。。。");
        }
        break;
    }
    case TestDIAndSLP:
    {
        if (0 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 1;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 1;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DI测试中。。。");
            emit signalReceiveStatus("DO1闭合中。。。");
        }
        else if (1 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 2;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI2状态中。。。");
        }
        else if (2 == step)
        {
            emit stopTimerSignal();
                emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 3;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO1断开中。。。");
        }
        else if (3 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 4;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI2状态中。。。");
        }
        else if (4 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 2;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 5;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO2闭合中。。。");
        }
        else if (5 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 4;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 6;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI4状态中。。。");
        }
        else if (6 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 2;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 7;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO2断开中。。。");
        }
        else if (7 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 4;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 8;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI4状态中。。。");
        }
        else if (8 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 3;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 9;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO3闭合中。。。");
        }
        else if (9 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 6;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 10;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI6状态中。。。");
        }
        else if (10 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 3;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 11;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO3断开中。。。");
        }
        else if (11 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 6;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 12;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI6状态中。。。");
        }
        else if (12 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 4;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 13;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO4闭合中。。。");
        }
        else if (13 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 14;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI1状态中。。。");
        }
        else if (14 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 4;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 15;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO4断开中。。。");
        }
        else if (15 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 16;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI1状态中。。。");
        }
        else if (16 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 5;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 17;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO5闭合中。。。");
        }
        else if (17 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 3;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 18;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI3状态中。。。");
        }
        else if (18 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 5;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 19;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO5断开中。。。");
        }
        else if (19 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 3;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 20;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI3状态中。。。");
        }
        else if (20 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 6;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 21;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO6闭合中。。。");
        }
        else if (21 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 5;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 22;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI5状态中。。。");
        }
        else if (22 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 6;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 23;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO6断开中。。。");
        }
        else if (23 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 5;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 24;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI5状态中。。。");
        }
        else if (24 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 7;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 25;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO7闭合中。。。");
        }
        else if (25 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 7;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 26;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI7状态中。。。");
        }
        else if (26 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 7;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 27;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO7断开中。。。");
        }
        else if (27 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 7;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 28;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI7状态中。。。");
        }
        else if (28 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 8;
            data[2] = 1;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 29;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO8闭合中。。。");
        }
        else if (29 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 8;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 30;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI8状态中。。。");
        }
        else if (30 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x01;
            data[1] = 8;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 31;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("DO8断开中。。。");
        }
        else if (31 == step)
        {
            emit stopTimerSignal();
            emit startTimerSignal(2000);
            unsigned char data[8];
            data[0] = 0x03;
            data[1] = 8;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 32;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("读取DI8状态中。。。");
        }
        break;
    }
    case TestSerialCommunication:
    {
        emit startFailTimerSignal(20000);
        if(step == 0)
        {
            unsigned char data[8];
            data[0] = 0x14;
            data[1] = 1;
            data[2] = 0x1F;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step = 1;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("485通信测试中。。。");
            emit signalReceiveStatus("485-1测试中。。。");
            emit startTimerSignal(10000);
        }
        else if(step == 1)
        {
            unsigned char data[8];
            data[0] = 0x14;
            data[1] = 2;
            data[2] = 0x1F;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step = 2;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("485-2测试中。。。");
            emit startTimerSignal(10000);
        }
        break;
    }
    case TestCANCommunication:
    {
        emit startFailTimerSignal(20000);
        if(step == 0)
        {
            unsigned char data[8];
            data[0] = 0x11;
            data[1] = 1;
            data[2] = 0x12;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step = 1;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("CAN通信测试中。。。");
            emit signalReceiveStatus("CAN0测试中。。。");
        }
        else if(step == 1)
        {
            unsigned char data[8];
            data[0] = 0x11;
            data[1] = 2;
            data[2] = 0x12;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step = 2;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("CAN1测试中。。。");
        }
        else if(step == 2)
        {
            unsigned char data[8];
            data[0] = 0x11;
            data[1] = 3;
            data[2] = 0x12;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            step = 3;
            emit signalReceiveStatus("CAN2测试中。。。");
        }
        break;
    }
    case TestEEPROM:
    {
        emit startFailTimerSignal(20000);
        QTimer *tempTimer = new QTimer(this);
        connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotTestE2()));
        tempTimer->setSingleShot(true);
        tempTimer->start(2000);
        break;
    }
    case TestFlash:
    {
        emit startFailTimerSignal(20000);
        unsigned char data[8];
        data[0]      = 0x18;
        data[1]      = 0;
        data[2]      = 0;
        data[3]      = 0;
        data[4]      = 0;
        data[5]      = 0;
        data[6]      = 0;
        data[7]      = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit signalReceiveStatus("FLASH测试中。。。");
        emit startTimerSignal(1000);
        break;
    }
    case TestSetRTC:
    {
        emit startFailTimerSignal(20000);
        unsigned char data[8];
        QString year = QDateTime::currentDateTime().toString("yyyy");
        QString mon  = QDateTime::currentDateTime().toString("MM");
        QString date = QDateTime::currentDateTime().toString("dd");
        QString hour = QDateTime::currentDateTime().toString("hh");
        QString min  = QDateTime::currentDateTime().toString("mm");
        QString sec  = QDateTime::currentDateTime().toString("ss");
        data[0]      = 0x19;
        data[1]      = year.right(2).toInt();
        data[2]      = mon.toInt();
        data[3]      = date.toInt();
        data[4]      = hour.toInt();
        data[5]      = min.toInt();
        data[6]      = sec.toInt();
        data[7]      = 0;
        emit startTimerSignal(1000);
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit signalReceiveStatus("RTC下设中。。。");
        break;
    }
    case TestRTC:
    {
        emit startFailTimerSignal(20000);
        unsigned char data[8];
        data[0] = 0x12;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x181F << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit startTimerSignal(1000);
        emit signalReceiveStatus("RTC读取中。。。");
        break;
    }
    case TestSystemTemperature:
    {
        emit startFailTimerSignal(20000);
        if (0 == step)
        {
            m_dctemp = 0.0;
            m_gztemp = 0.0;
            unsigned char data[8];
            data[0] = 0x0B;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 1;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("系统温度测试中。。。");
            emit signalReceiveStatus("工装板温度读取中。。。");
            emit startTimerSignal(1000);
        }
        else if (1 == step)
        {
            unsigned char data[8];
            data[0] = 0x0B;
            data[1] = 1;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0;
            data[6] = 0;
            data[7] = 0;
            step    = 1;
            TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("待测板温度读取中。。。");
            emit startTimerSignal(1000);
        }
        break;
    }
    case TestInternet:
    {
        unsigned char data[8];
        data[0] = 0x0C;
        data[1] = 2;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit signalReceiveStatus("待测板不带kb值预充电压读取中。。。");
        data[0] = 0x0C;
        data[1] = 1;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit signalReceiveStatus("待测板不带kb值组端电压读取中。。。");
        data[0] = 0x0C;
        data[1] = 2;
        data[2] = 1;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit signalReceiveStatus("待测板带kb值预充电压读取中。。。");
        data[0] = 0x0C;
        data[1] = 1;
        data[2] = 1;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit signalReceiveStatus("待测板带kb值组端电压读取中。。。");
        emit startFailTimerSignal(20000);
        data[0] = 0x26;
        data[1] = 2;
        data[2] = 195;
        data[3] = 16;
        data[4] = 19;
        data[5] = 100 + m_locate;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit signalReceiveStatus("设置IP地址中。。。");
        step = 0;
        QTimer *tempTimer = new QTimer(this);
        connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotTestInternet()));
        tempTimer->setSingleShot(true);
        tempTimer->start(5000);
        break;
    }
    case TestPower:
    {
        emit startFailTimerSignal(20000);
        emit signalReceiveStatus("功耗测试中。。。");
        float power = m_current * m_dcgd - 4.3;
        if(if_mes)
        {
            qDebug() << "current is " << m_current << " and v is " << m_dcgd;
            if (3 >= power && power >= 1)
            {
                QJsonObject Power;
                Power.insert("standard", "3W");
                Power.insert("test",
                             QString::number(power, 'f', 2) +
                                 "W");
                Power.insert("difference", "/");
                Power.insert("allowable", "≤3W");
                Power.insert("result", "通过");
                testResults.insert("Power", Power);
                step = 0;
                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();
            }
            else
            {
                emit signalReceiveStatus(
                    "功耗为" + QString::number(power) +
                    "W，不符合要求");
                QJsonObject Power;
                Power.insert("standard", "3W");
                Power.insert("test",
                             QString::number(power, 'f', 2) +
                                 "W");
                Power.insert("difference", "/");
                Power.insert("allowable", "≤3W");
                Power.insert("result", "未通过");
                testResults.insert("Power", Power);
                step = 0;
                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();
            }
        }
        else
        {
            qDebug() << "current is " << m_current << " and v is " << m_dcgd
                                             << " and power is " << power;
            switch (m_type)
            {
            case 0:
            case 1:
            {
                ExcelItem temp;
                temp.row = 72;
                temp.col = 15;
                temp.word =
                    QString::number(power, 'f', 2) + "W";
                items.push_back(temp);
                temp.row  = 73;
                temp.col  = 15;
                temp.word = "3W";
                items.push_back(temp);
                temp.row  = 74;
                temp.col  = 15;
                temp.word = "/";
                items.push_back(temp);
                temp.row  = 75;
                temp.col  = 15;
                temp.word = "≤3W";
                items.push_back(temp);
                if (3 >= power && power >= 1)
                {

                    failTimer->stop();
                    temp.row  = 76;
                    temp.col  = 15;
                    temp.word = "通过";
                    items.push_back(temp);
                    step = 0;
                    emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                }
                else
                {
                    emit signalReceiveStatus(
                        "功耗为" + QString::number(power) +
                        "W，不符合要"
                        "求");
                    temp.row  = 76;
                    temp.col  = 15;
                    temp.word = "未通过";
                    items.push_back(temp);
                    step = 0;
                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                }
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case TestSetMAC:
    {
        emit startFailTimerSignal(20000);
        unsigned char data[8];
        data[0] = 0x07;
        data[1] = m_macList.at(0).toInt(nullptr, 16);
        data[2] = m_macList.at(1).toInt(nullptr, 16);
        data[3] = m_macList.at(2).toInt(nullptr, 16);
        data[4] = m_macList.at(3).toInt(nullptr, 16);
        data[5] = m_macList.at(4).toInt(nullptr, 16);
        data[6] = m_macList.at(5).toInt(nullptr, 16);
        data[7] = 0xFF;
        TransmitCAN(((0x1813 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        QTimer *tempTimer = new QTimer(this);
        tempTimer->setSingleShot(true);
        connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotMACTimeout()));
        emit signalReceiveStatus("MAC地址下设测试中。。。");
        tempTimer->start(1000);
        break;
    }
    case TestReadMAC:
    {
        emit startFailTimerSignal(20000);
        unsigned char data[8];
        data[0] = 0x13;
        data[1] = 0x07;
        data[2] = 0xFF;
        data[3] = 0xFF;
        data[4] = 0xFF;
        data[5] = 0xFF;
        data[6] = 0xFF;
        data[7] = 0xFF;
        TransmitCAN(((0x181F << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit signalReceiveStatus("MAC地址读取测试中。。。");
        emit startTimerSignal(1000);
        step = 0;
        break;
    }
    case TestSetSerial:
    {
        emit startFailTimerSignal(20000);
        unsigned char data[8];
        switch (step)
        {
        case 0:
        {
            unsigned char data[8];
            data[0] = 0x00;
            data[1] = 0x00;
            data[2] = 0x00;
            data[3] = 0x00;
            data[4] = 0x00;
            data[5] = 0x00;
            data[6] = 0x00;
            data[7] = 0x00;
            TransmitCAN(((0x181F << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("序列号下设测试中。。。");
            emit signalReceiveStatus("系统信息读取中。。。");
            emit startTimerSignal(1000);
            step = 1;
            break;
        }
        case 1:
        {
            QByteArray tempArray = m_serial.toUtf8();
            data[0]              = 0x01;
            data[1]              = int(tempArray.at(0));
            data[2]              = int(tempArray.at(1));
            data[3]              = int(tempArray.at(2));
            data[4]              = int(tempArray.at(3));
            data[5]              = int(tempArray.at(4));
            data[6]              = int(tempArray.at(5));
            data[7]              = int(tempArray.at(6));
            TransmitCAN(((0x1800 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            data[0] = 0x02;
            data[1] = int(tempArray.at(7));
            data[2] = int(tempArray.at(8));
            data[3] = int(tempArray.at(9));
            data[4] = int(tempArray.at(10));
            data[5] = int(tempArray.at(11));
            data[6] = 0xFF;
            data[7] = 0xFF;
            TransmitCAN(((0x1800 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            data[0] = int(hardwareArray.at(0));
            data[1] = int(hardwareArray.at(1));
            data[2] = int(hardwareArray.at(2));
            data[3] = int(hardwareArray.at(3));
            data[4] = int(hardwareArray.at(4));
            data[5] = int(hardwareArray.at(5));
            data[6] = int(hardwareArray.at(6));
            data[7] = int(hardwareArray.at(7));
            TransmitCAN(((0x1800 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            if (int(nameArray.at(0)) == 4)
            {
                data[0] = 4;
                data[1] = int(nameArray.at(1));
                data[2] = int(nameArray.at(2));
                data[3] = int(nameArray.at(3));
                data[4] = int(nameArray.at(4));
                data[5] = int(nameArray.at(5));
                data[6] = int(nameArray.at(6));
                data[7] = int(nameArray.at(7));
                TransmitCAN(((0x1800 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
                data[0] = 5;
                data[1] = int(nameArray.at(1));
                data[2] = int(nameArray.at(2));
                data[3] = int(nameArray.at(3));
                data[4] = int(nameArray.at(4));
                data[5] = int(nameArray.at(5));
                data[6] = int(nameArray.at(6));
                data[7] = int(nameArray.at(7));
                TransmitCAN(((0x1800 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            }
            else
            {
                data[0] = 5;
                data[1] = nameArray.at(1);
                data[2] = nameArray.at(2);
                data[3] = nameArray.at(3);
                data[4] = nameArray.at(4);
                data[5] = nameArray.at(5);
                data[6] = nameArray.at(6);
                data[7] = nameArray.at(7);
                TransmitCAN(((0x1800 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
                data[0] = nameArray.at(8);
                data[1] = nameArray.at(9);
                data[2] = nameArray.at(10);
                data[3] = nameArray.at(11);
                data[4] = nameArray.at(12);
                data[5] = nameArray.at(13);
                data[6] = nameArray.at(14);
                data[7] = nameArray.at(15);
                TransmitCAN(((0x1800 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
                data[0] = nameArray.at(16);
                data[1] = nameArray.at(17);
                data[2] = nameArray.at(18);
                data[3] = nameArray.at(19);
                data[4] = nameArray.at(20);
                data[5] = nameArray.at(21);
                data[6] = nameArray.at(22);
                data[7] = nameArray.at(23);
                TransmitCAN(((0x1800 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
                data[0] = nameArray.at(24);
                data[1] = nameArray.at(25);
                data[2] = nameArray.at(26);
                data[3] = nameArray.at(27);
                data[4] = nameArray.at(28);
                data[5] = nameArray.at(29);
                data[6] = nameArray.at(30);
                data[7] = nameArray.at(31);
                TransmitCAN(((0x1800 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
                data[0] = nameArray.at(32);
                data[1] = nameArray.at(33);
                data[2] = nameArray.at(34);
                data[3] = nameArray.at(35);
                data[4] = nameArray.at(36);
                data[5] = nameArray.at(37);
                data[6] = nameArray.at(38);
                data[7] = nameArray.at(39);
                TransmitCAN(((0x1800 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            }
            data[0] = 0xFF;
            data[1] = 0xFF;
            data[2] = 0xFF;
            data[3] = 0xFF;
            data[4] = 0xFF;
            data[5] = 0xFF;
            data[6] = 0xFF;
            data[7] = 0xFF;
            TransmitCAN(((0x1800 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);

            QTimer *tempTimer = new QTimer(this);
            tempTimer->setSingleShot(true);
            connect(tempTimer, SIGNAL(timeout()), this,
                    SLOT(slotMACTimeout()));
            emit signalReceiveStatus("序列号下设中。。。");
            tempTimer->start(1000);
            break;
        }
        default:
            break;
        }
        break;
    }
    case TestReadSerial:
    {
        emit startFailTimerSignal(20000);
        unsigned char data[8];
        data[0] = 0x00;
        data[1] = 0x00;
        data[2] = 0x00;
        data[3] = 0x00;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        TransmitCAN(((0x181F << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit signalReceiveStatus("序列号读取测试中。。。");
        emit startTimerSignal(1000);
        break;
    }
    case TestSPI:
    {
        emit startFailTimerSignal(20000);
        unsigned char data[8];
        data[0] = 0x16;
        data[1] = 0x00;
        data[2] = 0x00;
        data[3] = 0x00;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit signalReceiveStatus("菊花链测试中。。。");
        emit startTimerSignal(1000);
        step = 0;
        break;
    }
    case TestSoftWareVersion:
    {
        emit startFailTimerSignal(20000);
        unsigned char data[8];
        data[0] = 0x01;
        data[1] = 0x00;
        data[2] = 0x00;
        data[3] = 0x00;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        TransmitCAN(((0x1840 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit signalReceiveStatus("软件版本测试中。。。");
        emit startTimerSignal(1000);
        step = 0;
        break;
    }
    case TestHardWareVersion:
    {
        emit startFailTimerSignal(20000);
        unsigned char data[8];
        data[0] = 0x00;
        data[1] = 0x00;
        data[2] = 0x00;
        data[3] = 0x00;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        TransmitCAN(((0x181F << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit signalReceiveStatus("硬件版本测试中。。。");
        emit startTimerSignal(1000);
        step = 0;
        break;
    }
    case TestKB:
    {
        emit startFailTimerSignal(20000);
        emit stopTimerSignal();
        emit startTimerSignal(100);
        unsigned char data[8];
        switch (step)
        {
        case 0:
        {
            data[0] = 0x1B;
            data[1] = 0x02;
            data[2] = 0x00;
            data[3] = 0x00;
            data[4] = 0x00;
            data[5] = 0x00;
            data[6] = 0x00;
            data[7] = 0x00;
            TransmitCAN(((0x181F << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("预充KB值读取中。。。");
            emit startTimerSignal(1000);
            step = 1;
            break;
        }
        case 1:
        {
            data[0] = 0x1B;
            data[1] = 0x03;
            data[2] = 0x00;
            data[3] = 0x00;
            data[4] = 0x00;
            data[5] = 0x00;
            data[6] = 0x00;
            data[7] = 0x00;
            TransmitCAN(((0x181F << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
            emit signalReceiveStatus("组端KB值读取中。。。");
            emit startTimerSignal(1000);
            step = 0;
            break;
        }
        default:
            break;
        }
        break;
    }
    case FinishTest:
    {
        emit stopTimerSignal();
        emit stopFailTimerSignal();
        step = 0;
        emit signalReceiveStatus("测试结束。。。");
        emit signalSetExcelItem(items);
        qDebug() << "cantest result is " << testResults;
        emit signalSendTestResult(testResults);
        closeDO7();
        break;
    }
    case ExitTest:
    {
        break;
    }
    case UnknownType:
    {
        break;
    }
    default:
    {

        break;
    }
    }
}

void CANGetAndSend::slotSetCurrentTestStep(int type)
{
    const TestItem nextTestType = static_cast<TestItem>(type);
    if (lastTestType != nextTestType)
    {
        qDebug() << "[TEST] switch test item"
                 << "locate" << m_locate
                 << "from" << lastTestType
                 << "to" << nextTestType
                 << "oldStep" << step;
        emit stopTimerSignal();
        emit stopFailTimerSignal();
        cancelPendingAsyncCallbacks();
        step = 0;
        hallTime = 0;
        canTime = 0;
        m_hallReadTime = 0;
    }
    lastTestType = nextTestType;
}
void CANGetAndSend::cancelPendingAsyncCallbacks()
{
    const auto timers = findChildren<QTimer *>(QString(), Qt::FindDirectChildrenOnly);
    for (QTimer *timer : timers)
    {
        if (timer == failTimer || timer == retryTimer || timer == setFailTimer)
        {
            continue;
        }
        if (timer->isActive())
        {
            timer->stop();
        }
        timer->deleteLater();
    }
    if (retryTimer != NULL && retryTimer->isActive())
    {
        retryTimer->stop();
    }
    if (failTimer != NULL && failTimer->isActive())
    {
        failTimer->stop();
    }
    if (setFailTimer != NULL && setFailTimer->isActive())
    {
        setFailTimer->stop();
    }
    if (testTimer != NULL)
    {
        testTimer->stop();
        testTimer->deleteLater();
        testTimer = NULL;
    }
}

void CANGetAndSend::slotReceiveData(VCI_CAN_OBJ pReceive)
{
    currentData.clear();
    for (int i = 0; i < 8; i++)
    {
        currentData.append(pReceive.Data[i]);
    }
    currentData = currentData.toHex();
    std::string tempString;
    unsigned_to_hex(pReceive.ID, tempString);
    int id3 = (0x1813F4 << 8) | ((m_locate + 0x20) & 0xFF);
    int id4 = (0x1800F4 << 8) | ((m_locate + 0x20) & 0xFF);
    int id5 = (0x1801F4 << 8) | ((m_locate + 0x60 - 1) & 0xFF);
    int id6 = (0x181BF4 << 8) | ((m_locate + 0x20) & 0xFF);
    int id7 = (0x1803F4 << 8) | ((m_locate + 0x60 - 1) & 0xFF);
    int id8 = (0x1841F4 << 8) | ((m_locate + 0x20) & 0xFF);
    int id9 = (0x1839F4 << 8) | ((m_locate + 0x20) & 0xFF);
    int id11 = (0x1812F4 << 8) | ((m_locate + 0x20) & 0xFF);
    int id13 = (0x1855F4 << 8) | ((m_locate + 0x20) & 0xFF);
    int id14 = (0x1855F4 << 8) | ((m_locate + 0x60 - 1) & 0xFF);
    if(static_cast<int>(pReceive.ID) == id3 || static_cast<int>(pReceive.ID) == id4 ||
            static_cast<int>(pReceive.ID) == id5 || static_cast<int>(pReceive.ID) == id6 ||
            static_cast<int>(pReceive.ID) == id7 || static_cast<int>(pReceive.ID) == id8 ||
            static_cast<int>(pReceive.ID) == id9 || static_cast<int>(pReceive.ID) == id11 ||
            static_cast<int>(pReceive.ID) == id13 || static_cast<int>(pReceive.ID) == id14 || ((pReceive.ID & 0xFFFF) == 0 && m_locate == 1))
    {
        qDebug() << "receive " << currentData << " from "
                 << QString::fromStdString(tempString);
    }
    if (if_mes)
    {
        if(static_cast<int>(pReceive.ID) == id3)
        {
            if (7 == static_cast<int>(pReceive.Data[0]) && lastTestType == TestReadMAC)
            {
                emit stopTimerSignal();
                QString tempString =
                    QString::number(static_cast<int>(pReceive.Data[1]), 16);
                QString mac = tempString.toUpper();
                if (mac.length() == 1)
                {
                    mac = "0" + mac;
                }
                mac.append(QByteArray(":", sizeof(":")-1));
                tempString =
                    QString::number(static_cast<int>(pReceive.Data[2]), 16);
                if (tempString.length() == 1)
                {
                    tempString = "0" + tempString;
                }
                mac.append(tempString.toUpper()).append(QByteArray(":", sizeof(":")-1));
                tempString =
                    QString::number(static_cast<int>(pReceive.Data[3]), 16);
                if (tempString.length() == 1)
                {
                    tempString = "0" + tempString;
                }
                mac.append(tempString.toUpper()).append(QByteArray(":", sizeof(":")-1));
                tempString =
                    QString::number(static_cast<int>(pReceive.Data[4]), 16);
                if (tempString.length() == 1)
                {
                    tempString = "0" + tempString;
                }
                mac.append(tempString.toUpper()).append(QByteArray(":", sizeof(":")-1));
                tempString =
                    QString::number(static_cast<int>(pReceive.Data[5]), 16);
                if (tempString.length() == 1)
                {
                    tempString = "0" + tempString;
                }
                mac.append(tempString.toUpper()).append(QByteArray(":", sizeof(":")-1));
                tempString =
                    QString::number(static_cast<int>(pReceive.Data[6]), 16);
                if (tempString.length() == 1)
                {
                    tempString = "0" + tempString;
                }
                mac.append(tempString.toUpper());
                if (m_macList.size() != 0)
                {
                    qDebug()
                        << "mac receive is " << mac << " and mac normal is "
                        << m_macList.join(":");
                    if (!mac.compare(m_macList.join(":"),
                                     Qt::CaseInsensitive))
                    {

                        QJsonObject MAC;
                        MAC.insert("standard", m_macList.join(":"));
                        MAC.insert("test", mac);
                        MAC.insert("difference", "0");
                        MAC.insert("allowable", "0");
                        MAC.insert("result", "通过");
                        testResults.insert("MAC", MAC);
                        emit singalReceiveMAC(mac);
                        step = 0;
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();
                    }
                    else
                    {
                        QJsonObject MAC;
                        MAC.insert("standard", m_macList.join(":"));
                        MAC.insert("test", mac);
                        MAC.insert("difference", "0");
                        MAC.insert("allowable", "0");
                        MAC.insert("result", "未通过");
                        testResults.insert("MAC", MAC);
                        emit signalReceiveStatus("MAC地址设置失败");
                        step = 0;
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();
                    }
                }
            }
        }
        else if(static_cast<int>(pReceive.ID) == id4)
        {
            if (lastTestType == TestSetSerial)
            {
                if (3 == static_cast<int>(pReceive.Data[0]))
                {
                    hardwareArray.clear();
                    hardwareArray.append(static_cast<int>(pReceive.Data[0]))
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                }
                else if (4 == static_cast<int>(pReceive.Data[0]))
                {
                    nameArray.clear();
                    nameArray.append(pReceive.Data[0])
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    step = 1;
                    emit stopTimerSignal();
                    updateTestItem();
                }
                else if (5 == static_cast<int>(pReceive.Data[0]))
                {
                    nameArray.clear();
                    nameArray.append(pReceive.Data[0])
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                }
                else if (6 == static_cast<int>(pReceive.Data[0]))
                {
                    nameArray.append(pReceive.Data[0])
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                }
                else if (7 == static_cast<int>(pReceive.Data[0]))
                {
                    nameArray.append(pReceive.Data[0])
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                }
                else if (8 == static_cast<int>(pReceive.Data[0]))
                {
                    nameArray.append(pReceive.Data[0])
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                }
                else if (9 == static_cast<int>(pReceive.Data[0]))
                {
                    nameArray.append(pReceive.Data[0])
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    step = 1;
                    emit stopTimerSignal();
                    updateTestItem();
                }
            }
            else if (lastTestType == TestReadSerial)
            {
                if (1 == static_cast<int>(pReceive.Data[0]))
                {
                    receiveArray.clear();
                    receiveArray.append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                }
                else if (2 == static_cast<int>(pReceive.Data[0]))
                {
                    receiveArray.append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5]);
                    QByteArray tempArray = m_serial.toUtf8();
                    serial_receive       = QString::fromUtf8(receiveArray);
                    step                 = 0;

                    qDebug() << "receive serial is " << receiveArray
                             << " and set serial is " << tempArray;
                    emit stopTimerSignal();
                    if (m_serial == "")
                    {
                        emit signalReceiveSerial(serial_receive);
                        QJsonObject Serial;
                        Serial.insert("standard", serial_receive);
                        Serial.insert("test", serial_receive);
                        Serial.insert("difference", "0");
                        Serial.insert("allowable", "0");
                        Serial.insert("result", "通过");
                        testResults.insert("Serial", Serial);
                        QTimer *tempTimer = new QTimer(this);
                        connect(tempTimer, SIGNAL(timeout()), this,
                                SLOT(slotSendSuccess()));
                        tempTimer->setSingleShot(true);
                        tempTimer->start(2000);
                    }
                    else
                    {
                        if (tempArray == receiveArray)
                        {
                            QJsonObject Serial;
                            Serial.insert("standard", m_serial);
                            Serial.insert("test", serial_receive);
                            Serial.insert("difference", "0");
                            Serial.insert("allowable", "0");
                            Serial.insert("result", "通过");
                            testResults.insert("Serial", Serial);

                            emit signalReceiveSerial(serial_receive);
                            QTimer *tempTimer = new QTimer(this);
                            connect(tempTimer, SIGNAL(timeout()), this,
                                    SLOT(slotSendSuccess()));
                            tempTimer->setSingleShot(true);
                            tempTimer->start(2000);
                        }
                        else
                        {
                            QJsonObject Serial;
                            Serial.insert("standard", m_serial);
                            Serial.insert("test", serial_receive);
                            Serial.insert("difference", "0");
                            Serial.insert("allowable", "0");
                            Serial.insert("result", "未通过");
                            testResults.insert("Serial", Serial);
                            QTimer *tempTimer = new QTimer(this);
                            connect(tempTimer, SIGNAL(timeout()), this,
                                    SLOT(slotSendFail()));
                            tempTimer->setSingleShot(true);
                            tempTimer->start(2000);

                            emit signalReceiveStatus("序列号设置失败");
                        }
                    }
                }
            }
            else if (lastTestType == TestHardWareVersion)
            {
                if (3 == static_cast<int>(pReceive.Data[0]))
                {
                    hardwareArray.clear();
                    hardwareArray.append(static_cast<int>(pReceive.Data[0]))
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                }
                else if (4 == static_cast<int>(pReceive.Data[0]))
                {
                    nameArray.clear();
                    nameArray.append(pReceive.Data[0])
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                }
                if (3 == static_cast<int>(pReceive.Data[0]))
                {
                    emit stopTimerSignal();
                    QStringList list = m_hardware.split(".");
                    if (list.at(0).toInt() ==
                            static_cast<int>(pReceive.Data[1]) &&
                        list.at(1).toInt() ==
                            static_cast<int>(pReceive.Data[2]) &&
                        list.at(2).toInt() ==
                            static_cast<int>(pReceive.Data[3]))
                    {
                        QJsonObject HardwareVersionTest;
                        HardwareVersionTest.insert("standard",
                                                   "HV" + m_hardware);
                        HardwareVersionTest.insert(
                            "test",
                            "HV" +
                                QString::number(
                                    static_cast<int>(pReceive.Data[1]))
                                    .append(QByteArray(".", sizeof(".")-1))
                                    .append(QString::number(
                                        static_cast<int>(pReceive.Data[2])))
                                    .append(QByteArray(".", sizeof(".")-1))
                                    .append(
                                        QString::number(static_cast<int>(
                                            pReceive.Data[3]))));
                        HardwareVersionTest.insert("difference", "0");
                        HardwareVersionTest.insert("allowable", "0");
                        HardwareVersionTest.insert("result", "通过");
                        testResults.insert("HardwareVersionTest",
                                           HardwareVersionTest);

                        step = 0;
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    else
                    {
                        QJsonObject HardwareVersionTest;
                        HardwareVersionTest.insert("standard",
                                                   "HV" + m_hardware);
                        HardwareVersionTest.insert(
                            "test",
                            "HV" +
                                QString::number(
                                    static_cast<int>(pReceive.Data[1]))
                                    .append(QByteArray(".", sizeof(".")-1))
                                    .append(QString::number(
                                        static_cast<int>(pReceive.Data[2])))
                                    .append(QByteArray(".", sizeof(".")-1))
                                    .append(
                                        QString::number(static_cast<int>(
                                            pReceive.Data[3]))));
                        HardwareVersionTest.insert("difference", "0");
                        HardwareVersionTest.insert("allowable", "0");
                        HardwareVersionTest.insert("result", "未通过");
                        testResults.insert("HardwareVersionTest",
                                           HardwareVersionTest);

                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                }
            }
        }
        else if(static_cast<int>(pReceive.ID) == id5)
        {
            int a = static_cast<int>(pReceive.Data[0]);
            int b = (static_cast<int>(pReceive.Data[1]) << 8) +
                    static_cast<int>(pReceive.Data[2]);
            emit stopTimerSignal();
            emit signalGetInfo(a, b);
        }
        else if(static_cast<int>(pReceive.ID) == id6)
        {
                if (pReceive.Data[0] == 2)
                {
                    float k = ((static_cast<int>(pReceive.Data[1]) << 8) +
                               static_cast<int>(pReceive.Data[2])) /
                              1000.0;
                    float b = ((static_cast<int>(pReceive.Data[3]) << 8) +
                               static_cast<int>(pReceive.Data[4])) /
                              1000.0;
                    if (b > 65.535 / 2)
                    {
                        b = b - 65.535;
                    }
                    else if (b == 65.535)
                    {
                        b = 0;
                    }
                    emit stopTimerSignal();
                    KBStatus.insert("preChargeK", QString::number(k, 'f', 3));
                    KBStatus.insert("preChargeB", QString::number(b, 'f', 3));
                    updateTestItem();
                }
                else if (pReceive.Data[0] == 3)
                {
                    float k = ((static_cast<int>(pReceive.Data[1]) << 8) +
                               static_cast<int>(pReceive.Data[2])) /
                              1000.0;
                    float b = ((static_cast<int>(pReceive.Data[3]) << 8) +
                               static_cast<int>(pReceive.Data[4])) /
                              1000.0;
                    if (b > 65.535 / 2)
                    {
                        b = b - 65.535;
                    }
                    else if (b == 65.535)
                    {
                        b = 0;
                    }
                    emit stopTimerSignal();
                    KBStatus.insert("endK", QString::number(k, 'f', 3));
                    KBStatus.insert("endB", QString::number(b, 'f', 3));
                    KBStatus.insert("result", "success");
                    testResults.insert("KBStatus", KBStatus);
                    step = 0;
                    emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                }
        }
        else if(static_cast<int>(pReceive.ID) == id7)
        {
            float k = ((static_cast<int>(pReceive.Data[2]) << 8) +
                       static_cast<int>(pReceive.Data[3])) /
                      1000.0;
            float b = ((static_cast<int>(pReceive.Data[4]) << 8) +
                       static_cast<int>(pReceive.Data[5])) /
                      1000.0;
            if (b > 65.535 / 2)
            {
                b = b - 65.535;
            }
            else if (b == 65.535)
            {
                b = 0;
            }
            emit stopTimerSignal();
            emit signalGetKB(k, b);
        }
        else if(static_cast<int>(pReceive.ID) == id8)
        {
            if (lastTestType == TestSoftWareVersion)
            {
                QStringList fullList = m_software.split("-");
                if (fullList.size() == 1)
                {
                    emit stopTimerSignal();
                    if (1 == static_cast<int>(pReceive.Data[0]))
                    {
                        QStringList list = fullList.at(2).split(".");
                        if (list.size() == 3)
                        {
                            if (list.at(0).toInt() ==
                                    static_cast<int>(pReceive.Data[4]) &&
                                list.at(1).toInt() ==
                                    static_cast<int>(pReceive.Data[5]) &&
                                list.at(2).toInt() ==
                                    static_cast<int>(pReceive.Data[6]) &&
                                0x43 ==
                                    static_cast<int>(pReceive.Data[2]) &&
                                3 == static_cast<int>(pReceive.Data[3]))
                            {
                                QJsonObject SoftwareVersionTest;
                                SoftwareVersionTest.insert(
                                    "standard", "SV:C-3-" + m_software);
                                SoftwareVersionTest.insert(
                                    "test",
                                    "SV:C-3-" +
                                        QString::number(
                                            static_cast<int>(
                                                pReceive.Data[4]))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[5])))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[6]))));
                                SoftwareVersionTest.insert("difference",
                                                           "0");
                                SoftwareVersionTest.insert("allowable",
                                                           "0");
                                SoftwareVersionTest.insert("result",
                                                           "通过");
                                testResults.insert("SoftwareVersionTest",
                                                   SoftwareVersionTest);
                                step = 0;
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                QJsonObject SoftwareVersionTest;
                                SoftwareVersionTest.insert(
                                    "standard", "SV:-c-3-" + m_software);
                                SoftwareVersionTest.insert(
                                    "test",
                                    "SV:C-3-" +
                                        QString::number(
                                            static_cast<int>(
                                                pReceive.Data[4]))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[5])))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[6]))));
                                SoftwareVersionTest.insert("difference",
                                                           "0");
                                SoftwareVersionTest.insert("allowable",
                                                           "0");
                                SoftwareVersionTest.insert("result",
                                                           "未通过");
                                testResults.insert("SoftwareVersionTest",
                                                   SoftwareVersionTest);
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                        }
                        else if (list.size() == 4)
                        {

                            if (list.at(0).toInt() ==
                                    static_cast<int>(pReceive.Data[4]) &&
                                list.at(1).toInt() ==
                                    static_cast<int>(pReceive.Data[5]) &&
                                list.at(2).toInt() ==
                                    static_cast<int>(pReceive.Data[6]) &&
                                list.at(3).toInt() ==
                                    static_cast<int>(pReceive.Data[7]) &&
                                0x43 ==
                                    static_cast<int>(pReceive.Data[2]) &&
                                3 == static_cast<int>(pReceive.Data[3]))
                            {
                                QJsonObject SoftwareVersionTest;
                                SoftwareVersionTest.insert(
                                    "standard", "SV:C-3-" + m_software);
                                SoftwareVersionTest.insert(
                                    "test",
                                    "SV:C-3-" +
                                        QString::number(
                                            static_cast<int>(
                                                pReceive.Data[4]))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[5])))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[6])))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[7]))));
                                SoftwareVersionTest.insert("difference",
                                                           "0");
                                SoftwareVersionTest.insert("allowable",
                                                           "0");
                                SoftwareVersionTest.insert("result",
                                                           "通过");
                                testResults.insert("SoftwareVersionTest",
                                                   SoftwareVersionTest);
                                step = 0;
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                QJsonObject SoftwareVersionTest;
                                SoftwareVersionTest.insert(
                                    "standard", "SV:-C-3-" + m_software);
                                SoftwareVersionTest.insert(
                                    "test",
                                    "SV:C-3-" +
                                        QString::number(
                                            static_cast<int>(
                                                pReceive.Data[4]))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[5])))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[6])))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[7]))));
                                SoftwareVersionTest.insert("difference",
                                                           "0");
                                SoftwareVersionTest.insert("allowable",
                                                           "0");
                                SoftwareVersionTest.insert("result",
                                                           "未通过");
                                testResults.insert("SoftwareVersionTest",
                                                   SoftwareVersionTest);
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                        }
                    }
                }
                else if (fullList.size() == 2)
                {
                    if (1 == static_cast<int>(pReceive.Data[0]))
                    {
                        QStringList list = fullList.at(2).split(".");
                        m_tempSoft =
                            "SV:C-3-" +
                            QString::number(
                                static_cast<int>(pReceive.Data[4]))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(QString::number(
                                    static_cast<int>(pReceive.Data[5])))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(QString::number(
                                    static_cast<int>(pReceive.Data[6])))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(QString::number(
                                    static_cast<int>(pReceive.Data[7]))) +
                            "-";
                    }
                    else if (2 == static_cast<int>(pReceive.Data[0]))
                    {
                        emit stopTimerSignal();
                        m_tempSoft =
                            m_tempSoft +
                            QString((
                                char)(static_cast<int>(pReceive.Data[2]))) +
                            QString((
                                char)(static_cast<int>(pReceive.Data[3]))) +
                            QString::number(
                                static_cast<int>(pReceive.Data[4]))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(QString::number(
                                    static_cast<int>(pReceive.Data[5])));
                        if (m_tempSoft == "SV:" + m_software)
                        {
                            QJsonObject SoftwareVersionTest;
                            SoftwareVersionTest.insert(
                                "standard", "SV:C-3-" + m_software);
                            SoftwareVersionTest.insert("test", m_tempSoft);
                            SoftwareVersionTest.insert("difference", "0");
                            SoftwareVersionTest.insert("allowable", "0");
                            SoftwareVersionTest.insert("result", "通过");
                            testResults.insert("SoftwareVersionTest",
                                               SoftwareVersionTest);
                            step = 0;
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            QJsonObject SoftwareVersionTest;
                            SoftwareVersionTest.insert(
                                "standard", "SV:C-3-" + m_software);
                            SoftwareVersionTest.insert("test", m_tempSoft);
                            SoftwareVersionTest.insert("difference", "0");
                            SoftwareVersionTest.insert("allowable", "0");
                            SoftwareVersionTest.insert("result", "未通过");
                            testResults.insert("SoftwareVersionTest",
                                               SoftwareVersionTest);
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                    }
                }
                else if (fullList.size() == 3)
                {
                    if (1 == static_cast<int>(pReceive.Data[0]))
                    {
                        emit stopTimerSignal();
                        QStringList list = fullList.at(2).split(".");
                        if (list.size() == 3)
                        {
                            if (list.at(0).toInt() ==
                                    static_cast<int>(pReceive.Data[4]) &&
                                list.at(1).toInt() ==
                                    static_cast<int>(pReceive.Data[5]) &&
                                list.at(2).toInt() ==
                                    static_cast<int>(pReceive.Data[6]) &&
                                0x43 ==
                                    static_cast<int>(pReceive.Data[2]) &&
                                3 == static_cast<int>(pReceive.Data[3]))
                            {
                                QJsonObject SoftwareVersionTest;
                                SoftwareVersionTest.insert(
                                    "standard", "SV:" + m_software);
                                SoftwareVersionTest.insert(
                                    "test",
                                    "SV:C-3-" +
                                        QString::number(
                                            static_cast<int>(
                                                pReceive.Data[4]))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[5])))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[6]))));
                                SoftwareVersionTest.insert("difference",
                                                           "0");
                                SoftwareVersionTest.insert("allowable",
                                                           "0");
                                SoftwareVersionTest.insert("result",
                                                           "通过");
                                testResults.insert("SoftwareVersionTest",
                                                   SoftwareVersionTest);
                                step = 0;
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                QJsonObject SoftwareVersionTest;
                                SoftwareVersionTest.insert(
                                    "standard", "SV:" + m_software);
                                SoftwareVersionTest.insert(
                                    "test",
                                    "SV:C-3-" +
                                        QString::number(
                                            static_cast<int>(
                                                pReceive.Data[4]))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[5])))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[6]))));
                                SoftwareVersionTest.insert("difference",
                                                           "0");
                                SoftwareVersionTest.insert("allowable",
                                                           "0");
                                SoftwareVersionTest.insert("result",
                                                           "未通过");
                                testResults.insert("SoftwareVersionTest",
                                                   SoftwareVersionTest);
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                        }
                        else if (list.size() == 4)
                        {
                            if (list.at(0).toInt() ==
                                    static_cast<int>(pReceive.Data[4]) &&
                                list.at(1).toInt() ==
                                    static_cast<int>(pReceive.Data[5]) &&
                                list.at(2).toInt() ==
                                    static_cast<int>(pReceive.Data[6]) &&
                                list.at(3).toInt() ==
                                    static_cast<int>(pReceive.Data[7]) &&
                                0x43 ==
                                    static_cast<int>(pReceive.Data[2]) &&
                                3 == static_cast<int>(pReceive.Data[3]))
                            {
                                QJsonObject SoftwareVersionTest;
                                SoftwareVersionTest.insert(
                                    "standard", "SV:" + m_software);
                                SoftwareVersionTest.insert(
                                    "test",
                                    "SV:C-3-" +
                                        QString::number(
                                            static_cast<int>(
                                                pReceive.Data[4]))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[5])))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[6])))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[7]))));
                                SoftwareVersionTest.insert("difference",
                                                           "0");
                                SoftwareVersionTest.insert("allowable",
                                                           "0");
                                SoftwareVersionTest.insert("result",
                                                           "通过");
                                testResults.insert("SoftwareVersionTest",
                                                   SoftwareVersionTest);
                                step = 0;
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                QJsonObject SoftwareVersionTest;
                                SoftwareVersionTest.insert(
                                    "standard", "SV:" + m_software);
                                SoftwareVersionTest.insert(
                                    "test",
                                    "SV:C-3-" +
                                        QString::number(
                                            static_cast<int>(
                                                pReceive.Data[4]))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[5])))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[6])))
                                            .append(QByteArray(".", sizeof(".")-1))
                                            .append(QString::number(
                                                static_cast<int>(
                                                    pReceive.Data[7]))));
                                SoftwareVersionTest.insert("difference",
                                                           "0");
                                SoftwareVersionTest.insert("allowable",
                                                           "0");
                                SoftwareVersionTest.insert("result",
                                                           "未通过");
                                testResults.insert("SoftwareVersionTest",
                                                   SoftwareVersionTest);
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                        }
                    }
                }
                else if (fullList.size() == 4)
                {
                    if (1 == static_cast<int>(pReceive.Data[0]))
                    {
                        QStringList list = fullList.at(2).split(".");
                        m_tempSoft =
                            "SV:C-3-" +
                            QString::number(
                                static_cast<int>(pReceive.Data[4]))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(QString::number(
                                    static_cast<int>(pReceive.Data[5])))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(QString::number(
                                    static_cast<int>(pReceive.Data[6])))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(QString::number(
                                    static_cast<int>(pReceive.Data[7]))) +
                            "-";
                    }
                    else if (2 == static_cast<int>(pReceive.Data[0]))
                    {
                        emit stopTimerSignal();
                        m_tempSoft =
                            m_tempSoft +
                            QString((
                                char)(static_cast<int>(pReceive.Data[2]))) +
                            QString((
                                char)(static_cast<int>(pReceive.Data[3]))) +
                            QString::number(
                                static_cast<int>(pReceive.Data[4]))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(QString::number(
                                    static_cast<int>(pReceive.Data[5])));
                        if (m_tempSoft == "SV:" + m_software)
                        {
                            QJsonObject SoftwareVersionTest;
                            SoftwareVersionTest.insert("standard",
                                                       "SV:" + m_software);
                            SoftwareVersionTest.insert("test", m_tempSoft);
                            SoftwareVersionTest.insert("difference", "0");
                            SoftwareVersionTest.insert("allowable", "0");
                            SoftwareVersionTest.insert("result", "通过");
                            testResults.insert("SoftwareVersionTest",
                                               SoftwareVersionTest);
                            step = 0;
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            QJsonObject SoftwareVersionTest;
                            SoftwareVersionTest.insert("standard",
                                                       "SV:" + m_software);
                            SoftwareVersionTest.insert("test", m_tempSoft);
                            SoftwareVersionTest.insert("difference", "0");
                            SoftwareVersionTest.insert("allowable", "0");
                            SoftwareVersionTest.insert("result", "未通过");
                            testResults.insert("SoftwareVersionTest",
                                               SoftwareVersionTest);
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                    }
                }
            }
        }
        else if(static_cast<int>(pReceive.ID) == id11)
        {
            if (lastTestType == TestRTC || lastTestType == TestSetRTC)
            {
                emit stopTimerSignal();
                QString year =
                    QDateTime::currentDateTime().toString("yyyy");
                QString mon  = QDateTime::currentDateTime().toString("MM");
                QString date = QDateTime::currentDateTime().toString("dd");
                QString hour = QDateTime::currentDateTime().toString("hh");
                QString min  = QDateTime::currentDateTime().toString("mm");
                QString sec  = QDateTime::currentDateTime().toString("ss");
                int year1    = (static_cast<int>(pReceive.Data[0]) << 8) +
                            static_cast<int>(pReceive.Data[1]);
                int mon1  = static_cast<int>(pReceive.Data[2]);
                int date1 = static_cast<int>(pReceive.Data[3]);
                int hour1 = static_cast<int>(pReceive.Data[4]);
                int min1  = static_cast<int>(pReceive.Data[5]);
                int sec1  = static_cast<int>(pReceive.Data[6]);
                QDate testDate(year1, mon1, date1);
                QTime testTime(hour1, min1, sec1);
                QDateTime testDateTime(testDate, testTime);
                QSettings setting(QDir::currentPath() + "/version.ini",
                                  QSettings::IniFormat);
                int wc  = setting.value("RTC").toString().toInt();
                if (testDateTime.secsTo(QDateTime::currentDateTime()) <= wc)
                {
                    step = 0;
                    QJsonObject RTC;
                    RTC.insert("test", testDateTime.toString(
                                           "yyyy-MM-dd hh:mm:ss"));
                    RTC.insert("standard", year + "-" + mon + "-" + date +
                                               " " + hour + ":" + min +
                                               ":" + sec);
                    RTC.insert("difference",
                               "±" + QString::number(testDateTime.secsTo(
                                   QDateTime::currentDateTime())));
                    RTC.insert("allowable", "<=" + QString::number(wc) + "秒");
                    RTC.insert("result", "通过");
                    testResults.insert("RTC", RTC);
                    step = 0;
                    emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                }
                else
                {
                    emit signalReceiveStatus("RTC读取错误");
                    QJsonObject RTC;
                    RTC.insert("standard", year + "-" + mon + "-" + date +
                                               " " + hour + ":" + min +
                                               ":" + sec);
                    RTC.insert("test", testDateTime.toString(
                                           "yyyy-MM-dd hh:mm:ss"));
                    RTC.insert("difference",
                               "±" + QString::number(testDateTime.secsTo(
                                   QDateTime::currentDateTime())));
                    RTC.insert("allowable", "<=" + QString::number(wc) + "秒");
                    RTC.insert("result", "未通过");
                    testResults.insert("RTC", RTC);
                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    step = 0;
                }
            }
        }
        else if(static_cast<int>(pReceive.ID) == id13)
        {
            switch (lastTestType)
            {
            case TestSetRTC:
            {
                emit startFailTimerSignal(20000);
                unsigned char data[8];
                data[0] = 0x12;
                data[1] = 0;
                data[2] = 0;
                data[3] = 0;
                data[4] = 0;
                data[5] = 0;
                data[6] = 0;
                data[7] = 0;
                TransmitCAN(((0x181F << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
                emit startTimerSignal(1000);
                emit signalReceiveStatus("RTC读取中。。。");
                break;
            }
            case TestSetBatteryVoltage:
            {
                emit stopTimerSignal();
                setFailTimer->stop();
                switch(step)
                {
                case 1:
                {
                    step = 2;
                    slotSetTestMod();
                    break;
                }
                case 2:
                {
                    QByteArray temp;
                    switch(m_locate)
                    {
                    case 3:
                    {
                        temp = QByteArray("\x01\x10\x10\x04\x00\x01\x02\x00\x01", 9);
                            Rs485Frame req;
                        req.sendData = temp;
                        req.sendTick = QDateTime::currentMSecsSinceEpoch();

                        // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                        bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                        if (!ok)
                        {
                            qWarning() << "485发送缓冲区已满";
                        }
                        break;
                    }
                    case 7:
                    {
                        temp = QByteArray("\x05\x10\x10\x04\x00\x01\x02\x00\x01", 9);
                            Rs485Frame req;
                        req.sendData = temp;
                        req.sendTick = QDateTime::currentMSecsSinceEpoch();

                        // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                        bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                        if (!ok)
                        {
                            qWarning() << "485发送缓冲区已满";
                        }
                        break;
                    }
                    case 11:
                    {
                        temp = QByteArray("\x09\x10\x10\x04\x00\x01\x02\x00\x01", 9);
                            Rs485Frame req;
                        req.sendData = temp;
                        req.sendTick = QDateTime::currentMSecsSinceEpoch();

                        // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                        bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                        if (!ok)
                        {
                            qWarning() << "485发送缓冲区已满";
                        }
                        break;
                    }
                    case 15:
                    {
                        temp = QByteArray("\x0D\x10\x10\x04\x00\x01\x02\x00\x01", 9);
                            Rs485Frame req;
                        req.sendData = temp;
                        req.sendTick = QDateTime::currentMSecsSinceEpoch();

                        // 写入全局Tx缓冲，串口控制器线程自动轮询发送
                        bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
                        if (!ok)
                        {
                            qWarning() << "485发送缓冲区已满";
                        }
                        break;
                    }
                    default:
                        break;
                    }
                    emit signalReceiveStatus("电池总压测试中。。。");
                    emit signalReceiveStatus("高压源开启中。。。");
                    QTimer *tempTimer = new QTimer(this);
                    connect(tempTimer, SIGNAL(timeout()), this,
                            SLOT(slotSetGYY()));
                    tempTimer->setSingleShot(true);
                    tempTimer->start(2000);
                    qDebug()  << " step at " << step << " finish send batteryVoltage";
                    break;
                }
                case 3:
                {
                    m_dcvz1 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    slotCheckIfUpdate();
                    break;
                }
                case 5:
                {
                    m_dcvy1 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    slotCheckIfUpdate();
                    break;
                }
                case 8:
                {
                    m_dcvz2 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    slotCheckIfUpdate();
                    break;
                }
                case 10:
                {
                    m_dcvy2 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    slotCheckIfUpdate();
                    break;
                }
                case 11:
                {
                    break;
                }
                case 14:
                {
                    m_dcvz3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    if(2 < abs(m_dcvz3 - m_gzvz3))
                    {
                        QJsonObject BatteryVoltage;
                        BatteryVoltage.insert(
                            "standard",
                            QString::number(m_gzvz3, 'f', 1) + "V");
                        BatteryVoltage.insert(
                            "test",
                            QString::number(m_dcvz3, 'f', 1) + "V");
                        BatteryVoltage.insert(
                            "difference",
                            QString::number((m_dcvz3 - m_gzvz3), 'f',
                                            1) +
                                "V");
                        BatteryVoltage.insert("allowable", "±2.0V");
                        BatteryVoltage.insert("result", "未通过");
                        testResults.insert("BatteryVoltage",
                                           BatteryVoltage);
                        emit signalReceiveStatus(
                            "电压偏差过大！要求为±2V，实际为" +
                            QString::number(m_dcvz3 - m_gzvz3) + "V");
                        ifVPass = false;
                    }
                    else
                    {
                        QJsonObject BatteryVoltage;
                        BatteryVoltage.insert(
                            "standard",
                            QString::number(m_gzvz3, 'f', 1) + "V");
                        BatteryVoltage.insert(
                            "test",
                            QString::number(m_dcvz3, 'f', 1) + "V");
                        BatteryVoltage.insert(
                            "difference",
                            QString::number((m_dcvz3 - m_gzvz3), 'f',
                                            1) +
                                "V");
                        BatteryVoltage.insert("allowable", "±2.0V");
                        BatteryVoltage.insert("result", "通过");
                        testResults.insert("BatteryVoltage",
                                           BatteryVoltage);
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 16:
                {
                    m_dcvy3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    if (m_type != 21 && m_type != 22 && m_type != 23 &&
                                                    m_type != 34 && m_type != 35)
                    {
                        if(2 < abs(m_dcvy3 - m_gzvy3))
                        {
                            QJsonObject PrechargeVoltage;
                            PrechargeVoltage.insert(
                                "standard",
                                QString::number(m_gzvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "test",
                                QString::number(m_dcvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "difference",
                                QString::number((m_dcvy3 - m_gzvy3), 'f',
                                                1) +
                                    "V");
                            PrechargeVoltage.insert("allowable", "±2.0V");
                            PrechargeVoltage.insert("result", "未通过");
                            testResults.insert("PrechargeVoltage",
                                               PrechargeVoltage);

                            emit signalReceiveStatus(
                                "电压偏差过大！要求为±2V，实际为" +
                                QString::number(m_dcvy3 - m_gzvy3) + "V");
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            step = 0;
                        }
                        else
                        {
                            QJsonObject PrechargeVoltage;
                            PrechargeVoltage.insert(
                                "standard",
                                QString::number(m_gzvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "test",
                                QString::number(m_dcvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "difference",
                                QString::number((m_dcvy3 - m_gzvy3), 'f',
                                                1) +
                                    "V");
                            PrechargeVoltage.insert("allowable", "±2.0V");
                            PrechargeVoltage.insert("result", "通过");
                            testResults.insert("PrechargeVoltage",
                                               PrechargeVoltage);
                            if(ifVPass)
                            {
                                step = 0;
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                step = 0;
                            }
                            else
                            {
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                step = 0;
                            }
                        }
                    }
                    else if (m_type != 30 && m_type != 31 &&
                             m_type != 32)
                    {
                        if(3 < abs(m_dcvy3 - m_gzvy3))
                        {
                            QJsonObject PrechargeVoltage;
                            PrechargeVoltage.insert(
                                "standard",
                                QString::number(m_gzvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "test",
                                QString::number(m_dcvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "difference",
                                QString::number((m_dcvy3 - m_gzvy3), 'f',
                                                1) +
                                    "V");
                            PrechargeVoltage.insert("allowable", "±3.0V");
                            PrechargeVoltage.insert("result", "未通过");
                            testResults.insert("PrechargeVoltage",
                                               PrechargeVoltage);

                            emit signalReceiveStatus(
                                "电压偏差过大！要求为±3V，实际为" +
                                QString::number(m_dcvy3 - m_gzvy3) + "V");
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            step = 0;
                        }
                        else
                        {
                            QJsonObject PrechargeVoltage;
                            PrechargeVoltage.insert(
                                "standard",
                                QString::number(m_gzvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "test",
                                QString::number(m_dcvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "difference",
                                QString::number((m_dcvy3 - m_gzvy3), 'f',
                                                1) +
                                    "V");
                            PrechargeVoltage.insert("allowable", "±3.0V");
                            PrechargeVoltage.insert("result", "通过");
                            testResults.insert("PrechargeVoltage",
                                               PrechargeVoltage);
                            if(ifVPass)
                            {
                                step = 0;
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                step = 0;
                            }
                            else
                            {
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                step = 0;
                            }
                        }
                    }
                    else
                    {
                        if(4 < abs(m_dcvy3 - m_gzvy3))
                        {
                            QJsonObject PrechargeVoltage;
                            PrechargeVoltage.insert(
                                "standard",
                                QString::number(m_gzvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "test",
                                QString::number(m_dcvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "difference",
                                QString::number((m_dcvy3 - m_gzvy3), 'f',
                                                1) +
                                    "V");
                            PrechargeVoltage.insert("allowable", "±4.0V");
                            PrechargeVoltage.insert("result", "未通过");
                            testResults.insert("PrechargeVoltage",
                                               PrechargeVoltage);

                            emit signalReceiveStatus(
                                "电压偏差过大！要求为±4V，实际为" +
                                QString::number(m_dcvy3 - m_gzvy3) + "V");
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            step = 0;
                        }
                        else
                        {
                            QJsonObject PrechargeVoltage;
                            PrechargeVoltage.insert(
                                "standard",
                                QString::number(m_gzvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "test",
                                QString::number(m_dcvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "difference",
                                QString::number((m_dcvy3 - m_gzvy3), 'f',
                                                1) +
                                    "V");
                            PrechargeVoltage.insert("allowable", "±4.0V");
                            PrechargeVoltage.insert("result", "通过");
                            testResults.insert("PrechargeVoltage",
                                               PrechargeVoltage);
                            if(ifVPass)
                            {
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                step = 0;
                            }
                            else
                            {
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                step = 0;
                            }
                        }
                    }
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case TestZDResistance:
            {
                emit stopTimerSignal();
                setFailTimer->stop();
                switch(step)
                {
                case 5:
                {
                    step = 6;
                    slotSetTestMod();
                    break;
                }
                case 6:
                {
                    if(if_zdPass)
                    {
                        step = 0;
                        emit stopFailTimerSignal();step = 0;
                        emit signalFinish();timeoutTimer.stop();
                    }
                    else
                    {
                        step = 0;
                        emit stopFailTimerSignal();step = 0;
                        emit signalErrorFinish();timeoutTimer.stop();
                    }
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case TestBatteryVoltage:
            {
                emit stopTimerSignal();
                switch (step)
                {
                case 19:
                {
                    m_dcvz3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    if (m_type != 21 && m_type != 22 && m_type != 23 &&
                                                    m_type != 34 && m_type != 35)
                    {
                        if(2 < abs(m_dcvz3 - m_gzvz3))
                        {
                            QJsonObject BatteryVoltage;
                            BatteryVoltage.insert(
                                "standard",
                                QString::number(m_gzvz3, 'f', 1) + "V");
                            BatteryVoltage.insert(
                                "test",
                                QString::number(m_dcvz3, 'f', 1) + "V");
                            BatteryVoltage.insert(
                                "difference",
                                QString::number((m_dcvz3 - m_gzvz3), 'f',
                                                1) +
                                    "V");
                            BatteryVoltage.insert("allowable", "±2.0V");
                            BatteryVoltage.insert("result", "未通过");
                            testResults.insert("BatteryVoltage",
                                               BatteryVoltage);

                            emit signalReceiveStatus(
                                "电压偏差过大！要求为±2V，实际为" +
                                QString::number(m_dcvz3 - m_gzvz3) + "V");
                            ifVPass = false;
                        }
                        else
                        {
                            QJsonObject BatteryVoltage;
                            BatteryVoltage.insert(
                                "standard",
                                QString::number(m_gzvz3, 'f', 1) + "V");
                            BatteryVoltage.insert(
                                "test",
                                QString::number(m_dcvz3, 'f', 1) + "V");
                            BatteryVoltage.insert(
                                "difference",
                                QString::number((m_dcvz3 - m_gzvz3), 'f',
                                                1) +
                                    "V");
                            BatteryVoltage.insert("allowable", "±2.0V");
                            BatteryVoltage.insert("result", "通过");
                            testResults.insert("BatteryVoltage",
                                               BatteryVoltage);
                        }
                    }
                    else if (m_type != 30 && m_type != 31 &&
                             m_type != 32)
                    {
                        if(3 < abs(m_dcvz3 - m_gzvz3))
                        {
                            QJsonObject BatteryVoltage;
                            BatteryVoltage.insert(
                                "standard",
                                QString::number(m_gzvz3, 'f', 1) + "V");
                            BatteryVoltage.insert(
                                "test",
                                QString::number(m_dcvz3, 'f', 1) + "V");
                            BatteryVoltage.insert(
                                "difference",
                                QString::number((m_dcvz3 - m_gzvz3), 'f',
                                                1) +
                                    "V");
                            BatteryVoltage.insert("allowable", "±3.0V");
                            BatteryVoltage.insert("result", "未通过");
                            testResults.insert("BatteryVoltage",
                                               BatteryVoltage);

                            emit signalReceiveStatus(
                                "电压偏差过大！要求为±3V，实际为" +
                                QString::number(m_dcvz3 - m_gzvz3) + "V");
                            ifVPass = false;
                        }
                        else
                        {
                            QJsonObject BatteryVoltage;
                            BatteryVoltage.insert(
                                "standard",
                                QString::number(m_gzvz3, 'f', 1) + "V");
                            BatteryVoltage.insert(
                                "test",
                                QString::number(m_dcvz3, 'f', 1) + "V");
                            BatteryVoltage.insert(
                                "difference",
                                QString::number((m_dcvz3 - m_gzvz3), 'f',
                                                1) +
                                    "V");
                            BatteryVoltage.insert("allowable", "±3.0V");
                            BatteryVoltage.insert("result", "通过");
                            testResults.insert("BatteryVoltage",
                                               BatteryVoltage);
                        }
                    }
                    else
                    {
                        if(4 < abs(m_dcvz3 - m_gzvz3))
                        {
                            QJsonObject BatteryVoltage;
                            BatteryVoltage.insert(
                                "standard",
                                QString::number(m_gzvz3, 'f', 1) + "V");
                            BatteryVoltage.insert(
                                "test",
                                QString::number(m_dcvz3, 'f', 1) + "V");
                            BatteryVoltage.insert(
                                "difference",
                                QString::number((m_dcvz3 - m_gzvz3), 'f',
                                                1) +
                                    "V");
                            BatteryVoltage.insert("allowable", "±4.0V");
                            BatteryVoltage.insert("result", "未通过");
                            testResults.insert("BatteryVoltage",
                                               BatteryVoltage);

                            emit signalReceiveStatus(
                                "电压偏差过大！要求为±4V，实际为" +
                                QString::number(m_dcvz3 - m_gzvz3) + "V");
                            ifVPass = false;
                        }
                        else
                        {
                            QJsonObject BatteryVoltage;
                            BatteryVoltage.insert(
                                "standard",
                                QString::number(m_gzvz3, 'f', 1) + "V");
                            BatteryVoltage.insert(
                                "test",
                                QString::number(m_dcvz3, 'f', 1) + "V");
                            BatteryVoltage.insert(
                                "difference",
                                QString::number((m_dcvz3 - m_gzvz3), 'f',
                                                1) +
                                    "V");
                            BatteryVoltage.insert("allowable", "±4.0V");
                            BatteryVoltage.insert("result", "通过");
                            testResults.insert("BatteryVoltage",
                                               BatteryVoltage);
                        }
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 21:
                {
                    m_dcvy3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    if (m_type != 21 && m_type != 22 && m_type != 23 &&
                                                    m_type != 34 && m_type != 35)
                    {
                        if(2 < abs(m_dcvy3 - m_gzvy3))
                        {
                            QJsonObject PrechargeVoltage;
                            PrechargeVoltage.insert(
                                "standard",
                                QString::number(m_gzvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "test",
                                QString::number(m_dcvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "difference",
                                QString::number((m_dcvy3 - m_gzvy3), 'f',
                                                1) +
                                    "V");
                            PrechargeVoltage.insert("allowable", "±2.0V");
                            PrechargeVoltage.insert("result", "未通过");
                            testResults.insert("PrechargeVoltage",
                                               PrechargeVoltage);

                            emit signalReceiveStatus(
                                "电压偏差过大！要求为±2V，实际为" +
                                QString::number(m_dcvy3 - m_gzvy3) + "V");
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            step = 0;
                        }
                        else
                        {
                            QJsonObject PrechargeVoltage;
                            PrechargeVoltage.insert(
                                "standard",
                                QString::number(m_gzvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "test",
                                QString::number(m_dcvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "difference",
                                QString::number((m_dcvy3 - m_gzvy3), 'f',
                                                1) +
                                    "V");
                            PrechargeVoltage.insert("allowable", "±2.0V");
                            PrechargeVoltage.insert("result", "通过");
                            testResults.insert("PrechargeVoltage",
                                               PrechargeVoltage);
                            if(ifVPass)
                            {
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                step = 0;
                            }
                            else
                            {
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                step = 0;
                            }
                        }
                    }
                    else if (m_type != 30 && m_type != 31 &&
                             m_type != 32)
                    {
                        if(3 < abs(m_dcvy3 - m_gzvy3))
                        {
                            QJsonObject PrechargeVoltage;
                            PrechargeVoltage.insert(
                                "standard",
                                QString::number(m_gzvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "test",
                                QString::number(m_dcvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "difference",
                                QString::number((m_dcvy3 - m_gzvy3), 'f',
                                                1) +
                                    "V");
                            PrechargeVoltage.insert("allowable", "±3.0V");
                            PrechargeVoltage.insert("result", "未通过");
                            testResults.insert("PrechargeVoltage",
                                               PrechargeVoltage);

                            emit signalReceiveStatus(
                                "电压偏差过大！要求为±3V，实际为" +
                                QString::number(m_dcvy3 - m_gzvy3) + "V");
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            step = 0;
                        }
                        else
                        {
                            QJsonObject PrechargeVoltage;
                            PrechargeVoltage.insert(
                                "standard",
                                QString::number(m_gzvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "test",
                                QString::number(m_dcvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "difference",
                                QString::number((m_dcvy3 - m_gzvy3), 'f',
                                                1) +
                                    "V");
                            PrechargeVoltage.insert("allowable", "±3.0V");
                            PrechargeVoltage.insert("result", "通过");
                            testResults.insert("PrechargeVoltage",
                                               PrechargeVoltage);
                            if(ifVPass)
                            {
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                step = 0;
                            }
                            else
                            {
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                step = 0;
                            }
                        }
                    }
                    else
                    {
                        if(4 < abs(m_dcvy3 - m_gzvy3))
                        {
                            QJsonObject PrechargeVoltage;
                            PrechargeVoltage.insert(
                                "standard",
                                QString::number(m_gzvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "test",
                                QString::number(m_dcvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "difference",
                                QString::number((m_dcvy3 - m_gzvy3), 'f',
                                                1) +
                                    "V");
                            PrechargeVoltage.insert("allowable", "±4.0V");
                            PrechargeVoltage.insert("result", "未通过");
                            testResults.insert("PrechargeVoltage",
                                               PrechargeVoltage);

                            emit signalReceiveStatus(
                                "电压偏差过大！要求为±4V，实际为" +
                                QString::number(m_dcvy3 - m_gzvy3) + "V");
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            step = 0;
                        }
                        else
                        {
                            QJsonObject PrechargeVoltage;
                            PrechargeVoltage.insert(
                                "standard",
                                QString::number(m_gzvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "test",
                                QString::number(m_dcvy3, 'f', 1) + "V");
                            PrechargeVoltage.insert(
                                "difference",
                                QString::number((m_dcvy3 - m_gzvy3), 'f',
                                                1) +
                                    "V");
                            PrechargeVoltage.insert("allowable", "±4.0V");
                            PrechargeVoltage.insert("result", "通过");
                            testResults.insert("PrechargeVoltage",
                                               PrechargeVoltage);
                            if(ifVPass)
                            {
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                step = 0;
                            }
                            else
                            {
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                step = 0;
                            }
                        }
                    }
                    break;
                }
                default:
                    break;
                }
                    break;
            }
            case TestSupplyVoltage:
            {
                emit stopTimerSignal();
                if(pReceive.Data[0] == 0x0C)
                {
                    m_dcgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    updateTestItem();
                }
                break;
            }
            case TestInsulationResistance:
            {
                emit stopTimerSignal();
                QByteArray temp;
                m_dcjyf = ((static_cast<int>(pReceive.Data[1]) << 8) +
                        static_cast<int>(pReceive.Data[2]));
                m_dcjyz = ((static_cast<int>(pReceive.Data[3]) << 8) +
                        static_cast<int>(pReceive.Data[4]));
                if (1150 >= m_dcjyz && 850 <= m_dcjyz &&
                        575 >= m_dcjyf && 425 <= m_dcjyf)
                {
                    step = 0;
                    QJsonObject InsulationResistance;
                    InsulationResistance.insert("standard",
                                                        "1000kΩ/500kΩ");
                    InsulationResistance.insert(
                        "test", "正线" + QString::number(m_dcjyz) +
                                    "kΩ/负线" +
                                    QString::number(m_dcjyf) + "kΩ");
                    InsulationResistance.insert(
                        "difference",
                        "正线" + QString::number(m_dcjyz - 1000) +
                            "kΩ/负线" +
                            QString::number(m_dcjyf - 500) + "kΩ");
                    InsulationResistance.insert(
                        "allowable", "正线大于850kΩ小于1150kΩ/负线大于425kΩ小于575kΩ");
                    InsulationResistance.insert("result", "通过");
                    testResults.insert("InsulationResistance",
                                       InsulationResistance);
                    switch(m_locate)
                    {
                    case 3:
                    {
                        temp = QByteArray("\x01\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                        break;
                    }
                    case 7:
                    {
                        temp = QByteArray("\x05\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                        break;
                    }
                    case 11:
                    {
                        temp = QByteArray("\x09\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                        break;
                    }
                    case 15:
                    {
                        temp = QByteArray("\x0D\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                        break;
                    }
                    default:
                        break;
                    }
                    step = 0;
                    emit stopFailTimerSignal();
                    emit signalFinish();timeoutTimer.stop();
                }
                else
                {
                    step = 0;
                    QJsonObject InsulationResistance;
                    InsulationResistance.insert("standard",
                                                "1000kΩ/500kΩ");
                    InsulationResistance.insert(
                        "test", "正线" + QString::number(m_dcjyz) +
                                    "kΩ/负线" +
                                    QString::number(m_dcjyf) + "kΩ");
                    InsulationResistance.insert(
                        "difference",
                        "正线" + QString::number(m_dcjyz - 1000) +
                            "kΩ/负线" +
                            QString::number(m_dcjyf - 500) + "kΩ");
                    InsulationResistance.insert(
                        "allowable", "正线大于850kΩ小于1150kΩ/负线大于425kΩ小于575kΩ");
                    InsulationResistance.insert("result", "未通过");
                    testResults.insert("InsulationResistance",
                                       InsulationResistance);
                    switch(m_locate)
                    {
                    case 3:
                    {
                        temp = QByteArray("\x01\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                        break;
                    }
                    case 7:
                    {
                        temp = QByteArray("\x05\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                        break;
                    }
                    case 11:
                    {
                        temp = QByteArray("\x09\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                        break;
                    }
                    case 15:
                    {
                        temp = QByteArray("\x0D\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                        break;
                    }
                    default:
                        break;
                    }
                    emit stopFailTimerSignal();
                    emit signalErrorFinish();timeoutTimer.stop();
                }
                break;
            }
            case TestTemperature:
            {
                emit stopTimerSignal();
                switch(step)
                {
                case 1:
                {
                    if((static_cast<int>(pReceive.Data[3]) << 8) +
                            static_cast<int>(pReceive.Data[4]) > 32767)
                    {
                        m_temp1 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                        static_cast<int>(pReceive.Data[4])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_temp1 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                        static_cast<int>(pReceive.Data[4]))) / 10.0;
                    }
                    if(m_temp1 < 10)
                    {
                        m_temp1 = static_cast<int>(pReceive.Data[4]) - 40;
                    }
                    updateTestItem();
                    break;
                }
                case 2:
                {
                    if((static_cast<int>(pReceive.Data[3]) << 8) +
                            static_cast<int>(pReceive.Data[4]) > 32767)
                    {
                        m_temp2 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                        static_cast<int>(pReceive.Data[4])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_temp2 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                        static_cast<int>(pReceive.Data[4]))) / 10.0;
                    }
                    if(m_temp2 < 10)
                    {
                        m_temp2 = static_cast<int>(pReceive.Data[4]) - 40;
                    }
                    updateTestItem();
                    break;
                }
                case 3:
                {
                    if((static_cast<int>(pReceive.Data[3]) << 8) +
                            static_cast<int>(pReceive.Data[4]) > 32767)
                    {
                        m_temp3 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                        static_cast<int>(pReceive.Data[4])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_temp3 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                        static_cast<int>(pReceive.Data[4]))) / 10.0;
                    }
                    if(m_temp3 < 10)
                    {
                        m_temp3 = static_cast<int>(pReceive.Data[4]) - 40;
                    }
                    updateTestItem();
                    break;
                }
                case 4:
                {
                        if((static_cast<int>(pReceive.Data[3]) << 8) +
                                static_cast<int>(pReceive.Data[4]) > 32767)
                        {
                            m_temp4 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                            static_cast<int>(pReceive.Data[4])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_temp4 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                            static_cast<int>(pReceive.Data[4]))) / 10.0;
                        }
                        if(m_temp4 < 10)
                        {
                            m_temp4 = static_cast<int>(pReceive.Data[4]) - 40;
                        }
                        if (m_type != 15 && m_type != 16 && m_type != 17)
                        {
                            QJsonObject Temperature;
                            QJsonObject One;
                            One.insert("standard", "25℃");
                            One.insert(
                                "test",
                                QString::number(m_temp1) +
                                    "℃");
                            One.insert(
                                "difference",
                                QString::number(25 -m_temp1) +
                                    "℃");
                            One.insert("allowable", "±1℃");
                            if (1 >= abs(25 - m_temp1))
                            {
                                One.insert("result", "通过");
                            }
                            else
                            {
                                One.insert("result", "未通过");
                            }
                            Temperature.insert("One", One);
                            QJsonObject Two;
                            Two.insert("standard", "54℃");
                            Two.insert(
                                "test",
                                QString::number(m_temp2) +
                                    "℃");
                            Two.insert(
                                "difference",
                                QString::number(
                                    54 - m_temp2) +
                                    "℃");
                            Two.insert("allowable", "±1℃");
                            if (1 >= abs(54 - m_temp2))
                            {
                                Two.insert("result", "通过");
                            }
                            else
                            {
                                Two.insert("result", "未通过");
                            }
                            Temperature.insert("Two", Two);
                            QJsonObject Three;
                            Three.insert("standard", "25℃");
                            Three.insert(
                                "test",
                                QString::number(m_temp3) +
                                    "℃");
                            Three.insert(
                                "difference",
                                QString::number(
                                    25 - m_temp3) +
                                    "℃");
                            Three.insert("allowable", "±1℃");
                            if (1 >= abs(25 - m_temp3))
                            {
                                Three.insert("result", "通过");
                            }
                            else
                            {
                                Three.insert("result", "未通过");
                            }
                            Temperature.insert("Three", Three);
                            QJsonObject Four;
                            Four.insert("standard", "54℃");
                            Four.insert(
                                "test",
                                QString::number(m_temp4) +
                                    "℃");
                            Four.insert(
                                "difference",
                                QString::number(54 - m_temp4) +
                                    "℃");
                            Four.insert("allowable", "±1℃");
                            if (1 >=
                                abs(54 - m_temp4))
                            {
                                Four.insert("result", "通过");
                            }
                            else
                            {
                                Four.insert("result", "未通过");
                            }
                            Temperature.insert("Four", Four);
                            testResults.insert("Temperature", Temperature);
                            if (1 >= abs(25 - m_temp1) &&
                                1 >= abs(54 - m_temp2) &&
                                1 >= abs(25 - m_temp3) &&
                                1 >= abs(54 - m_temp4))
                            {
                                step = 0;
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                step = 0;
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                        }
                        else
                        {
                            QJsonObject Temperature;
                            QJsonObject One;
                            One.insert("standard", "35℃");
                            One.insert(
                                "test",
                                QString::number(m_temp1) +
                                    "℃");
                            One.insert(
                                "difference",
                                QString::number(35 -m_temp1) +
                                    "℃");
                            One.insert("allowable", "±1℃");
                            if (1 >= abs(35 - m_temp1))
                            {
                                One.insert("result", "通过");
                            }
                            else
                            {
                                One.insert("result", "未通过");
                            }
                            Temperature.insert("One", One);
                            QJsonObject Two;
                            Two.insert("standard", "65℃");
                            Two.insert(
                                "test",
                                QString::number(m_temp2) +
                                    "℃");
                            Two.insert(
                                "difference",
                                QString::number(
                                    65 - m_temp2) +
                                    "℃");
                            Two.insert("allowable", "±1℃");
                            if (1 >= abs(65 - m_temp2))
                            {
                                Two.insert("result", "通过");
                            }
                            else
                            {
                                Two.insert("result", "未通过");
                            }
                            Temperature.insert("Two", Two);
                            QJsonObject Three;
                            Three.insert("standard", "35℃");
                            Three.insert(
                                "test",
                                QString::number(m_temp3) +
                                    "℃");
                            Three.insert(
                                "difference",
                                QString::number(
                                    35 - m_temp3) +
                                    "℃");
                            Three.insert("allowable", "±1℃");
                            if (1 >= abs(35 - m_temp3))
                            {
                                Three.insert("result", "通过");
                            }
                            else
                            {
                                Three.insert("result", "未通过");
                            }
                            Temperature.insert("Three", Three);
                            QJsonObject Four;
                            Four.insert("standard", "65℃");
                            Four.insert(
                                "test",
                                QString::number(m_temp4) +
                                    "℃");
                            Four.insert(
                                "difference",
                                QString::number(65 - m_temp4) +
                                    "℃");
                            Four.insert("allowable", "±1℃");
                            if (1 >=
                                abs(65 - m_temp4))
                            {
                                Four.insert("result", "通过");
                            }
                            else
                            {
                                Four.insert("result", "未通过");
                            }
                            Temperature.insert("Four", Four);
                            testResults.insert("Temperature", Temperature);
                            if (1 >= abs(35 - m_temp1) &&
                                1 >= abs(65 - m_temp2) &&
                                1 >= abs(35 - m_temp3) &&
                                1 >= abs(65 - m_temp4))
                            {
                                step = 0;
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                step = 0;
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                        }
                        break;
                }
                default:
                    break;
                }
                break;
            }
            case TestEncodeInterface:
            {
                    emit stopTimerSignal();
                    switch(step)
                    {
                    case 1:
                    {
                        updateTestItem();
                        break;
                    }
                    case 2:
                    {
                        updateTestItem();
                        break;
                    }
                    case 3:
                    {
                        updateTestItem();
                        break;
                    }
                    case 4:
                    {
                        if(static_cast<int>(pReceive.Data[4]) == 1)
                        {
                            updateTestItem();
                        }
                        else
                        {
                            QJsonObject EncodeInterface;
                            EncodeInterface.insert("standard", "0");
                            EncodeInterface.insert("test", "1");
                            EncodeInterface.insert("difference", "1");
                            EncodeInterface.insert("allowable", "0");
                            EncodeInterface.insert("result", "未通过");
                            testResults.insert("EncodeInterface",
                                               EncodeInterface);
                            step = 0;
                            emit signalReceiveStatus("设置高电平失败");
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        break;
                    }
                    case 5:
                    {
                        updateTestItem();
                        break;
                    }
                    case 6:
                    {
                        if(static_cast<int>(pReceive.Data[4]) == 1)
                        {
                            QJsonObject EncodeInterface;
                            EncodeInterface.insert("standard", "0");
                            EncodeInterface.insert("test", "1");
                            EncodeInterface.insert("difference", "1");
                            EncodeInterface.insert("allowable", "0");
                            EncodeInterface.insert("result", "未通过");
                            testResults.insert("EncodeInterface",
                                               EncodeInterface);
                            step = 0;
                            emit signalReceiveStatus("设置低电平失败");
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            QJsonObject EncodeInterface;
                            EncodeInterface.insert(
                                "standard",
                                QString::number(
                                    static_cast<int>(pReceive.Data[2])));
                            EncodeInterface.insert(
                                "test", QString::number(static_cast<int>(
                                            pReceive.Data[2])));
                            EncodeInterface.insert("difference", "0");
                            EncodeInterface.insert("allowable", "0");
                            EncodeInterface.insert("result", "通过");
                            testResults.insert("EncodeInterface",
                                               EncodeInterface);
                            step = 0;
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        break;
                    }
                    default:
                        break;
                    }
                    break;
            }
            case TestDO:
            {
                switch (step)
                {
                case 2:
                {
                        if (1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO11", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO1L设置错误");
                                step = 0;
                                DOTest.insert("DO11", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 4:
                {
                        if (0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO10", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO1L设置错误");
                                step = 0;
                                DOTest.insert("DO10", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 6:
                {
                        if (1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO21", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO2L设置错误");
                                step = 0;
                                DOTest.insert("DO21", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 8:
                {
                        if (0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO20", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO2L设置错误");
                                step = 0;
                                DOTest.insert("DO20", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 10:
                {
                        if (1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO31", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO3L设置错误");
                                step = 0;
                                DOTest.insert("DO31", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 12:
                {
                        if (0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO30", "通过");
                            if(24 == m_type || 25 == m_type || 26 == m_type)
                            {
                                step = 0;
                                testResults.insert("DOTest", DOTest);
                                emit stopTimerSignal();
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                if (!testTimer)
                                {
                                    testTimer = new QTimer();
                                    testTimer->setSingleShot(true);
                                    connect(testTimer,
                                            SIGNAL(timeout()), this,
                                            SLOT(updateTestItem()));
                                }
                                testTimer->start(100);
                                emit stopTimerSignal();
                            }
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO3L设置错误");
                                step = 0;
                                DOTest.insert("DO30", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 14:
                {
                        if (1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO41", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO4L设置错误");
                                step = 0;
                                DOTest.insert("DO41", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 16:
                {
                        if (0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO40", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO4L设置错误");
                                step = 0;
                                DOTest.insert("DO40", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 18:
                {
                        if (1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO51", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO5L设置错误");
                                step = 0;
                                DOTest.insert("DO51", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 20:
                {
                        if (0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO50", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO5L设置错误");
                                step = 0;
                                DOTest.insert("DO50", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 22:
                {
                        if (1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO61", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO6L设置错误");
                                step = 0;
                                DOTest.insert("DO61", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 24:
                {
                        if (0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO60", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO6L设置错误");
                                step = 0;
                                DOTest.insert("DO60", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 26:
                {
                        if (1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO71", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO7L设置错误");
                                step = 0;
                                DOTest.insert("DO71", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 28:
                {
                        if (0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO70", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO7L设置错误");
                                step = 0;
                                DOTest.insert("DO70", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 30:
                {
                        if (1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO81", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO8L设置错误");
                                step = 0;
                                DOTest.insert("DO81", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                case 32:
                {
                        if (0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DOTest.insert("DO80", "通过");
                            step = 0;
                            testResults.insert("DOTest", DOTest);
                            emit stopTimerSignal();
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DO8设置错误");
                                step = 0;
                                DOTest.insert("DO80", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                }
                default:
                {
                    updateTestItem();
                    break;
                }
                }
                break;
            }
            case TestDIAndSLP:
            {
                if(m_type == 24 || m_type == 25 || m_type == 26)
                {
                        switch (step)
                        {
                        case 2:
                        {
                            if (1 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                            {
                                DITest.insert("DI11", "通过");
                                if (!testTimer)
                                {
                                    testTimer = new QTimer();
                                    testTimer->setSingleShot(true);
                                    connect(testTimer,
                                            SIGNAL(timeout()), this,
                                            SLOT(updateTestItem()));
                                }
                                testTimer->start(100);
                                emit stopTimerSignal();
                            }
                            else
                            {
                                if (retryNumber >= 50)
                                {
                                    retryNumber = 0;
                                    emit signalReceiveStatus(
                                        "DI1L设置错误");
                                    step = 0;
                                    DITest.insert("DI11", "未通过");
                                    emit stopTimerSignal();
                                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                                else
                                {
                                    retryNumber++;
                                    step--;
                                    updateTestItem();
                                }
                            }
                            break;
                        }
                        case 4:
                        {
                            if (1 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                            {
                                DITest.insert("DI10", "通过");
                                step = 0;
                                emit stopTimerSignal();
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                if (retryNumber >= 50)
                                {
                                    retryNumber = 0;
                                    emit signalReceiveStatus(
                                        "DI1L设置错误");
                                    step = 0;
                                    DITest.insert("DI10", "未通过");
                                    emit stopTimerSignal();
                                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                                else
                                {
                                    retryNumber++;
                                    step--;
                                    updateTestItem();
                                }
                            }
                            break;
                        }
                        case 10:
                        {
                            if (3 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                            {
                                DITest.insert("DI31", "通过");
                                if (!testTimer)
                                {
                                    testTimer = new QTimer();
                                    testTimer->setSingleShot(true);
                                    connect(testTimer,
                                            SIGNAL(timeout()), this,
                                            SLOT(updateTestItem()));
                                }
                                testTimer->start(100);
                                emit stopTimerSignal();
                            }
                            else
                            {
                                if (retryNumber >= 50)
                                {
                                    retryNumber = 0;
                                    emit signalReceiveStatus(
                                        "DI3L设置错误");
                                    step = 0;
                                    DITest.insert("DI31", "未通过");
                                    emit stopTimerSignal();
                                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                                else
                                {
                                    retryNumber++;
                                    step--;
                                    updateTestItem();
                                }
                            }
                            break;
                        }
                        case 12:
                        {
                            if (3 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                            {
                                DITest.insert("DI30", "通过");
                                step = 0;
                                testResults.insert("DITest", DITest);
                                emit stopTimerSignal();
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                if (retryNumber >= 50)
                                {
                                    retryNumber = 0;
                                    emit signalReceiveStatus(
                                        "DI3L设置错误");
                                    step = 0;
                                    DITest.insert("DI30", "未通过");
                                    emit stopTimerSignal();
                                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                                else
                                {
                                    retryNumber++;
                                    step--;
                                    updateTestItem();
                                }
                            }
                            break;
                        }
                        case 6:
                        {
                            if (2 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                            {
                                DITest.insert("DI21", "通过");
                                if (!testTimer)
                                {
                                    testTimer = new QTimer();
                                    testTimer->setSingleShot(true);
                                    connect(testTimer,
                                            SIGNAL(timeout()), this,
                                            SLOT(updateTestItem()));
                                }
                                testTimer->start(100);
                                emit stopTimerSignal();
                            }
                            else
                            {
                                if (retryNumber >= 50)
                                {
                                    retryNumber = 0;
                                    emit signalReceiveStatus(
                                        "DI2L设置错误");
                                    step = 0;
                                    DITest.insert("DI21", "未通过");
                                    emit stopTimerSignal();
                                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                                else
                                {
                                    retryNumber++;
                                    step--;
                                    updateTestItem();
                                }
                            }
                            break;
                        }
                        case 8:
                        {
                            if (2 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                            {
                                DITest.insert("DI20", "通过");
                                step = 0;
                                emit stopTimerSignal();
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                if (retryNumber >= 50)
                                {
                                    retryNumber = 0;
                                    emit signalReceiveStatus(
                                        "DI2L设置错误");
                                    step = 0;
                                    DITest.insert("DI20", "未通过");
                                    emit stopTimerSignal();
                                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                                else
                                {
                                    retryNumber++;
                                    step--;
                                    updateTestItem();
                                }
                            }
                            break;
                        }
                        default:
                        {
                            updateTestItem();
                            break;
                        }
                        }
                }
                else
                {
                    switch (step)
                    {
                    case 2:
                    {
                        if (2 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI21", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI1L设置错误");
                                step = 0;
                                DITest.insert("DI21", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 4:
                    {
                        if (2 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI20", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI1L设置错误");
                                step = 0;
                                DITest.insert("DI20", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 6:
                    {
                        if (4 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI41", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI2L设置错误");
                                step = 0;
                                DITest.insert("DI41", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 8:
                    {
                        if (4 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI40", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI2L设置错误");
                                step = 0;
                                DITest.insert("DI40", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 10:
                    {
                        if (6 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI61", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI3L设置错误");
                                step = 0;
                                DITest.insert("DI61", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 12:
                    {
                        if (6 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI60", "通过");
                            if(24 == m_type || 25 == m_type || 26 == m_type)
                            {
                                step = 0;
                                testResults.insert("DITest", DITest);
                                emit stopTimerSignal();
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                if (!testTimer)
                                {
                                    testTimer = new QTimer();
                                    testTimer->setSingleShot(true);
                                    connect(testTimer,
                                            SIGNAL(timeout()), this,
                                            SLOT(updateTestItem()));
                                }
                                testTimer->start(100);
                                emit stopTimerSignal();
                            }
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI3L设置错误");
                                step = 0;
                                DITest.insert("DI60", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 14:
                    {
                        if (1 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI11", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI4L设置错误");
                                step = 0;
                                DITest.insert("DI11", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 16:
                    {
                        if (1 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI10", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI4L设置错误");
                                step = 0;
                                DITest.insert("DI10", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 18:
                    {
                        if (3 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI31", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI5L设置错误");
                                step = 0;
                                DITest.insert("DI31", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 20:
                    {
                        if (3 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI30", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI5L设置错误");
                                step = 0;
                                DITest.insert("DI30", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 22:
                    {
                        if (5 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI51", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI6L设置错误");
                                step = 0;
                                DITest.insert("DI51", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 24:
                    {
                        if (5 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI50", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI6L设置错误");
                                step = 0;
                                DITest.insert("DI50", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 26:
                    {
                        if (7 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI71", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI7L设置错误");
                                step = 0;
                                DITest.insert("DI71", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 28:
                    {
                        if (7 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI70", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI7L设置错误");
                                step = 0;
                                DITest.insert("DI70", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 30:
                    {
                        if (8 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI81", "通过");
                            if (!testTimer)
                            {
                                testTimer = new QTimer();
                                testTimer->setSingleShot(true);
                                connect(testTimer,
                                        SIGNAL(timeout()), this,
                                        SLOT(updateTestItem()));
                            }
                            testTimer->start(100);
                            emit stopTimerSignal();
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI8L设置错误");
                                step = 0;
                                DITest.insert("DI81", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    case 32:
                    {
                        if (8 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                        {
                            DITest.insert("DI80", "通过");
                            step = 0;
                            testResults.insert("DITest", DITest);
                            emit stopTimerSignal();
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            if (retryNumber >= 50)
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "DI8设置错误");
                                step = 0;
                                DITest.insert("DI80", "未通过");
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                retryNumber++;
                                step--;
                                updateTestItem();
                            }
                        }
                        break;
                    }
                    default:
                    {
                        updateTestItem();
                        break;
                    }
                    }
                }
                break;
            }
            case TestEEPROM:
            {
                emit stopTimerSignal();
                if (0 == static_cast<int>(pReceive.Data[2]))
                {
                    step = 0;
                    QJsonObject EEPROM;
                    EEPROM.insert("standard",
                                  QString::number(static_cast<int>(
                                      pReceive.Data[2])));
                    EEPROM.insert("test", "0");
                    EEPROM.insert("difference",
                                  QString::number(static_cast<int>(
                                      pReceive.Data[1])));
                    EEPROM.insert("allowable", "0");
                    EEPROM.insert("result", "通过");
                    testResults.insert("EEPROM", EEPROM);
                    emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                }
                else
                {
                    emit signalReceiveStatus("EEPROM读取错误");
                    QJsonObject EEPROM;
                    EEPROM.insert("standard",
                                  QString::number(static_cast<int>(
                                      pReceive.Data[2])));
                    EEPROM.insert("test", "0");
                    EEPROM.insert("difference",
                                  QString::number(static_cast<int>(
                                      pReceive.Data[2])));
                    EEPROM.insert("allowable", "0");
                    EEPROM.insert("result", "未通过");
                    testResults.insert("EEPROM", EEPROM);
                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    step = 0;
                }
                break;
            }
            case TestFlash:
            {
                emit stopTimerSignal();
                if (m_type == 36 || m_type == 37 || m_type == 38)
                {
                    if (0 == static_cast<int>(pReceive.Data[2]))
                    {
                        step = 0;
                        QJsonObject Flash1;
                        Flash1.insert("test",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash1.insert("standard", "0");
                        Flash1.insert("difference",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash1.insert("allowable", "0");
                        Flash1.insert("result", "通过");
                        testResults.insert("Flash1", Flash1);
                        QJsonObject Flash2;
                        Flash2.insert("test",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash2.insert("standard", "0");
                        Flash2.insert("difference",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash2.insert("allowable", "0");
                        Flash2.insert("result", "通过");
                        testResults.insert("Flash2", Flash2);
                        QJsonObject Flash3;
                        Flash3.insert("test",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash3.insert("standard", "0");
                        Flash3.insert("difference",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash3.insert("allowable", "0");
                        Flash3.insert("result", "通过");
                        testResults.insert("Flash3", Flash3);
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    else
                    {
                        emit signalReceiveStatus("Flash读取错误");
                        QJsonObject Flash1;
                        Flash1.insert("test",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash1.insert("standard", "0");
                        Flash1.insert("difference",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash1.insert("allowable", "0");
                        Flash1.insert("result", static_cast<int>(
                                          pReceive.Data[2]) == 0 ? "通过" : "未通过");
                        testResults.insert("Flash1", Flash1);
                        QJsonObject Flash2;
                        Flash2.insert("test",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash2.insert("standard", "0");
                        Flash2.insert("difference",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash2.insert("allowable", "0");
                        Flash2.insert("result", static_cast<int>(
                                          pReceive.Data[2]) == 0 ? "通过" : "未通过");
                        testResults.insert("Flash2", Flash2);
                        QJsonObject Flash3;
                        Flash3.insert("test",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash3.insert("standard", "0");
                        Flash3.insert("difference",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash3.insert("allowable", "0");
                        Flash3.insert("result", static_cast<int>(
                                          pReceive.Data[2]) == 0 ? "通过" : "未通过");
                        testResults.insert("Flash3", Flash3);
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        step = 0;
                    }
                }
                else
                {
                    if (0 == static_cast<int>(pReceive.Data[2]))
                    {
                        step = 0;
                        QJsonObject Flash1;
                        Flash1.insert("test",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash1.insert("standard", "0");
                        Flash1.insert("difference",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash1.insert("allowable", "0");
                        Flash1.insert("result", "通过");
                        testResults.insert("Flash1", Flash1);
                        QJsonObject Flash2;
                        Flash2.insert("test", "untested");
                        Flash2.insert("standard", "untested");
                        Flash2.insert("difference", "untested");
                        Flash2.insert("allowable", "untested");
                        Flash2.insert("result", "untested");
                        testResults.insert("Flash2", Flash2);
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    else
                    {
                        emit signalReceiveStatus("Flash读取错误");
                        QJsonObject Flash1;
                        Flash1.insert("test",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash1.insert("standard", "0");
                        Flash1.insert("difference",
                                      QString::number(static_cast<int>(
                                          pReceive.Data[2])));
                        Flash1.insert("allowable", "0");
                        Flash1.insert("result", "未通过");
                        testResults.insert("Flash1", Flash1);
                        QJsonObject Flash2;
                        Flash2.insert("test", "untested");
                        Flash2.insert("standard", "untested");
                        Flash2.insert("difference", "untested");
                        Flash2.insert("allowable", "untested");
                        Flash2.insert("result", "untested");
                        testResults.insert("Flash2", Flash2);
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        step = 0;
                    }
                }
                break;
            }
            case TestRTC:
            {
                emit stopTimerSignal();
                unsigned char data[8];
                data[0] = 0x12;
                data[1] = 0;
                data[2] = 0;
                data[3] = 0;
                data[4] = 0;
                data[5] = 0;
                data[6] = 0;
                data[7] = 0;
                TransmitCAN(((0x181F << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
                emit signalReceiveStatus("RTC读取中。。。");
                break;
            }
            case TestSystemTemperature:
            {
                emit stopTimerSignal();
                if((static_cast<int>(pReceive.Data[3]) << 8) +
                        static_cast<int>(pReceive.Data[4]) > 32767)
                {
                    m_dctemp = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                    static_cast<int>(pReceive.Data[4])) - 65535) / 10.0;
                }
                else
                {
                    m_dctemp = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                    static_cast<int>(pReceive.Data[4]))) / 10.0;
                }
                if (m_type == 19 || m_type == 20 || m_type == 18 ||
                    m_type == 12 || m_type == 13 || m_type == 14 ||
                    m_type == 30 || m_type == 31 || m_type == 32)
                {
                    if (8 >= abs(m_gztemp - m_dctemp))
                    {
                        step = 0;
                        SystemTemperature.insert(
                            "test",
                            QString::number(m_dctemp) +
                                "℃");
                        SystemTemperature.insert(
                            "difference",
                            QString::number(
                                m_gztemp - m_dctemp) +
                                "℃");
                        SystemTemperature.insert("allowable", "±8℃");
                        SystemTemperature.insert("result", "通过");
                        testResults.insert("SystemTemperature",
                                           SystemTemperature);
                        step = 0;
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    else
                    {
                        SystemTemperature.insert(
                            "test",
                            QString::number(m_dctemp) +
                                "℃");
                        SystemTemperature.insert(
                            "difference",
                            QString::number(
                                m_gztemp - m_dctemp) +
                                "℃");
                        SystemTemperature.insert("allowable", "±8℃");
                        SystemTemperature.insert("result", "未通过");
                        testResults.insert("SystemTemperature",
                                           SystemTemperature);
                        emit signalReceiveStatus(
                            "系统温度测试错误，工装板温度为" +
                            QString::number(m_gztemp) +
                            "待测板温度为" +
                            QString::number(m_dctemp) +
                            "℃");
                        step = 0;
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                }
                else
                {
                    if (5 >=
                        abs(m_gztemp - m_dctemp))
                    {
                        step = 0;
                        SystemTemperature.insert(
                            "test",
                            QString::number(m_dctemp) +
                                "℃");
                        SystemTemperature.insert(
                            "difference",
                            QString::number(
                                m_gztemp - m_dctemp) +
                                "℃");
                        SystemTemperature.insert("allowable", "±5℃");
                        SystemTemperature.insert("result", "通过");
                        testResults.insert("SystemTemperature",
                                           SystemTemperature);
                        step = 0;
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    else
                    {
                        SystemTemperature.insert(
                            "test",
                            QString::number(m_dctemp) +
                                "℃");
                        SystemTemperature.insert(
                            "difference",
                            QString::number(
                                m_gztemp - m_dctemp) +
                                "℃");
                        SystemTemperature.insert("allowable", "±5℃");
                        SystemTemperature.insert("result", "未通过");
                        testResults.insert("SystemTemperature",
                                           SystemTemperature);
                        emit signalReceiveStatus(
                            "系统温度测试错误，工装板温度为" +
                            QString::number(m_gztemp) +
                            "待测板温度为" +
                            QString::number(m_dctemp) +
                            "℃");
                        step = 0;
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                }
                break;
            }
            case TestSPI:
            {
                emit stopTimerSignal();
                if (0 == static_cast<int>(pReceive.Data[1]))
                {
                    QJsonObject SPI;
                    SPI.insert("standard",
                               QString::number(0));
                    SPI.insert("test", QString::number(static_cast<int>(
                                           pReceive.Data[1])));
                    SPI.insert("difference", "0");
                    SPI.insert("allowable", "0");
                    SPI.insert("result", "通过");
                    testResults.insert("SPI", SPI);
                    step = 0;
                    emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();
                }
                else
                {
                    QJsonObject SPI;
                    SPI.insert("standard",
                               QString::number(0));
                    SPI.insert("test", QString::number(static_cast<int>(
                                           pReceive.Data[1])));
                    SPI.insert("difference", QString::number(static_cast<int>(
                                                                 pReceive.Data[1])));
                    SPI.insert("allowable", "0");
                    SPI.insert("result", "未通过");
                    testResults.insert("SPI", SPI);
                    step = 0;
                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();
                }
                break;
            }
            case TestSetHALLCurrent:
            {
                if(pReceive.Data[0] != 0xAA)
                {
                    emit stopTimerSignal();
                    switch(step)
                    {
                    case 3:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 5:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 7:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 10:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 12:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 14:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 17:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 19:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 21:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 24:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 26:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 28:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 29:
                    {
                        break;
                    }
                    case 32:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 34:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 36:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 39:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 41:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 43:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        QJsonObject OneZheng;
                        OneZheng.insert("standard",
                                        QString::number(m_gzaz13, 'f', 1) + "A");
                        OneZheng.insert("test",
                                        QString::number(m_dcaz13, 'f', 1) + "A");
                        OneZheng.insert("difference",
                                        QString::number(m_gzaz13 - m_dcaz13, 'f', 1) +
                                            "A");
                        OneZheng.insert("allowable", "±2A");
                        if (2 >= abs(m_gzaz13 - m_dcaz13))
                        {
                            OneZheng.insert("result", "通过");
                        }
                        else
                        {
                            OneZheng.insert("result", "未通过");
                            ifCPass = false;
                        }
                        QJsonObject TwoZheng;
                        TwoZheng.insert("standard",
                                        QString::number(m_gzaz23, 'f', 1) + "A");
                        TwoZheng.insert("test",
                                        QString::number(m_dcaz23, 'f', 1) + "A");
                        TwoZheng.insert("difference",
                                        QString::number(m_gzaz23 - m_dcaz23, 'f', 1) +
                                            "A");
                        TwoZheng.insert("allowable", "±2A");
                        if (2 >= abs(m_gzaz23 - m_dcaz23))
                        {
                            TwoZheng.insert("result", "通过");
                        }
                        else
                        {
                            TwoZheng.insert("result", "未通过");
                            ifCPass = false;
                        }
                        QJsonObject ThreeZheng;
                        ThreeZheng.insert("standard",
                                        QString::number(m_gzaz33, 'f', 1) + "A");
                        ThreeZheng.insert("test",
                                        QString::number(m_dcaz33, 'f', 1) + "A");
                        ThreeZheng.insert("difference",
                                        QString::number(m_gzaz33 - m_dcaz33, 'f', 1) +
                                            "A");
                        ThreeZheng.insert("allowable", "±2A");
                        if (2 >= abs(m_gzaz33 - m_dcaz33))
                        {
                            ThreeZheng.insert("result", "通过");
                        }
                        else
                        {
                            ThreeZheng.insert("result", "未通过");
                            ifCPass = false;
                        }
                        QJsonObject OneFu;
                        OneFu.insert("standard",
                                        QString::number(m_gzaf13, 'f', 1) + "A");
                        OneFu.insert("test",
                                        QString::number(m_dcaf13, 'f', 1) + "A");
                        OneFu.insert("difference",
                                        QString::number(m_gzaf13 - m_dcaf13, 'f', 1) +
                                            "A");
                        OneFu.insert("allowable", "±2A");
                        if (2 >= abs(m_gzaf13 - m_dcaf13))
                        {
                            OneFu.insert("result", "通过");
                        }
                        else
                        {
                            OneFu.insert("result", "未通过");
                            ifCPass = false;
                        }
                        QJsonObject TwoFu;
                        TwoFu.insert("standard",
                                        QString::number(m_gzaf23, 'f', 1) + "A");
                        TwoFu.insert("test",
                                        QString::number(m_dcaf23, 'f', 1) + "A");
                        TwoFu.insert("difference",
                                        QString::number(m_gzaf23 - m_dcaf23, 'f', 1) +
                                            "A");
                        TwoFu.insert("allowable", "±2A");
                        if (2 >= abs(m_gzaf23 - m_dcaf23))
                        {
                            TwoFu.insert("result", "通过");
                        }
                        else
                        {
                            TwoFu.insert("result", "未通过");
                            ifCPass = false;
                        }
                        QJsonObject ThreeFu;
                        ThreeFu.insert("standard",
                                        QString::number(m_gzaf33, 'f', 1) + "A");
                        ThreeFu.insert("test",
                                        QString::number(m_dcaf33, 'f', 1) + "A");
                        ThreeFu.insert("difference",
                                        QString::number(m_gzaf33 - m_dcaf33, 'f', 1) +
                                            "A");
                        ThreeFu.insert("allowable", "±2A");
                        if (2 >= abs(m_gzaf33 - m_dcaf33))
                        {
                            ThreeFu.insert("result", "通过");
                        }
                        else
                        {
                            ThreeFu.insert("result", "未通过");
                            ifCPass = false;
                        }
                        HALLCurrent.insert("OneZheng", OneZheng);
                        HALLCurrent.insert("TwoZheng", TwoZheng);
                        HALLCurrent.insert("ThreeZheng", ThreeZheng);
                        HALLCurrent.insert("OneFu", OneFu);
                        HALLCurrent.insert("TwoFu", TwoFu);
                        HALLCurrent.insert("ThreeFu", ThreeFu);
                        testResults.insert("HALLCurrent", HALLCurrent);
                        failTimer->stop();
                        hallTime = 0;
                        if (ifCPass)
                        {
                            step = 0;
                            emit signalFinish();timeoutTimer.stop();step = 0;
                        }
                        else
                        {
                            step = 0;
                            emit signalErrorFinish();timeoutTimer.stop();step = 0;
                            ifCPass = true;
                        }
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    }
                }
                break;
            }
            case TestHALLCurrent:
            {
                if(pReceive.Data[0] != 0xAA)
                {
                    emit stopTimerSignal();
                    switch(step)
                    {
                    case 46:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 48:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 50:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 53:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 55:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 57:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        QJsonObject OneZheng;
                        OneZheng.insert("standard",
                                        QString::number(m_gzaz13, 'f', 1) + "A");
                        OneZheng.insert("test",
                                        QString::number(m_dcaz13, 'f', 1) + "A");
                        OneZheng.insert("difference",
                                        QString::number(m_gzaz13 - m_dcaz13, 'f', 1) +
                                            "A");
                        OneZheng.insert("allowable", "±2A");
                        if (2 >= abs(m_gzaz13 - m_dcaz13))
                        {
                            OneZheng.insert("result", "通过");
                        }
                        else
                        {
                            OneZheng.insert("result", "未通过");
                            ifCPass = false;
                        }
                        QJsonObject TwoZheng;
                        TwoZheng.insert("standard",
                                        QString::number(m_gzaz23, 'f', 1) + "A");
                        TwoZheng.insert("test",
                                        QString::number(m_dcaz23, 'f', 1) + "A");
                        TwoZheng.insert("difference",
                                        QString::number(m_gzaz23 - m_dcaz23, 'f', 1) +
                                            "A");
                        TwoZheng.insert("allowable", "±2A");
                        if (2 >= abs(m_gzaz23 - m_dcaz23))
                        {
                            TwoZheng.insert("result", "通过");
                        }
                        else
                        {
                            TwoZheng.insert("result", "未通过");
                            ifCPass = false;
                        }
                        QJsonObject ThreeZheng;
                        ThreeZheng.insert("standard",
                                        QString::number(m_gzaz33, 'f', 1) + "A");
                        ThreeZheng.insert("test",
                                        QString::number(m_dcaz33, 'f', 1) + "A");
                        ThreeZheng.insert("difference",
                                        QString::number(m_gzaz33 - m_dcaz33, 'f', 1) +
                                            "A");
                        ThreeZheng.insert("allowable", "±2A");
                        if (2 >= abs(m_gzaz33 - m_dcaz33))
                        {
                            ThreeZheng.insert("result", "通过");
                        }
                        else
                        {
                            ThreeZheng.insert("result", "未通过");
                            ifCPass = false;
                        }
                        QJsonObject OneFu;
                        OneFu.insert("standard",
                                        QString::number(m_gzaf13, 'f', 1) + "A");
                        OneFu.insert("test",
                                        QString::number(m_dcaf13, 'f', 1) + "A");
                        OneFu.insert("difference",
                                        QString::number(m_gzaf13 - m_dcaf13, 'f', 1) +
                                            "A");
                        OneFu.insert("allowable", "±2A");
                        if (2 >= abs(m_gzaf13 - m_dcaf13))
                        {
                            OneFu.insert("result", "通过");
                        }
                        else
                        {
                            OneFu.insert("result", "未通过");
                            ifCPass = false;
                        }
                        QJsonObject TwoFu;
                        TwoFu.insert("standard",
                                        QString::number(m_gzaf23, 'f', 1) + "A");
                        TwoFu.insert("test",
                                        QString::number(m_dcaf23, 'f', 1) + "A");
                        TwoFu.insert("difference",
                                        QString::number(m_gzaf23 - m_dcaf23, 'f', 1) +
                                            "A");
                        TwoFu.insert("allowable", "±2A");
                        if (2 >= abs(m_gzaf23 - m_dcaf23))
                        {
                            TwoFu.insert("result", "通过");
                        }
                        else
                        {
                            TwoFu.insert("result", "未通过");
                            ifCPass = false;
                        }
                        QJsonObject ThreeFu;
                        ThreeFu.insert("standard",
                                        QString::number(m_gzaf33, 'f', 1) + "A");
                        ThreeFu.insert("test",
                                        QString::number(m_dcaf33, 'f', 1) + "A");
                        ThreeFu.insert("difference",
                                        QString::number(m_gzaf33 - m_dcaf33, 'f', 1) +
                                            "A");
                        ThreeFu.insert("allowable", "±2A");
                        if (2 >= abs(m_gzaf33 - m_dcaf33))
                        {
                            ThreeFu.insert("result", "通过");
                        }
                        else
                        {
                            ThreeFu.insert("result", "未通过");
                            ifCPass = false;
                        }
                        HALLCurrent.insert("OneZheng", OneZheng);
                        HALLCurrent.insert("TwoZheng", TwoZheng);
                        HALLCurrent.insert("ThreeZheng", ThreeZheng);
                        HALLCurrent.insert("OneFu", OneFu);
                        HALLCurrent.insert("TwoFu", TwoFu);
                        HALLCurrent.insert("ThreeFu", ThreeFu);
                        testResults.insert("HALLCurrent", HALLCurrent);
                        failTimer->stop();
                        hallTime = 0;
                        if (ifCPass)
                        {
                            step = 0;
                            emit signalFinish();timeoutTimer.stop();step = 0;
                        }
                        else
                        {
                            step = 0;
                            emit signalErrorFinish();timeoutTimer.stop();step = 0;
                            ifCPass = true;
                        }
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    }
                }
                break;
            }
            default:
                break;
            }
        }
        else if(static_cast<int>(pReceive.ID) == id14)
        {
            if(pReceive.Data[0] == 0x0C)
            {
                    emit stopTimerSignal();
                    emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                       static_cast<int>(pReceive.Data[5])) /
                                      10.0);
            }
            else if(pReceive.Data[0] == 0x0E)
            {
                    emit stopTimerSignal();
                    float temp = 0.0;
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    emit signalGetInfo(m_info, temp);
            }
            switch (lastTestType)
            {
            case TestSetBatteryVoltage:
            {
                emit stopTimerSignal();
                switch(step)
                {
                case 2:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvz1 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                case 4:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvy1 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                case 7:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvz2 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                case 9:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvy2 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                case 13:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvz3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                case 15:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvy3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                default:
                {
                    break;
                }
                }
                break;
            }
            case TestBatteryVoltage:
            {
                emit stopTimerSignal();
                switch(step)
                {
                case 18:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvz3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                case 20:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvy3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                default:
                {
                    break;
                }
                }
                break;
            }
            case TestSupplyVoltage:
            {
                    emit stopTimerSignal();
                    m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    if (m_info == 9)
                    {
                        emit signalGetInfo(99, m_gzgd * 10);
                        m_info = 0;
                    }
                    else
                    {
                        if (abs(m_gzgd - m_dcgd) <= 1)
                        {
                            QJsonObject SupplyVoltage;
                            SupplyVoltage.insert(
                                "standard",
                                QString::number(m_gzgd, 'f', 1) + "V");
                            SupplyVoltage.insert(
                                "test",
                                QString::number(m_dcgd, 'f', 1) + "V");
                            SupplyVoltage.insert(
                                "difference",
                                QString::number(abs(m_gzgd - m_dcgd), 'f', 1) +
                                    "V");
                            SupplyVoltage.insert("allowable", "±1V");
                            SupplyVoltage.insert("result", "通过");
                            testResults.insert("SupplyVoltage",
                                               SupplyVoltage);
                            step = 0;
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            QJsonObject SupplyVoltage;
                            SupplyVoltage.insert(
                                "standard",
                                QString::number(m_gzgd, 'f', 1) + "V");
                            SupplyVoltage.insert(
                                "test",
                                QString::number(m_dcgd, 'f', 1) + "V");
                            SupplyVoltage.insert(
                                "difference",
                                QString::number(abs(m_gzgd - m_dcgd), 'f', 1) +
                                    "V");
                            SupplyVoltage.insert("allowable", "±1V");
                            SupplyVoltage.insert("result", "未通过");
                            testResults.insert("SupplyVoltage",
                                               SupplyVoltage);
                            step = 0;
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                    }
                    break;
            }
            case TestHALLSignal:
            {
                emit stopTimerSignal();
                if(pReceive.Data[0] == 0x0E)
                {
                    if(step == 1)
                    {
                        float power = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                           static_cast<int>(pReceive.Data[5]))) / 10.0;
                        if (m_type == 27 || m_type == 28 || m_type == 29)
                        {
                            if (5.2 >= power && 4.5 <= power)
                            {
                                QJsonObject Five;
                                Five.insert("standard", "4.85V");
                                Five.insert("test", QString::number(
                                                        power, 'f', 1) +
                                                        "V");
                                Five.insert("difference",
                                            QString::number(
                                                power - 4.85, 'f', 2) +
                                                "V");
                                Five.insert("allowable", "4.5V~5.2V");
                                Five.insert("result", "通过");
                                HALLSignal.insert("Five", Five);
                                updateTestItem();
                            }
                            else
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "电压为" + QString::number(power) +
                                    "V，不符合要求");
                                step = 0;
                                QJsonObject Five;
                                Five.insert("standard", "4.85V");
                                Five.insert("test", QString::number(
                                                        power, 'f', 1) +
                                                        "V");
                                Five.insert("difference",
                                            QString::number(
                                                power - 4.85, 'f', 2) +
                                                "V");
                                Five.insert("allowable", "4.5V~5.2V");
                                Five.insert("allowable", "±0.5V");
                                Five.insert("result", "未通过");
                                HALLSignal.insert("Five", Five);
                                QJsonObject Twelve;
                                Twelve.insert("standard", "12V");
                                Twelve.insert("test", "untested");
                                Twelve.insert("difference", "untested");
                                Twelve.insert("allowable", "±0.5V");
                                Twelve.insert("result", "untested");
                                HALLSignal.insert("Twelve", Twelve);
                                testResults.insert("HALLSignal", HALLSignal);
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                        }
                        else if (m_type == 33 || m_type == 34 || m_type == 35)
                        {
                            if (5.5 >= power && 4.5 <= power)
                            {
                                QJsonObject Five;
                                Five.insert("standard", "5V");
                                Five.insert("test", QString::number(
                                                        power, 'f', 1) +
                                                        "V");
                                Five.insert("difference",
                                            QString::number(power - 5, 'f', 1) +
                                                "V");
                                Five.insert("allowable", "±0.5V");
                                Five.insert("result", "通过");
                                HALLSignal.insert("Five", Five);
                                updateTestItem();
                            }
                            else
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "电压为" + QString::number(power) +
                                    "V，不符合要求");
                                step = 0;
                                QJsonObject Five;
                                Five.insert("standard", "5V");
                                Five.insert("test", QString::number(
                                                        power, 'f', 1) +
                                                        "V");
                                Five.insert("difference",
                                            QString::number(power - 5, 'f', 1) +
                                                "V");
                                Five.insert("allowable", "±0.1V");
                                Five.insert("result", "未通过");
                                HALLSignal.insert("Five", Five);
                                QJsonObject Twelve;
                                Twelve.insert("standard", "12V");
                                Twelve.insert("test", "untested");
                                Twelve.insert("difference", "untested");
                                Twelve.insert("allowable", "±0.5V");
                                Twelve.insert("result", "untested");
                                HALLSignal.insert("Twelve", Twelve);
                                testResults.insert("HALLSignal", HALLSignal);
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                        }
                        else
                        {
                            if (5.1 >= power && 4.9 <= power)
                            {
                                QJsonObject Five;
                                Five.insert("standard", "5V");
                                Five.insert("test", QString::number(
                                                        power, 'f', 1) +
                                                        "V");
                                Five.insert("difference",
                                            QString::number(power - 5, 'f', 1) +
                                                "V");
                                Five.insert("allowable", "±0.1V");
                                Five.insert("result", "通过");
                                HALLSignal.insert("Five", Five);
                                updateTestItem();
                            }
                            else
                            {
                                retryNumber = 0;
                                emit signalReceiveStatus(
                                    "电压为" + QString::number(power) +
                                    "V，不符合要求");
                                step = 0;
                                QJsonObject Five;
                                Five.insert("standard", "5V");
                                Five.insert("test", QString::number(
                                                        power, 'f', 1) +
                                                        "V");
                                Five.insert("difference",
                                            QString::number(power - 5, 'f', 1) +
                                                "V");
                                Five.insert("allowable", "±0.1V");
                                Five.insert("result", "未通过");
                                HALLSignal.insert("Five", Five);
                                QJsonObject Twelve;
                                Twelve.insert("standard", "12V");
                                Twelve.insert("test", "untested");
                                Twelve.insert("difference", "untested");
                                Twelve.insert("allowable", "±0.5V");
                                Twelve.insert("result", "untested");
                                HALLSignal.insert("Twelve", Twelve);
                                testResults.insert("HALLSignal", HALLSignal);
                                emit stopTimerSignal();
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                        }
                    }
                    else if(step == 2)
                    {
                        float cur = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                         static_cast<int>(pReceive.Data[5])));
                        if (m_type == 33 || m_type == 34 || m_type == 35)
                        {
                            if (128 >= cur && 112 <= cur)
                            {
                                QJsonObject Twelve;
                                Twelve.insert("standard", "12V");
                                Twelve.insert(
                                    "test",
                                    QString::number(cur / 10, 'f', 1) + "V");
                                Twelve.insert(
                                    "difference",
                                    QString::number(12.0 - cur / 10, 'f', 1) +
                                        "V");
                                Twelve.insert("allowable", "±0.8V");
                                Twelve.insert("result", "通过");
                                HALLSignal.insert("Twelve", Twelve);
                                testResults.insert("HALLSignal", HALLSignal);
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                emit signalReceiveStatus("霍尔错误");
                                QJsonObject Twelve;
                                Twelve.insert("standard", "12V");
                                Twelve.insert(
                                    "test",
                                    QString::number(cur / 10, 'f', 1) + "V");
                                Twelve.insert(
                                    "difference",
                                    QString::number(12.0 - cur / 10, 'f', 1) +
                                        "V");
                                Twelve.insert("allowable", "±0.5V");
                                Twelve.insert("result", "未通过");
                                HALLSignal.insert("Twelve", Twelve);
                                testResults.insert("HALLSignal", HALLSignal);
                                step = 0;
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                        }
                        else
                        {
                            if (125 >= cur && 115 <= cur)
                            {
                                QJsonObject Twelve;
                                Twelve.insert("standard", "12V");
                                Twelve.insert(
                                    "test",
                                    QString::number(cur / 10, 'f', 1) + "V");
                                Twelve.insert(
                                    "difference",
                                    QString::number(12.0 - cur / 10, 'f', 1) +
                                        "V");
                                Twelve.insert("allowable", "±0.5V");
                                Twelve.insert("result", "通过");
                                HALLSignal.insert("Twelve", Twelve);
                                testResults.insert("HALLSignal", HALLSignal);
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                emit signalReceiveStatus("霍尔错误");
                                QJsonObject Twelve;
                                Twelve.insert("standard", "12V");
                                Twelve.insert(
                                    "test",
                                    QString::number(cur / 10, 'f', 1) + "V");
                                Twelve.insert(
                                    "difference",
                                    QString::number(12.0 - cur / 10, 'f', 1) +
                                        "V");
                                Twelve.insert("allowable", "±0.5V");
                                Twelve.insert("result", "未通过");
                                HALLSignal.insert("Twelve", Twelve);
                                testResults.insert("HALLSignal", HALLSignal);
                                step = 0;
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                        }
                    }
                }
                break;
            }
            case TestSerialCommunication:
            {
                emit stopTimerSignal();
                if (m_type != 27 && m_type != 28 && m_type != 29)
                {
                    if(static_cast<int>(pReceive.Data[3]) == 0x1F)
                    {
                        QJsonObject SerialCommunication;
                        SerialCommunication.insert("standard", "31");
                        SerialCommunication.insert(
                            "test", QString::number(static_cast<int>(
                                        pReceive.Data[3])));
                        SerialCommunication.insert(
                            "difference",
                            QString::number(
                                static_cast<int>(pReceive.Data[3]) - 31));
                        SerialCommunication.insert("allowable", "0");
                        SerialCommunication.insert("result", "通过");
                        testResults.insert("SerialCommunication",
                                           SerialCommunication);
                        step = 0;
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();
                    }
                    else
                    {
                        QJsonObject SerialCommunication;
                        SerialCommunication.insert("standard", "31");
                        SerialCommunication.insert(
                            "test", QString::number(static_cast<int>(
                                        pReceive.Data[3])));
                        SerialCommunication.insert(
                            "difference",
                            QString::number(
                                static_cast<int>(pReceive.Data[3]) - 31));
                        SerialCommunication.insert("allowable", "0");
                        SerialCommunication.insert("result", "未通过");
                        emit signalReceiveStatus("485通信错误");
                        testResults.insert("SerialCommunication",
                                           SerialCommunication);
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();
                        step = 0;
                    }
                }
                else
                {
                    if(step == 1)
                    {
                        if(static_cast<int>(pReceive.Data[3]) == 0x1F)
                        {
                            updateTestItem();
                        }
                        else
                        {
                            QJsonObject SerialCommunication;
                            SerialCommunication.insert("standard", "31");
                            SerialCommunication.insert(
                                "test", QString::number(static_cast<int>(
                                            pReceive.Data[3])));
                            SerialCommunication.insert(
                                "difference",
                                QString::number(
                                    static_cast<int>(pReceive.Data[3]) - 31));
                            SerialCommunication.insert("allowable", "0");
                            SerialCommunication.insert("result", "未通过");
                            emit signalReceiveStatus("第1号485通信错误");
                            testResults.insert("SerialCommunication",
                                               SerialCommunication);
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();
                            step = 0;
                        }
                    }
                    else
                    {
                        if(static_cast<int>(pReceive.Data[3]) == 0x1F)
                        {
                            QJsonObject SerialCommunication;
                            SerialCommunication.insert("standard", "31");
                            SerialCommunication.insert(
                                "test", QString::number(static_cast<int>(
                                            pReceive.Data[3])));
                            SerialCommunication.insert(
                                "difference",
                                QString::number(
                                    static_cast<int>(pReceive.Data[3]) - 31));
                            SerialCommunication.insert("allowable", "0");
                            SerialCommunication.insert("result", "通过");
                            testResults.insert("SerialCommunication",
                                               SerialCommunication);
                            step = 0;
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();
                        }
                        else
                        {
                            QJsonObject SerialCommunication;
                            SerialCommunication.insert("standard", "31");
                            SerialCommunication.insert(
                                "test", QString::number(static_cast<int>(
                                            pReceive.Data[3])));
                            SerialCommunication.insert(
                                "difference",
                                QString::number(
                                    static_cast<int>(pReceive.Data[3]) - 31));
                            SerialCommunication.insert("allowable", "0");
                            SerialCommunication.insert("result", "未通过");
                            emit signalReceiveStatus("第2号485通信错误");
                            testResults.insert("SerialCommunication",
                                               SerialCommunication);
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();
                            step = 0;
                        }
                    }
                }
                break;
            }
            case TestCANCommunication:
            {
                emit stopTimerSignal();
                emit stopFailTimerSignal();
                if(step == 1)
                {
                    if(pReceive.Data[0] == 0x11)
                    {
                        if(pReceive.Data[3] == 18)
                        {
                            CANCommunication.insert("CAN0", "通过");
                        }
                        else
                        {
                            CANCommunication.insert("CAN0", "未通过");
                            ifCanPass = false;
                        }
                        QTimer *tempTimer = new QTimer(this);
                        connect(tempTimer, SIGNAL(timeout()), this, SLOT(updateTestItem()));
                        tempTimer->setSingleShot(true);
                        tempTimer->start(2000);
                    }
                }
                else if(step == 2)
                {
                    if(pReceive.Data[0] == 0x11)
                    {
                        if(pReceive.Data[3] == 18)
                        {
                            CANCommunication.insert("CAN1", "通过");
                            if (m_type == 18 || m_type == 19 || m_type == 20)
                            {
                                if(ifCanPass)
                                {
                                    if(failTimer->isActive())
                                    {
                                        failTimer->stop();
                                    }
                                    step = 0;
                                    emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();
                                }
                                else
                                {
                                    if(failTimer->isActive())
                                    {
                                        failTimer->stop();
                                    }
                                    step = 0;
                                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                            }
                            else
                            {
                                QTimer *tempTimer = new QTimer(this);
                                connect(tempTimer, SIGNAL(timeout()), this, SLOT(updateTestItem()));
                                tempTimer->setSingleShot(true);
                                tempTimer->start(2000);
                            }
                        }
                        else
                        {
                            CANCommunication.insert("CAN1", "未通过");
                            ifCanPass = false;
                            if (m_type == 18 || m_type == 19 || m_type == 20)
                            {
                                step = 0;
                                if(failTimer->isActive())
                                {
                                    failTimer->stop();
                                }
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                ifCanPass = false;
                                QTimer *tempTimer = new QTimer(this);
                                connect(tempTimer, SIGNAL(timeout()), this, SLOT(updateTestItem()));
                                tempTimer->setSingleShot(true);
                                tempTimer->start(2000);
                            }
                        }
                    }
                }
                else if(step == 3)
                {
                    if(pReceive.Data[3] == 18)
                    {
                        CANCommunication.insert("CAN2", "通过");
                        if(ifCanPass)
                        {
                            testResults.insert("CANCommunication",
                                               CANCommunication);
                            step = 0;
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();
                        }
                        else
                        {
                            testResults.insert("CANCommunication",
                                               CANCommunication);
                            step = 0;
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();
                        }
                    }
                    else
                    {
                        CANCommunication.insert("CAN2", "未通过");
                        testResults.insert("CANCommunication",
                                           CANCommunication);
                        step = 0;
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();
                    }
                }
                break;
            }
            case TestSystemTemperature:
            {
                emit stopTimerSignal();
                if((static_cast<int>(pReceive.Data[3]) << 8) +
                        static_cast<int>(pReceive.Data[4]) > 32767)
                {
                    m_gztemp = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                    static_cast<int>(pReceive.Data[4])) - 65535) / 10.0;
                }
                else
                {
                    m_gztemp = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                    static_cast<int>(pReceive.Data[4]))) / 10.0;
                }
                SystemTemperature.insert(
                    "standard",
                    QString::number(m_gztemp) +
                        "℃");
                updateTestItem();
                break;
            }
            case TestSetHALLCurrent:
            {
                emit stopTimerSignal();
                switch(step)
                {
                case 2:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 4:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 6:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 9:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 11:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 13:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 16:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 18:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 20:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 23:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 25:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 27:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 31:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 33:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 35:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 38:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 40:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 42:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                default:
                {
                    qDebug() << "[HALL] ignore unexpected frame while waiting for step"
                             << step << "locate" << m_locate
                             << "data0" << static_cast<int>(pReceive.Data[0]);
                    break;
                }
                }
                break;
            }
            case TestHALLCurrent:
            {
                emit stopTimerSignal();
                switch(step)
                {
                case 45:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 47:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 49:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 52:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 54:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 56:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                default:
                {
                    qDebug() << "[HALL] ignore unexpected frame while waiting for step"
                             << step << "locate" << m_locate
                             << "data0" << static_cast<int>(pReceive.Data[0]);
                    break;
                }
                }
                break;
            }
            case TestZDResistance:
            {
                emit stopTimerSignal();
                if(step == 1 || step == 2)
                {
                    QTimer *tempTimer1 = new QTimer(this);
                    connect(tempTimer1, SIGNAL(timeout()), this,
                            SLOT(slotCheckIfUpdate()));
                    tempTimer1->setSingleShot(true);
                    tempTimer1->start(5000);
                }
                break;
            }
            default:
                break;
            }
        }
    }
    else
    {
        if(static_cast<int>(pReceive.ID) == id6)
        {
            emit stopTimerSignal();
            if (pReceive.Data[0] == 2)
            {
                float k = ((static_cast<int>(pReceive.Data[1]) << 8) +
                           static_cast<int>(pReceive.Data[2])) /
                          1000.0;
                float b = ((static_cast<int>(pReceive.Data[3]) << 8) +
                           static_cast<int>(pReceive.Data[4])) /
                          1000.0;
                if (b > 65.535 / 2)
                {
                    b = b - 65.535;
                }
                else if (b == 65.535)
                {
                    b = 0;
                }
                switch (m_type)
                {
                case 0:
                case 1:
                {
                    ExcelItem temp;
                    temp.row  = 78;
                    temp.col  = 15;
                    temp.word = QString::number(k, 'f', 3);
                    items.push_back(temp);
                    temp.row  = 79;
                    temp.col  = 15;
                    temp.word = QString::number(b, 'f', 3);
                    items.push_back(temp);
                    break;
                }
                default:
                    break;
                }
                updateTestItem();
            }
            else if (pReceive.Data[0] == 3)
            {
                float k = ((static_cast<int>(pReceive.Data[1]) << 8) +
                           static_cast<int>(pReceive.Data[2])) /
                          1000.0;
                float b = ((static_cast<int>(pReceive.Data[3]) << 8) +
                           static_cast<int>(pReceive.Data[4])) /
                          1000.0;
                if (b > 65.535 / 2)
                {
                    b = b - 65.535;
                }
                else if (b == 65.535)
                {
                    b = 0;
                }
                switch (m_type)
                {
                case 0:
                case 1:
                {
                    ExcelItem temp;
                    temp.row  = 80;
                    temp.col  = 15;
                    temp.word = QString::number(k, 'f', 3);
                    items.push_back(temp);
                    temp.row  = 81;
                    temp.col  = 15;
                    temp.word = QString::number(b, 'f', 3);
                    items.push_back(temp);
                    break;
                }
                default:
                    break;
                }
                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            }
        }
        else if(static_cast<int>(pReceive.ID) == id3)
        {
            if (7 == static_cast<int>(pReceive.Data[0]) &&
                lastTestType == TestReadMAC)
            {
                emit stopTimerSignal();
                QString tempString = QString::number(
                    static_cast<int>(pReceive.Data[1]), 16);
                QString mac = tempString.toUpper();
                if (mac.length() == 1)
                {
                    mac = "0" + mac;
                }
                mac.append(QByteArray(":", sizeof(":")-1));
                tempString = QString::number(
                    static_cast<int>(pReceive.Data[2]), 16);
                if (tempString.length() == 1)
                {
                    tempString = "0" + tempString;
                }
                mac.append(tempString.toUpper()).append(QByteArray(":", sizeof(":")-1));
                tempString = QString::number(
                    static_cast<int>(pReceive.Data[3]), 16);
                if (tempString.length() == 1)
                {
                    tempString = "0" + tempString;
                }
                mac.append(tempString.toUpper()).append(QByteArray(":", sizeof(":")-1));
                tempString = QString::number(
                    static_cast<int>(pReceive.Data[4]), 16);
                if (tempString.length() == 1)
                {
                    tempString = "0" + tempString;
                }
                mac.append(tempString.toUpper()).append(QByteArray(":", sizeof(":")-1));
                tempString = QString::number(
                    static_cast<int>(pReceive.Data[5]), 16);
                if (tempString.length() == 1)
                {
                    tempString = "0" + tempString;
                }
                mac.append(tempString.toUpper()).append(QByteArray(":", sizeof(":")-1));
                tempString = QString::number(
                    static_cast<int>(pReceive.Data[6]), 16);
                if (tempString.length() == 1)
                {
                    tempString = "0" + tempString;
                }
                mac.append(tempString.toUpper());
                switch (m_type)
                {
                case 0:
                case 1:
                {
                    ExcelItem temp;
                    temp.row  = 54;
                    temp.col  = 15;
                    temp.word = mac;
                    items.push_back(temp);
                    if (m_macList.size() != 0)
                    {
                        qDebug() << "mac receive is " << mac
                                 << " and mac normal is "
                                 << m_macList.join(":");
                        temp.row  = 55;
                        temp.col  = 15;
                        temp.word = m_macList.join(":");
                        items.push_back(temp);
                        if (!mac.compare(m_macList.join(":"),
                                         Qt::CaseInsensitive))
                        {
                            temp.row  = 58;
                            temp.col  = 15;
                            temp.word = "通过";
                            items.push_back(temp);
                            emit singalReceiveMAC(mac);
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            temp.row  = 58;
                            temp.col  = 15;
                            temp.word = "未通过";
                            items.push_back(temp);
                            emit signalReceiveStatus("MAC地址设置失败");
                            step = 0;
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                    }
                    else
                    {
                        temp.row  = 58;
                        temp.col  = 15;
                        temp.word = "通过";
                        items.push_back(temp);
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
        else if(static_cast<int>(pReceive.ID) == id4)
        {
            if (lastTestType == TestSetSerial)
            {
                if (3 == static_cast<int>(pReceive.Data[0]))
                {
                    hardwareArray.clear();
                    hardwareArray
                        .append(static_cast<int>(pReceive.Data[0]))
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                }
                else if (4 == static_cast<int>(pReceive.Data[0]))
                {
                    emit stopTimerSignal();
                    nameArray.clear();
                    nameArray.append(pReceive.Data[0])
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    step = 1;
                    updateTestItem();
                }
                else if (5 == static_cast<int>(pReceive.Data[0]))
                {
                    nameArray.clear();
                    nameArray.append(pReceive.Data[0])
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                }
                else if (6 == static_cast<int>(pReceive.Data[0]))
                {
                    nameArray.append(pReceive.Data[0])
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                }
                else if (7 == static_cast<int>(pReceive.Data[0]))
                {
                    nameArray.append(pReceive.Data[0])
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                }
                else if (8 == static_cast<int>(pReceive.Data[0]))
                {
                    nameArray.append(pReceive.Data[0])
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                }
                else if (9 == static_cast<int>(pReceive.Data[0]))
                {
                    emit stopTimerSignal();
                    nameArray.append(pReceive.Data[0])
                        .append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    step = 1;
                    updateTestItem();
                }
            }
            else if (lastTestType == TestReadSerial)
            {
                if (1 == static_cast<int>(pReceive.Data[0]))
                {
                    receiveArray.clear();
                    receiveArray.append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                }
                else if (2 == static_cast<int>(pReceive.Data[0]))
                {
                    emit stopTimerSignal();
                    receiveArray.append(pReceive.Data[1])
                        .append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5]);
                    QByteArray tempArray = m_serial.toUtf8();
                    serial_receive = QString::fromUtf8(receiveArray);
                    step           = 0;
                    qDebug() << "receive serial is " << receiveArray
                             << " and set serial is " << tempArray;
                    if (m_serial == "")
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            temp.row  = 85;
                            temp.col  = 7;
                            temp.word = serial_receive;
                            items.push_back(temp);
                            temp.row  = 89;
                            temp.col  = 7;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        emit signalReceiveSerial(serial_receive);
                        QTimer *tempTimer = new QTimer(this);
                        connect(tempTimer, SIGNAL(timeout()), this,
                                SLOT(slotSendSuccess()));
                        tempTimer->setSingleShot(true);
                        tempTimer->start(2000);
                    }
                    else
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            temp.row  = 86;
                            temp.col  = 7;
                            temp.word = m_serial;
                            items.push_back(temp);
                            temp.row  = 85;
                            temp.col  = 7;
                            temp.word = serial_receive;
                            items.push_back(temp);
                            if (tempArray == receiveArray)
                            {
                                temp.row  = 89;
                                temp.col  = 7;
                                temp.word = "通过";
                                items.push_back(temp);
                                emit signalReceiveSerial(
                                    serial_receive);
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                temp.row  = 89;
                                temp.col  = 7;
                                temp.word = "未通过";
                                items.push_back(temp);
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                emit signalReceiveStatus("序列号设置"
                                                         "失败");
                            }
                            break;
                        }
                        default:
                            break;
                        }
                    }
                }
            }
            else if (lastTestType == TestHardWareVersion)
            {
                if (3 == static_cast<int>(pReceive.Data[0]))
                {
                    emit stopTimerSignal();
                    QStringList list = m_hardware.split(".");
                    switch (m_type)
                    {
                    case 0:
                    case 1:
                    {
                        ExcelItem temp;
                        temp.row = 61;
                        temp.col = 11;
                        temp.word =
                            "HV" +
                            QString::number(
                                static_cast<int>(pReceive.Data[1]))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(QString::number(
                                    static_cast<int>(pReceive.Data[2])))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(
                                    QString::number(static_cast<int>(
                                        pReceive.Data[3])));
                        items.push_back(temp);
                        temp.row  = 62;
                        temp.col  = 11;
                        temp.word = "HV" + m_hardware;
                        items.push_back(temp);
                        if (list.at(0).toInt() ==
                                static_cast<int>(pReceive.Data[1]) &&
                            list.at(1).toInt() ==
                                static_cast<int>(pReceive.Data[2]) &&
                            list.at(2).toInt() ==
                                static_cast<int>(pReceive.Data[3]))
                        {
                            temp.row  = 65;
                            temp.col  = 11;
                            temp.word = "通过";
                            items.push_back(temp);
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            temp.row  = 65;
                            temp.col  = 11;
                            temp.word = "未通过";
                            items.push_back(temp);
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
        }
        else if(static_cast<int>(pReceive.ID) == id5)
        {
            int a = static_cast<int>(pReceive.Data[0]);
            int b = (static_cast<int>(pReceive.Data[1]) << 8) +
                    static_cast<int>(pReceive.Data[2]);
            emit stopTimerSignal();
            emit signalGetInfo(a, b);
        }
        else if(static_cast<int>(pReceive.ID) == id7)
        {
            float k = ((static_cast<int>(pReceive.Data[2]) << 8) +
                       static_cast<int>(pReceive.Data[3])) /
                      1000.0;
            float b = ((static_cast<int>(pReceive.Data[4]) << 8) +
                       static_cast<int>(pReceive.Data[5])) /
                      1000.0;
            emit stopTimerSignal();
            if (b > 65.535 / 2)
            {
                b = b - 65.535;
            }
            else if (b == 65.535)
            {
                b = 0;
            }
            emit signalGetKB(k, b);
        }
        else if(static_cast<int>(pReceive.ID) == id8)
        {
            if (lastTestType == TestSoftWareVersion)
            {
                QStringList fullList = m_software.split("-");
                if (fullList.size() == 1)
                {
                    if (1 == static_cast<int>(pReceive.Data[0]))
                    {
                        emit stopTimerSignal();
                        QStringList list = fullList.at(2).split(".");
                        if (list.size() == 3)
                        {
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                ExcelItem temp;
                                temp.row = 55;
                                temp.col = 11;
                                temp.word =
                                    "SV:C-3-" +
                                    QString::number(
                                        static_cast<int>(
                                            pReceive.Data[4]))
                                        .append(QByteArray(".", sizeof(".")-1))
                                        .append(QString::number(
                                            static_cast<int>(
                                                pReceive.Data[5])))
                                        .append(QByteArray(".", sizeof(".")-1))
                                        .append(QString::number(
                                            static_cast<int>(
                                                pReceive.Data[6])));
                                items.push_back(temp);
                                temp.row  = 56;
                                temp.col  = 11;
                                temp.word = "SV:C-3-" + m_software;
                                items.push_back(temp);
                                if (list.at(0).toInt() ==
                                        static_cast<int>(
                                            pReceive.Data[4]) &&
                                    list.at(1).toInt() ==
                                        static_cast<int>(
                                            pReceive.Data[5]) &&
                                    list.at(2).toInt() ==
                                        static_cast<int>(
                                            pReceive.Data[6]) &&
                                    0x43 == static_cast<int>(
                                                pReceive.Data[2]) &&
                                    3 == static_cast<int>(
                                             pReceive.Data[3]))
                                {
                                    temp.row  = 59;
                                    temp.col  = 11;
                                    temp.word = "通过";
                                    items.push_back(temp);
                                    emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                                else
                                {
                                    temp.row  = 59;
                                    temp.col  = 11;
                                    temp.word = "未通"
                                                "过";
                                    items.push_back(temp);
                                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                                break;
                            }
                            default:
                                break;
                            }
                        }
                        else if (list.size() == 4)
                        {
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                ExcelItem temp;
                                temp.row = 55;
                                temp.col = 11;
                                temp.word =
                                    "SV:C-3-" +
                                    QString::number(
                                        static_cast<int>(
                                            pReceive.Data[4]))
                                        .append(QByteArray(".", sizeof(".")-1))
                                        .append(QString::number(
                                            static_cast<int>(
                                                pReceive.Data[5])))
                                        .append(QByteArray(".", sizeof(".")-1))
                                        .append(QString::number(
                                            static_cast<int>(
                                                pReceive.Data[6])))
                                        .append(QByteArray(".", sizeof(".")-1))
                                        .append(QString::number(
                                            static_cast<int>(
                                                pReceive.Data[7])));
                                items.push_back(temp);
                                temp.row  = 56;
                                temp.col  = 11;
                                temp.word = "SV:C-3-" + m_software;
                                items.push_back(temp);
                                if (list.at(0).toInt() ==
                                        static_cast<int>(
                                            pReceive.Data[4]) &&
                                    list.at(1).toInt() ==
                                        static_cast<int>(
                                            pReceive.Data[5]) &&
                                    list.at(2).toInt() ==
                                        static_cast<int>(
                                            pReceive.Data[6]) &&
                                    list.at(3).toInt() ==
                                        static_cast<int>(
                                            pReceive.Data[7]) &&
                                    0x43 == static_cast<int>(
                                                pReceive.Data[2]) &&
                                    3 == static_cast<int>(
                                             pReceive.Data[3]))
                                {
                                    temp.row  = 59;
                                    temp.col  = 11;
                                    temp.word = "通过";
                                    items.push_back(temp);
                                    emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                                else
                                {
                                    temp.row  = 59;
                                    temp.col  = 11;
                                    temp.word = "未通"
                                                "过";
                                    items.push_back(temp);
                                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                                break;
                            }
                            default:
                                break;
                            }
                        }
                    }
                }
                else if (fullList.size() == 2)
                {
                    if (1 == static_cast<int>(pReceive.Data[0]))
                    {
                        QStringList list = fullList.at(2).split(".");
                        m_tempSoft =
                            "SV:C-3-" +
                            QString::number(
                                static_cast<int>(pReceive.Data[4]))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(QString::number(
                                    static_cast<int>(pReceive.Data[5])))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(QString::number(
                                    static_cast<int>(pReceive.Data[6])))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(
                                    QString::number(static_cast<int>(
                                        pReceive.Data[7]))) +
                            "-";
                    }
                    else if (2 == static_cast<int>(pReceive.Data[0]))
                    {
                        emit stopTimerSignal();
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            temp.row = 55;
                            temp.col = 11;
                            m_tempSoft =
                                m_tempSoft +
                                QString((char)(static_cast<int>(
                                    pReceive.Data[2]))) +
                                QString((char)(static_cast<int>(
                                    pReceive.Data[3]))) +
                                QString::number(
                                    static_cast<int>(pReceive.Data[4]))
                                    .append(QByteArray(".", sizeof(".")-1))
                                    .append(QString::number(
                                        static_cast<int>(
                                            pReceive.Data[5])));
                            temp.word = m_tempSoft;
                            items.push_back(temp);
                            temp.row  = 56;
                            temp.col  = 11;
                            temp.word = "SV:C-3-" + m_software;
                            items.push_back(temp);
                            if (m_tempSoft == "SV:C-3-" + m_software)
                            {
                                temp.row  = 59;
                                temp.col  = 11;
                                temp.word = "通过";
                                items.push_back(temp);
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                temp.row  = 59;
                                temp.col  = 11;
                                temp.word = "未通过";
                                items.push_back(temp);
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            break;
                        }
                        default:
                            break;
                        }
                    }
                }
                else if (fullList.size() == 3)
                {
                    if (1 == static_cast<int>(pReceive.Data[0]))
                    {
                        emit stopTimerSignal();
                        QStringList list = fullList.at(2).split(".");
                        if (list.size() == 3)
                        {
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                ExcelItem temp;
                                temp.row = 55;
                                temp.col = 11;
                                temp.word =
                                    "SV:C-3-" +
                                    QString::number(
                                        static_cast<int>(
                                            pReceive.Data[4]))
                                        .append(QByteArray(".", sizeof(".")-1))
                                        .append(QString::number(
                                            static_cast<int>(
                                                pReceive.Data[5])))
                                        .append(QByteArray(".", sizeof(".")-1))
                                        .append(QString::number(
                                            static_cast<int>(
                                                pReceive.Data[6])));
                                items.push_back(temp);
                                temp.row  = 56;
                                temp.col  = 11;
                                temp.word = "SV:" + m_software;
                                items.push_back(temp);
                                if (list.at(0).toInt() ==
                                        static_cast<int>(
                                            pReceive.Data[4]) &&
                                    list.at(1).toInt() ==
                                        static_cast<int>(
                                            pReceive.Data[5]) &&
                                    list.at(2).toInt() ==
                                        static_cast<int>(
                                            pReceive.Data[6]) &&
                                    0x43 == static_cast<int>(
                                                pReceive.Data[2]) &&
                                    3 == static_cast<int>(
                                             pReceive.Data[3]))
                                {
                                    temp.row  = 59;
                                    temp.col  = 11;
                                    temp.word = "通过";
                                    items.push_back(temp);
                                    emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                                else
                                {
                                    temp.row  = 59;
                                    temp.col  = 11;
                                    temp.word = "未通"
                                                "过";
                                    items.push_back(temp);
                                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                                break;
                            }
                            default:
                                break;
                            }
                        }
                        else if (list.size() == 4)
                        {
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                ExcelItem temp;
                                temp.row = 55;
                                temp.col = 11;
                                temp.word =
                                    "SV:C-3-" +
                                    QString::number(
                                        static_cast<int>(
                                            pReceive.Data[4]))
                                        .append(QByteArray(".", sizeof(".")-1))
                                        .append(QString::number(
                                            static_cast<int>(
                                                pReceive.Data[5])))
                                        .append(QByteArray(".", sizeof(".")-1))
                                        .append(QString::number(
                                            static_cast<int>(
                                                pReceive.Data[6])))
                                        .append(QByteArray(".", sizeof(".")-1))
                                        .append(QString::number(
                                            static_cast<int>(
                                                pReceive.Data[7])));
                                items.push_back(temp);
                                temp.row  = 56;
                                temp.col  = 11;
                                temp.word = "SV:" + m_software;
                                items.push_back(temp);
                                if (list.at(0).toInt() ==
                                        static_cast<int>(
                                            pReceive.Data[4]) &&
                                    list.at(1).toInt() ==
                                        static_cast<int>(
                                            pReceive.Data[5]) &&
                                    list.at(2).toInt() ==
                                        static_cast<int>(
                                            pReceive.Data[6]) &&
                                    list.at(3).toInt() ==
                                        static_cast<int>(
                                            pReceive.Data[7]) &&
                                    0x43 == static_cast<int>(
                                                pReceive.Data[2]) &&
                                    3 == static_cast<int>(
                                             pReceive.Data[3]))
                                {
                                    temp.row  = 59;
                                    temp.col  = 11;
                                    temp.word = "通过";
                                    items.push_back(temp);
                                    emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                                else
                                {
                                    temp.row  = 59;
                                    temp.col  = 11;
                                    temp.word = "未通"
                                                "过";
                                    items.push_back(temp);
                                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                                }
                                break;
                            }
                            default:
                                break;
                            }
                        }
                    }
                }
                else if (fullList.size() == 4)
                {
                    if (1 == static_cast<int>(pReceive.Data[0]))
                    {
                        QStringList list = fullList.at(2).split(".");
                        m_tempSoft =
                            "SV:C-3-" +
                            QString::number(
                                static_cast<int>(pReceive.Data[4]))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(QString::number(
                                    static_cast<int>(pReceive.Data[5])))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(QString::number(
                                    static_cast<int>(pReceive.Data[6])))
                                .append(QByteArray(".", sizeof(".")-1))
                                .append(
                                    QString::number(static_cast<int>(
                                        pReceive.Data[7]))) +
                            "-";
                    }
                    else if (2 == static_cast<int>(pReceive.Data[0]))
                    {
                        emit stopTimerSignal();
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            temp.row = 55;
                            temp.col = 11;
                            m_tempSoft =
                                m_tempSoft +
                                QString((char)(static_cast<int>(
                                    pReceive.Data[2]))) +
                                QString((char)(static_cast<int>(
                                    pReceive.Data[3]))) +
                                QString::number(
                                    static_cast<int>(pReceive.Data[4]))
                                    .append(QByteArray(".", sizeof(".")-1))
                                    .append(QString::number(
                                        static_cast<int>(
                                            pReceive.Data[5])));
                            temp.word = m_tempSoft;
                            items.push_back(temp);
                            temp.row  = 56;
                            temp.col  = 11;
                            temp.word = "SV:" + m_software;
                            items.push_back(temp);
                            if (m_tempSoft == "SV:" + m_software)
                            {
                                temp.row  = 59;
                                temp.col  = 11;
                                temp.word = "通过";
                                items.push_back(temp);
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                temp.row  = 59;
                                temp.col  = 11;
                                temp.word = "未通过";
                                items.push_back(temp);
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            break;
                        }
                        default:
                            break;
                        }
                    }
                }
            }
        }
        else if(static_cast<int>(pReceive.ID) == id11)
        {
            if (lastTestType == TestRTC)
            {
                emit stopTimerSignal();
                QString year =
                    QDateTime::currentDateTime().toString("yyyy");
                QString mon =
                    QDateTime::currentDateTime().toString("MM");
                QString date =
                    QDateTime::currentDateTime().toString("dd");
                QString hour =
                    QDateTime::currentDateTime().toString("hh");
                QString min =
                    QDateTime::currentDateTime().toString("mm");
                QString sec =
                    QDateTime::currentDateTime().toString("ss");
                int year1 = (static_cast<int>(pReceive.Data[0]) << 8) +
                            static_cast<int>(pReceive.Data[1]);
                int mon1  = static_cast<int>(pReceive.Data[2]);
                int date1 = static_cast<int>(pReceive.Data[3]);
                int hour1 = static_cast<int>(pReceive.Data[4]);
                int min1  = static_cast<int>(pReceive.Data[5]);
                int sec1  = static_cast<int>(pReceive.Data[6]);
                QDate testDate(year1, mon1, date1);
                QTime testTime(hour1, min1, sec1);
                QDateTime testDateTime(testDate, testTime);
                QSettings setting(QDir::currentPath() + "/version.ini",
                                  QSettings::IniFormat);
                int wc  = setting.value("RTC").toString().toInt();
                switch (m_type)
                {
                case 0:
                case 1:
                {
                    ExcelItem temp;
                    temp.row  = 43;
                    temp.col  = 15;
                    temp.word = year + "-" + mon + "-" + date + " " +
                                hour + ":" + min + ":" + sec;
                    items.push_back(temp);
                    temp.row = 44;
                    temp.col = 15;
                    temp.word =
                        testDateTime.toString("yyyy-MM-dd hh:mm:ss");
                    items.push_back(temp);
                    temp.row  = 45;
                    temp.col  = 15;
                    temp.word = QString::number(testDateTime.secsTo(
                        QDateTime::currentDateTime()));
                    items.push_back(temp);
                    temp.row  = 46;
                    temp.col  = 15;
                    temp.word = "<=" + QString::number(wc) + "秒";
                    items.push_back(temp);
                    if (abs(testDateTime.secsTo(
                            QDateTime::currentDateTime())) <= wc)
                    {
                        step      = 0;
                        temp.row  = 47;
                        temp.col  = 15;
                        temp.word = "通过";
                        items.push_back(temp);
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    else
                    {
                        emit signalReceiveStatus("RTC读取错误");
                        temp.row  = 47;
                        temp.col  = 15;
                        temp.word = "未通过";
                        items.push_back(temp);
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        step = 0;
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
        else if(static_cast<int>(pReceive.ID) == id13)
        {
            switch (lastTestType)
            {
            case TestSetRTC:
            {
                emit startFailTimerSignal(20000);
                unsigned char data[8];
                data[0] = 0x12;
                data[1] = 0;
                data[2] = 0;
                data[3] = 0;
                data[4] = 0;
                data[5] = 0;
                data[6] = 0;
                data[7] = 0;
                TransmitCAN(((0x181F << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
                emit startTimerSignal(1000);
                emit signalReceiveStatus("RTC读取中。。。");
                break;
            }
            case TestSetBatteryVoltage:
            {
                emit stopTimerSignal();
                setFailTimer->stop();
                switch(step)
                {
                case 1:
                {
                    step = 2;
                    slotSetTestMod();
                    break;
                }
                case 2:
                {
                    QByteArray temp;
                    switch(m_locate)
                    {
                    case 3:
                    {
                        temp = QByteArray("\x01\x10\x10\x04\x00\x01\x02\x00\x01", 9);
                            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                        break;
                    }
                    case 7:
                    {
                        temp = QByteArray("\x05\x10\x10\x04\x00\x01\x02\x00\x01", 9);
                            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                        break;
                    }
                    case 11:
                    {
                        temp = QByteArray("\x09\x10\x10\x04\x00\x01\x02\x00\x01", 9);
                            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                        break;
                    }
                    case 15:
                    {
                        temp = QByteArray("\x0D\x10\x10\x04\x00\x01\x02\x00\x01", 9);
                            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                        break;
                    }
                    default:
                        break;
                    }
                    emit signalReceiveStatus("电池总压测试中。。。");
                    emit signalReceiveStatus("高压源开启中。。。");
                    QTimer *tempTimer = new QTimer(this);
                    connect(tempTimer, SIGNAL(timeout()), this,
                            SLOT(slotSetGYY()));
                    tempTimer->setSingleShot(true);
                    tempTimer->start(2000);
                    qDebug()  << " step at " << step << " finish send batteryVoltage";
                    break;
                }
                case 3:
                {
                    m_dcvz1 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    slotCheckIfUpdate();
                    break;
                }
                case 5:
                {
                    m_dcvy1 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    slotCheckIfUpdate();
                    break;
                }
                case 8:
                {
                    m_dcvz2 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    slotCheckIfUpdate();
                    break;
                }
                case 10:
                {
                    m_dcvy2 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    slotCheckIfUpdate();
                    break;
                }
                case 11:
                {
                    break;
                }
                case 14:
                {
                    m_dcvz3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    ExcelItem temp;
                    temp.row  = 6;
                    temp.col  = 3;
                    temp.word = QString::number(m_gzvz3, 'f', 1) + "V";
                    items.push_back(temp);
                    temp.row  = 7;
                    temp.col  = 3;
                    temp.word = QString::number(m_dcvz3, 'f', 1) + "V";
                    items.push_back(temp);
                    temp.row  = 8;
                    temp.col  = 3;
                    temp.word = QString::number((m_dcvz3 - m_gzvz3), 'f', 1) + "V";
                    items.push_back(temp);
                    temp.row  = 9;
                    temp.col  = 3;
                    temp.word = "±2V";
                    items.push_back(temp);
                    if(2 < abs(m_dcvz3 - m_gzvz3))
                    {
                        temp.row  = 10;
                        temp.col  = 3;
                        temp.word = "未通过";
                        items.push_back(temp);
                        emit signalReceiveStatus(
                            "电压偏差过大！要求为±2V，实际为" +
                            QString::number(m_dcvz3 - m_gzvz3) + "V");
                        ifVPass = false;
                    }
                    else
                    {
                        temp.row  = 10;
                        temp.col  = 3;
                        temp.word = "通过";
                        items.push_back(temp);
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 16:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_dcvy3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        ExcelItem temp;
                        temp.row  = 91;
                        temp.col  = 3;
                        temp.word = QString::number(m_gzvy3, 'f', 1) + "V";
                        items.push_back(temp);
                        temp.row  = 92;
                        temp.col  = 3;
                        temp.word = QString::number(m_dcvy3, 'f', 1) + "V";
                        items.push_back(temp);
                        temp.row  = 93;
                        temp.col  = 3;
                        temp.word = QString::number((m_dcvy3 - m_gzvy3), 'f', 1) + "V";
                        items.push_back(temp);
                        temp.row  = 94;
                        temp.col  = 3;
                        temp.word = "±2V";
                        items.push_back(temp);
                        if(2 < abs(m_dcvy3 - m_gzvy3))
                        {
                            temp.row  = 95;
                            temp.col  = 3;
                            temp.word = "未通过";
                            items.push_back(temp);
                            emit signalReceiveStatus(
                                "电压偏差过大！要求为±2V，实际为" +
                                QString::number(m_dcvy3 - m_gzvy3) + "V");
                            ifVPass = false;
                        }
                        else
                        {
                            temp.row  = 95;
                            temp.col  = 3;
                            temp.word = "通过";
                            items.push_back(temp);
                        }
                        if (ifVPass)
                        {
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                    }
                    break;
                }
                default:
                    break;
                }
            }
            case TestZDResistance:
            {
                emit stopTimerSignal();
                setFailTimer->stop();
                switch(step)
                {
                case 5:
                {
                    step = 6;
                    slotSetTestMod();
                    break;
                }
                case 6:
                {
                    if(if_zdPass)
                    {
                        step = 0;
                        emit stopFailTimerSignal();
                        emit signalFinish();timeoutTimer.stop();
                    }
                    else
                    {
                        step = 0;
                        emit stopFailTimerSignal();
                        emit signalErrorFinish();timeoutTimer.stop();
                    }
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case TestBatteryVoltage:
            {
                emit stopTimerSignal();
                switch(step)
                {
                case 19:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_dcvz3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        ExcelItem temp;
                        temp.row  = 60;
                        temp.col  = 15;
                        temp.word = QString::number(m_gzvz3, 'f', 1) + "V";
                        items.push_back(temp);
                        temp.row  = 61;
                        temp.col  = 15;
                        temp.word = QString::number(m_dcvz3, 'f', 1) + "V";
                        items.push_back(temp);
                        temp.row  = 62;
                        temp.col  = 15;
                        temp.word = QString::number((m_dcvz3 - m_gzvz3), 'f', 1) + "V";
                        items.push_back(temp);
                        temp.row  = 63;
                        temp.col  = 15;
                        temp.word = "±2V";
                        items.push_back(temp);
                        if(2 < abs(m_dcvz3 - m_gzvz3))
                        {
                            temp.row  = 64;
                            temp.col  = 15;
                            temp.word = "未通过";
                            items.push_back(temp);
                            emit signalReceiveStatus(
                                "电压偏差过大！要求为±2V，实际为" +
                                QString::number(m_dcvz3 - m_gzvz3) + "V");
                            ifVPass = false;
                        }
                        else
                        {
                            temp.row  = 64;
                            temp.col  = 15;
                            temp.word = "通过";
                            items.push_back(temp);
                        }
                        slotCheckIfUpdate();
                    }
                    break;
                }
                case 21:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_dcvy3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        ExcelItem temp;
                        temp.row  = 79;
                        temp.col  = 11;
                        temp.word = QString::number(m_gzvy3, 'f', 1) + "V";
                        items.push_back(temp);
                        temp.row  = 80;
                        temp.col  = 11;
                        temp.word = QString::number(m_dcvy3, 'f', 1) + "V";
                        items.push_back(temp);
                        temp.row  = 81;
                        temp.col  = 11;
                        temp.word = QString::number((m_dcvy3 - m_gzvy3), 'f', 1) + "V";
                        items.push_back(temp);
                        temp.row  = 82;
                        temp.col  = 11;
                        temp.word = "±2V";
                        items.push_back(temp);
                        if(2 < abs(m_dcvy3 - m_gzvy3))
                        {
                            temp.row  = 83;
                            temp.col  = 11;
                            temp.word = "未通过";
                            items.push_back(temp);
                            emit signalReceiveStatus(
                                "电压偏差过大！要求为±2V，实际为" +
                                QString::number(m_dcvy3 - m_gzvy3) + "V");
                            ifVPass = false;
                        }
                        else
                        {
                            temp.row  = 83;
                            temp.col  = 11;
                            temp.word = "通过";
                            items.push_back(temp);
                        }
                        if (ifVPass)
                        {
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                    }
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case TestSupplyVoltage:
            {
                emit stopTimerSignal();
                if(pReceive.Data[0] == 0x0C)
                {
                    m_dcgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    updateTestItem();
                }
                break;
            }
            case TestInsulationResistance:
            {
                emit stopTimerSignal();
                m_dcjyf = ((static_cast<int>(pReceive.Data[1]) << 8) +
                        static_cast<int>(pReceive.Data[2]));
                m_dcjyz = ((static_cast<int>(pReceive.Data[3]) << 8) +
                        static_cast<int>(pReceive.Data[4]));

                switch (m_type)
                {
                case 0:
                case 1:
                {
                    ExcelItem temp;
                    temp.row  = 6;
                    temp.col  = 7;
                    temp.word = "正线" + QString::number(m_dcjyz) +
                                "kΩ/负线" + QString::number(m_dcjyf) +
                                "kΩ";
                    items.push_back(temp);
                    temp.row  = 7;
                    temp.col  = 7;
                    temp.word = "1000kΩ/500kΩ";
                    items.push_back(temp);
                    temp.row  = 8;
                    temp.col  = 7;
                    temp.word = "正线" +
                                QString::number(m_dcjyz - 1000) +
                                "kΩ/负线" +
                                QString::number(m_dcjyf - 500) + "kΩ";
                    items.push_back(temp);
                    temp.row  = 9;
                    temp.col  = 7;
                    temp.word = "正线大于850kΩ小于1150kΩ/负线大于425kΩ小于575kΩ";
                    items.push_back(temp);
                    if (1150 >= m_dcjyz && 850 <= m_dcjyz &&
                        575 >= m_dcjyf && 425 <= m_dcjyf)
                    {
                        step = 0;
                        temp.row  = 10;
                        temp.col  = 7;
                        temp.word = "通过";
                        items.push_back(temp);
                        QByteArray temp;
                        switch(m_locate)
                        {
                        case 3:
                        {
                            temp = QByteArray("\x01\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                            break;
                        }
                        case 7:
                        {
                            temp = QByteArray("\x05\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                            break;
                        }
                        case 11:
                        {
                            temp = QByteArray("\x09\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                            break;
                        }
                        case 15:
                        {

                            temp = QByteArray("\x0D\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                            break;
                        }
                        default:
                            break;
                        }
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    else
                    {
                        step = 0;
                        temp.row  = 10;
                        temp.col  = 7;
                        temp.word = "未通过";
                        items.push_back(temp);
                        step = 0;
                        QByteArray temp;
                        switch(m_locate)
                        {
                        case 3:
                        {
                            temp = QByteArray("\x01\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                            break;
                        }
                        case 7:
                        {
                            temp = QByteArray("\x05\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                            break;
                        }
                        case 11:
                        {
                            temp = QByteArray("\x09\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                            break;
                        }
                        case 15:
                        {
                            temp = QByteArray("\x0D\x10\x10\x00\x00\x01\x02\x00\x05", 9);
                                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
                            break;
                        }
                        default:
                            break;
                        }
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case TestTemperature:
            {
                emit stopTimerSignal();
                switch(step)
                {
                case 1:
                {
                    if((static_cast<int>(pReceive.Data[3]) << 8) +
                            static_cast<int>(pReceive.Data[4]) > 32767)
                    {
                        m_temp1 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                        static_cast<int>(pReceive.Data[4])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_temp1 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                        static_cast<int>(pReceive.Data[4]))) / 10.0;
                    }
                    if(m_temp1 < 10)
                    {
                        m_temp1 = static_cast<int>(pReceive.Data[4]) - 40;
                    }
                    updateTestItem();
                    break;
                }
                case 2:
                {
                    if((static_cast<int>(pReceive.Data[3]) << 8) +
                            static_cast<int>(pReceive.Data[4]) > 32767)
                    {
                        m_temp2 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                        static_cast<int>(pReceive.Data[4])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_temp2 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                        static_cast<int>(pReceive.Data[4]))) / 10.0;
                    }
                    if(m_temp2 < 10)
                    {
                        m_temp2 = static_cast<int>(pReceive.Data[4]) - 40;
                    }
                    updateTestItem();
                    break;
                }
                case 3:
                {
                    if((static_cast<int>(pReceive.Data[3]) << 8) +
                            static_cast<int>(pReceive.Data[4]) > 32767)
                    {
                        m_temp3 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                        static_cast<int>(pReceive.Data[4])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_temp3 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                        static_cast<int>(pReceive.Data[4]))) / 10.0;
                    }
                    if(m_temp3 < 10)
                    {
                        m_temp3 = static_cast<int>(pReceive.Data[4]) - 40;
                    }
                    updateTestItem();
                    break;
                }
                case 4:
                {
                    if((static_cast<int>(pReceive.Data[3]) << 8) +
                            static_cast<int>(pReceive.Data[4]) > 32767)
                    {
                        m_temp4 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                        static_cast<int>(pReceive.Data[4])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_temp4 = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                        static_cast<int>(pReceive.Data[4]))) / 10.0;
                    }
                    if(m_temp4 < 10)
                    {
                        m_temp4 = static_cast<int>(pReceive.Data[4]) - 40;
                    }
                    switch (m_type)
                    {
                    case 0:
                    case 1:
                    {
                        ExcelItem temp;
                        temp.row = 13;
                        temp.col = 6;
                        temp.word =
                            QString::number(
                                m_temp1) +
                            "℃";
                        items.push_back(temp);
                        temp.row = 19;
                        temp.col = 6;
                        temp.word =
                            QString::number(
                                m_temp3) +
                            "℃";
                        items.push_back(temp);
                        temp.row = 13;
                        temp.col = 8;
                        temp.word =
                            QString::number(
                                m_temp2) +
                            "℃";
                        items.push_back(temp);
                        temp.row = 19;
                        temp.col = 8;
                        temp.word =
                            QString::number(
                                m_temp4) +
                            "℃";
                        items.push_back(temp);
                        temp.row  = 14;
                        temp.col  = 6;
                        temp.word = "25℃";
                        items.push_back(temp);
                        temp.row  = 20;
                        temp.col  = 6;
                        temp.word = "25℃";
                        items.push_back(temp);
                        items.push_back(temp);
                        temp.row  = 14;
                        temp.col  = 8;
                        temp.word = "54℃";
                        items.push_back(temp);
                        temp.row  = 20;
                        temp.col  = 8;
                        temp.word = "54℃";
                        items.push_back(temp);
                        temp.row = 15;
                        temp.col = 6;
                        temp.word =
                            QString::number(
                                25 -
                                m_temp1) +
                            "℃";
                        items.push_back(temp);
                        temp.row = 21;
                        temp.col = 6;
                        temp.word =
                            QString::number(
                                25 -
                                m_temp3) +
                            "℃";
                        items.push_back(temp);
                        temp.row = 15;
                        temp.col = 8;
                        temp.word =
                            QString::number(
                                54 -
                                m_temp2) +
                            "℃";
                        items.push_back(temp);
                        temp.row = 21;
                        temp.col = 8;
                        temp.word =
                            QString::number(
                                54 -
                                m_temp4) +
                            "℃";
                        items.push_back(temp);
                        temp.row  = 16;
                        temp.col  = 6;
                        temp.word = "±1℃";
                        items.push_back(temp);
                        temp.row  = 22;
                        temp.col  = 6;
                        temp.word = "±1℃";
                        items.push_back(temp);
                        temp.row  = 16;
                        temp.col  = 8;
                        temp.word = "±1℃";
                        items.push_back(temp);
                        temp.row  = 22;
                        temp.col  = 8;
                        temp.word = "±1℃";
                        items.push_back(temp);
                        if (1 >= abs(25 - m_temp1) &&
                            1 >= abs(25 - m_temp3) &&
                            1 >= abs(54 - m_temp2) &&
                            1 >= abs(54 - m_temp4))
                        {
                            step = 0;
                            temp.row  = 17;
                            temp.col  = 6;
                            temp.word = "通过";
                            items.push_back(temp);
                            temp.row  = 17;
                            temp.col  = 8;
                            temp.word = "通过";
                            items.push_back(temp);
                            temp.row  = 23;
                            temp.col  = 6;
                            temp.word = "通过";
                            items.push_back(temp);
                            temp.row  = 23;
                            temp.col  = 8;
                            temp.word = "通过";
                            items.push_back(temp);
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else if (1 < abs(25 - m_temp1))
                        {
                            emit signalReceiveStatus(
                                "T0温度错误，实际"
                                "为" +
                                QString::number(m_temp1) +
                                "度");
                            step      = 0;
                            temp.row  = 17;
                            temp.col  = 6;
                            temp.word = "未通过";
                            items.push_back(temp);

                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else if (1 < abs(25 - m_temp3))
                        {
                            temp.row  = 23;
                            temp.col  = 6;
                            temp.word = "未通过";
                            items.push_back(temp);
                            emit signalReceiveStatus(
                                "T2温度错误，实际"
                                "为" +
                                QString::number(m_temp3) +
                                "度");
                            step = 0;
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else if (1 < abs(54 - m_temp2))
                        {
                            temp.row  = 17;
                            temp.col  = 8;
                            temp.word = "未通过";
                            items.push_back(temp);
                            emit signalReceiveStatus(
                                "T1温度错误，实际"
                                "为" +
                                QString::number(m_temp2) +
                                "度");
                            step = 0;
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else if (1 < abs(54 - m_temp4))
                        {
                            temp.row  = 23;
                            temp.col  = 8;
                            temp.word = "未通过";
                            items.push_back(temp);
                            emit signalReceiveStatus(
                                "T3温度错误，实际"
                                "为" +
                                QString::number(m_temp4) +
                                "度");
                            step = 0;
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        break;
                    }
                    default:
                        break;
                    }
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case TestEncodeInterface:
            {
                emit stopTimerSignal();
                switch(step)
                {
                case 1:
                {
                    updateTestItem();
                    break;
                }
                case 2:
                {
                    updateTestItem();
                    break;
                }
                case 3:
                {
                    updateTestItem();
                    break;
                }
                case 4:
                {
                    if(static_cast<int>(pReceive.Data[4]) == 1)
                    {
                        updateTestItem();
                    }
                    else
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            emit signalReceiveStatus("设置高电平失败");
                            temp.row  = 23;
                            temp.col  = 3;
                            temp.word = "未通过";
                            items.push_back(temp);

                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            step = 0;
                            break;
                        }
                        default:
                            break;
                        }
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    break;
                }
                case 5:
                {
                    updateTestItem();
                    break;
                }
                case 6:
                {
                    if(static_cast<int>(pReceive.Data[4]) == 1)
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            emit signalReceiveStatus("设置低电平失败");
                            temp.row  = 23;
                            temp.col  = 3;
                            temp.word = "未通过";
                            items.push_back(temp);

                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            step = 0;
                            break;
                        }
                        default:
                            break;
                        }
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    else
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            step      = 0;
                            temp.row  = 19;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 20;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 21;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 22;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 23;
                            temp.col  = 3;
                            temp.word = "通过";
                            items.push_back(temp);
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            break;
                        }
                        default:
                            break;
                        }
                    }
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case TestDO:
            {
                switch (step)
                {
                case 2:
                {
                    ExcelItem temp;
                    if (1 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO1L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 4:
                {
                    if (0 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO1L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 6:
                {
                    if (1 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO2L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 8:
                {
                    if (0 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO2L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 10:
                {
                    if (1 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO3L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 12:
                {
                    if (0 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO3L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 14:
                {
                    if (1 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO4L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 16:
                {
                    if (0 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO4L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 18:
                {
                    if (1 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO5L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 20:
                {
                    if (0 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO5L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 22:
                {
                    if (1 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO6L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 24:
                {
                    if (0 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO6L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 26:
                {
                    if (1 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO7L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 28:
                {
                    if (0 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO7L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 30:
                {
                    if (1 == static_cast<int>(pReceive.Data[3]))
                    {
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO8L设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 32:
                {
                    if (0 == static_cast<int>(pReceive.Data[3]))
                    {
                        step = 0;
                        testResults.insert("DOTest", DOTest);
                        retryNumber = 0;
                        emit stopTimerSignal();
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DO8设置错误");
                            step = 0;
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                default:
                {
                    updateTestItem();
                    break;
                }
                }
                break;
            }
            case TestDIAndSLP:
            {
                ExcelItem temp;
                switch (step)
                {
                case 2:
                {
                    if (2 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            retryNumber = 0;
                            temp.row    = 25;
                            temp.col    = 7;
                            temp.word   = "1";
                            items.push_back(temp);
                            temp.row  = 27;
                            temp.col  = 7;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 29;
                            temp.col  = 7;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI1L设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 25;
                                temp.col    = 7;
                                temp.word   = "0";
                                items.push_back(temp);
                                temp.row  = 27;
                                temp.col  = 7;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 29;
                                temp.col  = 7;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 4:
                {
                    if (2 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            retryNumber = 0;
                            temp.row    = 25;
                            temp.col    = 3;
                            temp.word   = "0";
                            items.push_back(temp);
                            temp.row  = 27;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 29;
                            temp.col  = 3;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            case 15:
                            case 6:
                            case 30:
                            case 33:
                            case 9:
                            {
                                retryNumber = 0;
                                temp.row    = 25;
                                temp.col    = 3;
                                temp.word   = "1";
                                items.push_back(temp);
                                temp.row  = 27;
                                temp.col  = 3;
                                temp.word = "1";
                                items.push_back(temp);
                                temp.row  = 29;
                                temp.col  = 3;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI1L设置错误");
                            step = 0;

                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 6:
                {
                    if (4 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            retryNumber = 0;
                            temp.row    = 31;
                            temp.col    = 7;
                            temp.word   = "1";
                            items.push_back(temp);
                            temp.row  = 33;
                            temp.col  = 7;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 35;
                            temp.col  = 7;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI2L设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 31;
                                temp.col    = 7;
                                temp.word   = "0";
                                items.push_back(temp);
                                temp.row  = 33;
                                temp.col  = 7;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 35;
                                temp.col  = 7;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 8:
                {
                    if (4 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 15:
                        case 6:
                        case 30:
                        case 33:
                        case 9:
                        {
                            retryNumber = 0;
                            temp.row    = 31;
                            temp.col    = 3;
                            temp.word   = "0";
                            items.push_back(temp);
                            temp.row  = 33;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 35;
                            temp.col  = 3;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        case 1:
                        case 17:
                        case 4:
                        case 16:
                        case 7:
                        case 31:
                        case 34:
                        case 8:
                        case 32:
                        case 35:
                        case 10:
                        case 11:
                        {
                            retryNumber = 0;
                            temp.row    = 32;
                            temp.col    = 3;
                            temp.word   = "0";
                            items.push_back(temp);
                            temp.row  = 34;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 36;
                            temp.col  = 3;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        case 2:
                        case 36:
                        case 12:
                        case 18:
                        case 27:
                        case 21:
                        case 24:
                        {
                            retryNumber = 0;
                            temp.row    = 25;
                            temp.col    = 3;
                            temp.word   = "0";
                            items.push_back(temp);
                            temp.row  = 27;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 29;
                            temp.col  = 3;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        case 3:
                        case 38:
                        case 5:
                        case 37:
                        case 13:
                        case 19:
                        case 28:
                        case 22:
                        case 25:
                        case 14:
                        case 20:
                        case 29:
                        case 23:
                        case 26:
                        {
                            retryNumber = 0;
                            temp.row    = 26;
                            temp.col    = 3;
                            temp.word   = "0";
                            items.push_back(temp);
                            temp.row  = 28;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 30;
                            temp.col  = 3;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI2L设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 31;
                                temp.col    = 3;
                                temp.word   = "1";
                                items.push_back(temp);
                                temp.row  = 33;
                                temp.col  = 3;
                                temp.word = "1";
                                items.push_back(temp);
                                temp.row  = 35;
                                temp.col  = 3;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 10:
                {
                    if (6 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            retryNumber = 0;
                            temp.row    = 38;
                            temp.col    = 7;
                            temp.word   = "1";
                            items.push_back(temp);
                            temp.row  = 40;
                            temp.col  = 7;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 42;
                            temp.col  = 7;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI3L设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 37;
                                temp.col    = 7;
                                temp.word   = "0";
                                items.push_back(temp);
                                temp.row  = 39;
                                temp.col  = 7;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 41;
                                temp.col  = 7;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 12:
                {
                    if (6 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            retryNumber = 0;
                            temp.row    = 37;
                            temp.col    = 3;
                            temp.word   = "0";
                            items.push_back(temp);
                            temp.row  = 39;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 41;
                            temp.col  = 3;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI3L设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 37;
                                temp.col    = 3;
                                temp.word   = "1";
                                items.push_back(temp);
                                temp.row  = 39;
                                temp.col  = 3;
                                temp.word = "1";
                                items.push_back(temp);
                                temp.row  = 41;
                                temp.col  = 3;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 14:
                {
                    if (1 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            retryNumber = 0;
                            temp.row    = 43;
                            temp.col    = 7;
                            temp.word   = "1";
                            items.push_back(temp);
                            temp.row  = 45;
                            temp.col  = 7;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 47;
                            temp.col  = 7;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI4L设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 43;
                                temp.col    = 7;
                                temp.word   = "0";
                                items.push_back(temp);
                                temp.row  = 45;
                                temp.col  = 7;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 47;
                                temp.col  = 7;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 16:
                {
                    if (1 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        case 15:
                        {
                            retryNumber = 0;
                            temp.row    = 43;
                            temp.col    = 3;
                            temp.word   = "0";
                            items.push_back(temp);
                            temp.row  = 45;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 47;
                            temp.col  = 3;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI4L设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 43;
                                temp.col    = 3;
                                temp.word   = "1";
                                items.push_back(temp);
                                temp.row  = 45;
                                temp.col  = 3;
                                temp.word = "1";
                                items.push_back(temp);
                                temp.row  = 47;
                                temp.col  = 3;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 18:
                {
                    if (3 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            retryNumber = 0;
                            temp.row    = 49;
                            temp.col    = 7;
                            temp.word   = "1";
                            items.push_back(temp);
                            temp.row  = 51;
                            temp.col  = 7;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 53;
                            temp.col  = 7;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI5L设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 49;
                                temp.col    = 7;
                                temp.word   = "0";
                                items.push_back(temp);
                                temp.row  = 51;
                                temp.col  = 7;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 53;
                                temp.col  = 7;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 20:
                {
                    if (3 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            retryNumber = 0;
                            temp.row    = 49;
                            temp.col    = 3;
                            temp.word   = "0";
                            items.push_back(temp);
                            temp.row  = 51;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 53;
                            temp.col  = 3;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI5L设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 49;
                                temp.col    = 3;
                                temp.word   = "1";
                                items.push_back(temp);
                                temp.row  = 51;
                                temp.col  = 3;
                                temp.word = "1";
                                items.push_back(temp);
                                temp.row  = 53;
                                temp.col  = 3;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 22:
                {
                    if (5 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            retryNumber = 0;
                            temp.row    = 55;
                            temp.col    = 7;
                            temp.word   = "1";
                            items.push_back(temp);
                            temp.row  = 57;
                            temp.col  = 7;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 59;
                            temp.col  = 7;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI6L设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 55;
                                temp.col    = 7;
                                temp.word   = "0";
                                items.push_back(temp);
                                temp.row  = 57;
                                temp.col  = 7;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 59;
                                temp.col  = 7;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 24:
                {
                    if (5 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            retryNumber = 0;
                            temp.row    = 55;
                            temp.col    = 3;
                            temp.word   = "0";
                            items.push_back(temp);
                            temp.row  = 57;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 59;
                            temp.col  = 3;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI6L设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 55;
                                temp.col    = 3;
                                temp.word   = "1";
                                items.push_back(temp);
                                temp.row  = 57;
                                temp.col  = 3;
                                temp.word = "1";
                                items.push_back(temp);
                                temp.row  = 59;
                                temp.col  = 3;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 26:
                {
                    if (7 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            retryNumber = 0;
                            temp.row    = 61;
                            temp.col    = 7;
                            temp.word   = "1";
                            items.push_back(temp);
                            temp.row  = 63;
                            temp.col  = 7;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 65;
                            temp.col  = 7;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI7L设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 61;
                                temp.col    = 7;
                                temp.word   = "0";
                                items.push_back(temp);
                                temp.row  = 63;
                                temp.col  = 7;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 65;
                                temp.col  = 7;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 28:
                {
                    if (7 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            retryNumber = 0;
                            temp.row    = 61;
                            temp.col    = 3;
                            temp.word   = "0";
                            items.push_back(temp);
                            temp.row  = 63;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 65;
                            temp.col  = 3;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI7L设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 61;
                                temp.col    = 3;
                                temp.word   = "1";
                                items.push_back(temp);
                                temp.row  = 63;
                                temp.col  = 3;
                                temp.word = "1";
                                items.push_back(temp);
                                temp.row  = 65;
                                temp.col  = 3;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 30:
                {
                    if (8 == static_cast<int>(pReceive.Data[2]) && 1 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            retryNumber = 0;
                            temp.row    = 67;
                            temp.col    = 7;
                            temp.word   = "1";
                            items.push_back(temp);
                            temp.row  = 69;
                            temp.col  = 7;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 71;
                            temp.col  = 7;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (!testTimer)
                        {
                            testTimer = new QTimer();
                            testTimer->setSingleShot(true);
                            connect(testTimer,
                                    SIGNAL(timeout()), this,
                                    SLOT(updateTestItem()));
                        }
                        testTimer->start(100);
                        emit stopTimerSignal();
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI8L设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 67;
                                temp.col    = 7;
                                temp.word   = "0";
                                items.push_back(temp);
                                temp.row  = 69;
                                temp.col  = 7;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 71;
                                temp.col  = 7;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                case 32:
                {
                    if (8 == static_cast<int>(pReceive.Data[2]) && 0 == static_cast<int>(pReceive.Data[3]))
                    {
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            retryNumber = 0;
                            temp.row    = 67;
                            temp.col    = 3;
                            temp.word   = "0";
                            items.push_back(temp);
                            temp.row  = 69;
                            temp.col  = 3;
                            temp.word = "0";
                            items.push_back(temp);
                            temp.row  = 71;
                            temp.col  = 3;
                            temp.word = "通过";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        step = 0;
                        emit stopTimerSignal();
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    else
                    {
                        if (retryNumber >= 50)
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "DI8设置错误");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                retryNumber = 0;
                                temp.row    = 67;
                                temp.col    = 3;
                                temp.word   = "1";
                                items.push_back(temp);
                                temp.row  = 69;
                                temp.col  = 3;
                                temp.word = "1";
                                items.push_back(temp);
                                temp.row  = 71;
                                temp.col  = 3;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            retryNumber++;
                            step--;
                            updateTestItem();
                        }
                    }
                    break;
                }
                default:
                {
                    updateTestItem();
                    break;
                }
                }
                break;
            }
            case TestEEPROM:
            {
                emit stopTimerSignal();
                switch (m_type)
                {
                case 0:
                case 1:
                {
                    ExcelItem temp;
                    temp.row  = 79;
                    temp.col  = 3;
                    temp.word = QString::number(
                        static_cast<int>(pReceive.Data[2]));
                    items.push_back(temp);
                    temp.row  = 80;
                    temp.col  = 3;
                    temp.word = "0";
                    items.push_back(temp);
                    temp.row  = 81;
                    temp.col  = 3;
                    temp.word = "0";
                    items.push_back(temp);
                    temp.row  = 82;
                    temp.col  = 3;
                    temp.word = "0";
                    items.push_back(temp);
                    if (0 == static_cast<int>(pReceive.Data[2]))
                    {
                        step = 0;
                        temp.row  = 83;
                        temp.col  = 3;
                        temp.word = "通过";
                        items.push_back(temp);
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    else
                    {
                        emit signalReceiveStatus("EEPROM读取错"
                                                 "误");
                        temp.row = 83;
                        temp.col = 3;
                        temp.word = "未通过";
                        items.push_back(temp);
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        step = 0;
                    }
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case TestFlash:
            {
                emit stopTimerSignal();
                switch (m_type)
                {
                case 0:
                case 1:
                {
                    ExcelItem temp;
                    temp.row  = 6;
                    temp.col  = 11;
                    temp.word = QString::number(
                        static_cast<int>(pReceive.Data[2]));
                    items.push_back(temp);
                    temp.row  = 7;
                    temp.col  = 11;
                    temp.word = "0";
                    items.push_back(temp);
                    temp.row  = 8;
                    temp.col  = 11;
                    temp.word = "0";
                    items.push_back(temp);
                    temp.row  = 9;
                    temp.col  = 11;
                    temp.word = "0";
                    items.push_back(temp);
                    if (0 == static_cast<int>(pReceive.Data[2]))
                    {
                        step = 0;
                        temp.row  = 10;
                        temp.col  = 11;
                        temp.word = "通过";
                        items.push_back(temp);
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    else
                    {

                        emit signalReceiveStatus("FLASH读取错"
                                                 "误");
                        temp.row  = 10;
                        temp.col  = 11;
                        temp.word = "未通过";
                        items.push_back(temp);
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        step = 0;
                    }
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case TestSystemTemperature:
            {
                emit stopTimerSignal();
                if((static_cast<int>(pReceive.Data[3]) << 8) +
                        static_cast<int>(pReceive.Data[4]) > 32767)
                {
                    m_dctemp = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                    static_cast<int>(pReceive.Data[4])) - 65535) / 10.0;
                }
                else
                {
                    m_dctemp = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                    static_cast<int>(pReceive.Data[4]))) / 10.0;
                }
                switch (m_type)
                {
                case 0:
                case 1:
                {
                    ExcelItem temp;
                    temp.row = 79;
                    temp.col = 7;
                    temp.word =
                        QString::number(
                            m_gztemp) +
                        "℃";
                    items.push_back(temp);
                    temp.row = 80;
                    temp.col = 7;
                    temp.word =
                        QString::number(
                            m_dctemp) +
                        "℃";
                    items.push_back(temp);
                    temp.row = 81;
                    temp.col = 7;
                    temp.word =
                        QString::number(
                            m_gztemp -
                            m_dctemp) +
                        "℃";
                    items.push_back(temp);
                    temp.row  = 82;
                    temp.col  = 7;
                    temp.word = "±5℃";
                    items.push_back(temp);
                    qDebug()
                        << "xiangcha "
                        << QString::number(abs(
                               m_gztemp -
                               m_dctemp));
                    if (5 >=
                        abs(m_gztemp -
                            m_dctemp))
                    {
                        step = 0;
                        temp.row  = 83;
                        temp.col  = 7;
                        temp.word = "通过";
                        items.push_back(temp);
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    else
                    {
                        temp.row  = 83;
                        temp.col  = 7;
                        temp.word = "未通过";
                        items.push_back(temp);
                        emit signalReceiveStatus(
                            "系统温度测试错误，"
                            "工装板温度为" +
                            QString::number(m_gztemp) +
                            "待测板温度为" +
                            QString::number(
                                m_dctemp) +
                            "℃");
                        step = 0;
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case TestSPI:
            {
                emit stopTimerSignal();
                if (0 == static_cast<int>(pReceive.Data[1]))
                {
                    step        = 0;
                    retryNumber = 0;
                    ExcelItem temp;
                    switch (m_type)
                    {
                    case 0:
                    case 1:
                    {
                        temp.row  = 85;
                        temp.col  = 15;
                        temp.word = "0";
                        items.push_back(temp);
                        temp.row  = 86;
                        temp.col  = 15;
                        temp.word = "0";
                        items.push_back(temp);
                        temp.row  = 87;
                        temp.col  = 15;
                        temp.word = "0";
                        items.push_back(temp);
                        temp.row  = 88;
                        temp.col  = 15;
                        temp.word = "0";
                        items.push_back(temp);
                        temp.row  = 89;
                        temp.col  = 15;
                        temp.word = "通过";
                        items.push_back(temp);
                        break;
                    }
                    default:
                        break;
                    }
                    emit stopTimerSignal();
                    emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                }
                else
                {
                    retryNumber = 0;
                    ExcelItem temp;
                    switch (m_type)
                    {
                    case 0:
                    case 1:
                    {
                        temp.row  = 85;
                        temp.col  = 15;
                        temp.word = QString::number(static_cast<int>(pReceive.Data[1]));
                        items.push_back(temp);
                        temp.row  = 86;
                        temp.col  = 15;
                        temp.word = "0";
                        items.push_back(temp);
                        temp.row  = 87;
                        temp.col  = 15;
                        temp.word = QString::number(static_cast<int>(pReceive.Data[1]));
                        items.push_back(temp);
                        temp.row  = 88;
                        temp.col  = 15;
                        temp.word = "0";
                        items.push_back(temp);
                        temp.row  = 89;
                        temp.col  = 15;
                        temp.word = "未通过";
                        items.push_back(temp);
                        break;
                    }
                    default:
                        break;
                    }
                    emit signalReceiveStatus("内网通讯故障");
                    step = 0;
                    emit stopTimerSignal();
                    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                }
                break;
            }
            case TestSetHALLCurrent:
            {
                if(pReceive.Data[0] != 0xAA)
                {
                    emit stopTimerSignal();
                    switch(step)
                    {
                    case 3:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 5:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 7:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 10:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 12:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 14:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 17:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 19:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 21:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 24:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 26:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 28:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 29:
                    {
                        break;
                    }
                    case 32:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            temp.row = 19;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_dcaz13, 'f', 1) + "A";
                            items.push_back(temp);
                            temp.row = 20;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_gzaz13, 'f', 1) + "A";
                            items.push_back(temp);
                            temp.row = 21;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_dcaz13 - m_gzaz13, 'f', 1) + "A";
                            items.push_back(temp);
                            if (2 >= abs(m_dcaz13 - m_gzaz13))
                            {
                                temp.row  = 23;
                                temp.col  = 11;
                                temp.word = "通过";
                                items.push_back(temp);
                            }
                            else
                            {
                                temp.row  = 23;
                                temp.col  = 11;
                                temp.word = "未通"
                                            "过";
                                items.push_back(temp);
                                ifCPass = false;
                            }
                            break;
                        }
                        default:
                          break;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 34:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        switch(m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            temp.row = 25;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_dcaz23, 'f', 1) + "A";
                            items.push_back(temp);
                            temp.row = 26;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_gzaz23, 'f', 1) + "A";
                            items.push_back(temp);
                            temp.row = 27;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_dcaz23 - m_gzaz23, 'f', 1) + "A";
                            items.push_back(temp);
                            if (2 >= abs(m_dcaz23 - m_gzaz23))
                            {
                                temp.row  = 29;
                                temp.col  = 11;
                                temp.word = "通过";
                                items.push_back(temp);
                            }
                            else
                            {

                                temp.row  = 29;
                                temp.col  = 11;
                                temp.word = "未通"
                                            "过";
                                items.push_back(temp);
                                ifCPass = false;
                            }
                            break;
                        }
                        default:
                            break;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 36:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            temp.row = 31;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_dcaz33, 'f', 1) + "A";
                            items.push_back(temp);
                            temp.row = 32;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_gzaz33, 'f', 1) + "A";
                            items.push_back(temp);
                            temp.row = 33;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_dcaz33 - m_gzaz33, 'f', 1) + "A";
                            items.push_back(temp);
                            if (2 >= abs(m_dcaz33 - m_gzaz33))
                            {
                                temp.row  = 35;
                                temp.col  = 11;
                                temp.word = "通过";
                                items.push_back(temp);
                            }
                            else
                            {

                                temp.row  = 35;
                                temp.col  = 11;
                                temp.word = "未通"
                                            "过";
                                items.push_back(temp);
                                ifCPass = false;
                            }
                            break;
                        }
                        default:
                            break;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 39:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            temp.row = 37;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_dcaf13, 'f', 1) + "A";
                            items.push_back(temp);
                            temp.row = 38;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_gzaf13, 'f', 1) + "A";
                            items.push_back(temp);
                            temp.row = 39;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_dcaf13 - m_gzaf13, 'f', 1) + "A";
                            items.push_back(temp);
                            if (2 >= abs(m_dcaf13 - m_gzaf13))
                            {
                                temp.row  = 41;
                                temp.col  = 11;
                                temp.word = "通过";
                                items.push_back(temp);

                            }
                            else
                            {

                                temp.row  = 41;
                                temp.col  = 11;
                                temp.word = "未通"
                                            "过";
                                items.push_back(temp);

                                ifCPass = false;
                            }
                            break;
                        }
                        default:
                            break;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 41:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            temp.row = 43;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_dcaf23, 'f', 1) + "A";
                            items.push_back(temp);
                            temp.row = 44;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_gzaf23, 'f', 1) + "A";
                            items.push_back(temp);
                            temp.row = 45;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_dcaf23 - m_gzaf23, 'f', 1) + "A";
                            items.push_back(temp);
                            if (2 >= abs(m_dcaf23 - m_gzaf23))
                            {
                                temp.row  = 47;
                                temp.col  = 11;
                                temp.word = "通过";
                                items.push_back(temp);

                            }
                            else
                            {

                                temp.row  = 47;
                                temp.col  = 11;
                                temp.word = "未通"
                                            "过";
                                items.push_back(temp);

                                ifCPass = false;
                            }
                            break;
                        }
                        default:
                            break;
                        }
                        slotCheckIfUpdate();
                        break;
                    }
                    case 43:
                    {
                        if((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]) > 32767)
                        {
                            m_dcaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                        }
                        else
                        {
                            m_dcaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                            static_cast<int>(pReceive.Data[5]))) / 10.0;
                        }
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            temp.row = 49;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_dcaf33, 'f', 1) + "A";
                            items.push_back(temp);
                            temp.row = 50;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_gzaf33, 'f', 1) + "A";
                            items.push_back(temp);
                            temp.row = 51;
                            temp.col = 11;
                            temp.word =
                                QString::number(m_dcaf33 - m_gzaf33, 'f', 1) + "A";
                            items.push_back(temp);
                            if (2 >= abs(m_dcaf33 - m_gzaf33))
                            {
                                temp.row  = 53;
                                temp.col  = 11;
                                temp.word = "通过";
                                items.push_back(temp);

                            }
                            else
                            {

                                temp.row  = 53;
                                temp.col  = 11;
                                temp.word = "未通"
                                            "过";
                                items.push_back(temp);

                                ifCPass = false;
                            }
                            break;
                        }
                        default:
                            break;
                        }
                        if (ifCPass)
                        {
                            step = 0;
                            emit signalFinish();timeoutTimer.stop();step = 0;
                        }
                        else
                        {
                            step = 0;
                            emit signalErrorFinish();timeoutTimer.stop();step = 0;
                            ifCPass = true;
                        }
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    }
                }
                break;
            }
            case TestHALLCurrent:
            {
                if(pReceive.Data[0] != 0xAA)
                {
                emit stopTimerSignal();
                switch(step)
                {
                case 46:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_dcaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_dcaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 48:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_dcaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_dcaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 50:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_dcaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_dcaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 53:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_dcaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_dcaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 55:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_dcaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_dcaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 57:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_dcaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_dcaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    switch (m_type)
                    {
                    case 0:
                    case 1:
                    {
                        ExcelItem temp;
                        temp.row = 6;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_dcaz13, 'f', 1) + "A";
                        items.push_back(temp);
                        temp.row = 7;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_gzaz13, 'f', 1) + "A";
                        items.push_back(temp);
                        temp.row = 8;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_dcaz13 - m_gzaz13, 'f', 1) + "A";
                        items.push_back(temp);
                        if (2 >= abs(m_dcaz13 - m_gzaz13))
                        {
                            temp.row  = 10;
                            temp.col  = 15;
                            temp.word = "通过";
                            items.push_back(temp);
                        }
                        else
                        {
                            temp.row  = 10;
                            temp.col  = 15;
                            temp.word = "未通过";
                            items.push_back(temp);
                            ifCPass = false;
                        }
                        temp.row = 12;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_dcaz23, 'f', 1) + "A";
                        items.push_back(temp);
                        temp.row = 13;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_gzaz23, 'f', 1) + "A";
                        items.push_back(temp);
                        temp.row = 14;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_dcaz23 - m_gzaz23, 'f', 1) + "A";
                        items.push_back(temp);
                        if (2 >= abs(m_dcaz23 - m_gzaz23))
                        {
                            temp.row  = 16;
                            temp.col  = 15;
                            temp.word = "通过";
                            items.push_back(temp);
                        }
                        else
                        {
                            temp.row  = 16;
                            temp.col  = 15;
                            temp.word = "未通过";
                            items.push_back(temp);
                            ifCPass = false;
                        }
                        temp.row = 18;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_dcaz33, 'f', 1) + "A";
                        items.push_back(temp);
                        temp.row = 19;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_gzaz33, 'f', 1) + "A";
                        items.push_back(temp);
                        temp.row = 20;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_dcaz33 - m_gzaz33, 'f', 1) + "A";
                        items.push_back(temp);
                        if (2 >= abs(m_dcaz33 - m_gzaz33))
                        {
                            temp.row  = 22;
                            temp.col  = 15;
                            temp.word = "通过";
                            items.push_back(temp);
                        }
                        else
                        {
                            temp.row  = 22;
                            temp.col  = 15;
                            temp.word = "未通过";
                            items.push_back(temp);
                            ifCPass = false;
                        }
                        temp.row = 24;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_dcaf13, 'f', 1) + "A";
                        items.push_back(temp);
                        temp.row = 25;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_gzaf13, 'f', 1) + "A";
                        items.push_back(temp);
                        temp.row = 26;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_dcaf13 - m_gzaf13, 'f', 1) + "A";
                        items.push_back(temp);
                        if (2 >= abs(m_dcaf13 - m_gzaf13))
                        {
                            temp.row  = 28;
                            temp.col  = 15;
                            temp.word = "通过";
                            items.push_back(temp);
                        }
                        else
                        {
                            temp.row  = 28;
                            temp.col  = 15;
                            temp.word = "未通过";
                            items.push_back(temp);
                            ifCPass = false;
                        }
                        temp.row = 30;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_dcaf23, 'f', 1) + "A";
                        items.push_back(temp);
                        temp.row = 31;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_gzaf23, 'f', 1) + "A";
                        items.push_back(temp);
                        temp.row = 32;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_dcaf23 - m_gzaf23, 'f', 1) + "A";
                        items.push_back(temp);
                        if (2 >= abs(m_dcaf23 - m_gzaf23))
                        {
                            temp.row  = 34;
                            temp.col  = 15;
                            temp.word = "通过";
                            items.push_back(temp);
                        }
                        else
                        {
                            temp.row  = 34;
                            temp.col  = 15;
                            temp.word = "未通过";
                            items.push_back(temp);
                            ifCPass = false;
                        }
                        temp.row = 36;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_dcaf33, 'f', 1) + "A";
                        items.push_back(temp);
                        temp.row = 37;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_gzaf33, 'f', 1) + "A";
                        items.push_back(temp);
                        temp.row = 38;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_dcaf33 - m_gzaf33, 'f', 1) + "A";
                        items.push_back(temp);
                        if (2 >= abs(m_dcaf33 - m_gzaf33))
                        {
                            temp.row  = 40;
                            temp.col  = 15;
                            temp.word = "通过";
                            items.push_back(temp);
                        }
                        else
                        {
                            temp.row  = 40;
                            temp.col  = 15;
                            temp.word = "未通过";
                            items.push_back(temp);
                            ifCPass = false;
                        }
                        break;
                    }
                    default:
                        break;
                    }
                    if (ifCPass)
                    {
                        step = 0;
                        emit signalFinish();timeoutTimer.stop();step = 0;
                    }
                    else
                    {
                        step = 0;
                        emit signalErrorFinish();timeoutTimer.stop();step = 0;
                        ifCPass = true;
                    }
                    break;
                }
                default:
                {
                    break;
                }
                }
                }break;
            }
            default:
                break;
            }
        }
        else if(static_cast<int>(pReceive.ID) == id14)
        {
            if(pReceive.Data[0] == 0x0C)
            {
                emit stopTimerSignal();
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0);
            }
            else if(pReceive.Data[0] == 0x0E)
            {
                emit stopTimerSignal();
                float temp = 0.0;
                if((static_cast<int>(pReceive.Data[4]) << 8) +
                        static_cast<int>(pReceive.Data[5]) > 32767)
                {
                    temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                    static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                }
                else
                {
                    temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                    static_cast<int>(pReceive.Data[5]))) / 10.0;
                }
                emit signalGetInfo(m_info, temp);
            }
            switch (lastTestType)
            {
            case TestSetBatteryVoltage:
            {
                emit stopTimerSignal();
                switch(step)
                {
                case 2:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvz1 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                case 4:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvy1 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                case 7:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvz2 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                case 9:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvy2 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                case 13:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvz3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                case 15:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvy3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                default:
                {
                    break;
                }
                }
                break;
            }
            case TestBatteryVoltage:
            {
                emit stopTimerSignal();
                switch(step)
                {
                case 18:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvz3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                case 20:
                {
                    if(0x0C == pReceive.Data[0])
                    {
                        m_gzvy3 = ((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) /
                               10.0;
                        slotCheckIfUpdate();
                    }
                    break;
                }
                default:
                {
                    //slotCheckIfUpdate();
                    break;
                }
                }
                break;
            }
            case TestSupplyVoltage:
            {
                emit stopTimerSignal();
                if(pReceive.Data[0] == 0x0C)
                {
                    m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5])) /
                           10.0;
                    if (m_info == 9)
                    {
                        emit signalGetInfo(99, m_gzgd * 10);
                        m_info = 0;
                    }
                    else
                    {
                        ExcelItem temp;
                        temp.row = 49;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_dcgd, 'f', 1) + "V";
                        items.push_back(temp);
                        temp.row = 50;
                        temp.col = 15;
                        temp.word =
                            QString::number(m_gzgd, 'f', 1) + "V";
                        items.push_back(temp);
                        temp.row = 51;
                        temp.col = 15;
                        temp.word =
                            QString::number(abs(m_gzgd - m_dcgd),
                                            'f', 1) +
                            "V";
                        items.push_back(temp);
                        if (abs(m_gzgd - m_dcgd) <= 1)
                        {
                            temp.row  = 53;
                            temp.col  = 15;
                            temp.word = "通过";
                            items.push_back(temp);
                            step = 0;
                            emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                        else
                        {
                            temp.row  = 53;
                            temp.col  = 15;
                            temp.word = "未通过";
                            items.push_back(temp);
                            step = 0;
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                    }
                }
                break;
            }
            case TestHALLSignal:
            {
                emit stopTimerSignal();
                if(pReceive.Data[0] == 0x0E)
                {
                    if(step == 1)
                    {
                        float power = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                       static_cast<int>(pReceive.Data[5]))) / 10.0;
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            temp.row = 13;
                            temp.col = 2;
                            temp.word =
                                QString::number(power, 'f', 1) + "V";
                            items.push_back(temp);
                            break;
                        }
                        default:
                            break;
                        }
                        if (5.1 >= power && 4.9 <= power)
                        {
                            retryNumber = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                ExcelItem temp;
                                temp.row = 15;
                                temp.col = 2;
                                temp.word =
                                    QString::number(
                                        power - 5, 'f', 1) +
                                    "V";
                                items.push_back(temp);
                                temp.row  = 17;
                                temp.col  = 2;
                                temp.word = "通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            updateTestItem();
                        }
                        else
                        {
                            retryNumber = 0;
                            emit signalReceiveStatus(
                                "电压为" +
                                QString::number(power) +
                                "V，不符合要求");
                            step = 0;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                ExcelItem temp;
                                temp.row  = 17;
                                temp.col  = 2;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            emit stopTimerSignal();
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                    }
                    else if(step == 2)
                    {
                        float cur = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                     static_cast<int>(pReceive.Data[5])));
                        switch (m_type)
                        {
                        case 0:
                        case 1:
                        {
                            ExcelItem temp;
                            temp.row = 13;
                            temp.col = 4;
                            temp.word =
                                QString::number(cur / 10, 'f', 1) + "V";
                            items.push_back(temp);
                            temp.row = 15;
                            temp.col = 4;
                            temp.word =
                                QString::number(12.0 - cur / 10, 'f', 1) +
                                "V";
                            items.push_back(temp);
                            temp.row  = 16;
                            temp.col  = 4;
                            temp.word = "±0.5V";
                            items.push_back(temp);
                            temp.row  = 16;
                            temp.col  = 2;
                            temp.word = "±0.1V";
                            items.push_back(temp);
                            if (125 >= cur && 115 <= cur)
                            {
                                ExcelItem temp;
                                temp.row  = 17;
                                temp.col  = 4;
                                temp.word = "通过";
                                items.push_back(temp);
                                step = 0;
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                emit signalReceiveStatus("霍尔错误");
                                temp.row  = 17;
                                temp.col  = 4;
                                temp.word = "未通过";
                                items.push_back(temp);
                                step = 0;
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            break;
                        }
                        default:
                            break;
                        }
                    }
                }
                break;
            }
            case TestSerialCommunication:
            {
                emit stopTimerSignal();
                switch (m_type)
                {
                case 0:
                case 1:
                {
                    ExcelItem temp;
                    temp.row  = 73;
                    temp.col  = 3;
                    temp.word = QString::number(
                        static_cast<int>(pReceive.Data[3]));
                    items.push_back(temp);
                    temp.row  = 74;
                    temp.col  = 3;
                    temp.word = QString::number(0x1F);
                    items.push_back(temp);
                    temp.row  = 75;
                    temp.col  = 3;
                    temp.word = QString::number(
                        static_cast<int>(pReceive.Data[3]));
                    items.push_back(temp);
                    temp.row  = 76;
                    temp.col  = 3;
                    temp.word = QString::number(0x1F);
                    items.push_back(temp);
                    if (0x1F ==  static_cast<int>(pReceive.Data[3]))
                    {

                        failTimer->stop();
                        step = 0;
                        temp.row  = 77;
                        temp.col  = 3;
                        temp.word = "通过";
                        items.push_back(temp);
                        emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    else
                    {
                        emit signalReceiveStatus(
                            "485通信错误");
                        step      = 0;
                        temp.row  = 77;
                        temp.col  = 3;
                        temp.word = "未通过";
                        items.push_back(temp);
                        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                    }
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case TestCANCommunication:
            {
                emit stopTimerSignal();
                failTimer->stop();
                if(step == 1)
                {
                    if(pReceive.Data[0] == 0x11)
                    {
                        if(pReceive.Data[3] == 18)
                        {
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                ExcelItem temp;
                                temp.row  = 73;
                                temp.col  = 7;
                                temp.word = QString::number(
                                    static_cast<int>(pReceive.Data[3]));
                                items.push_back(temp);
                                temp.row  = 74;
                                temp.col  = 7;
                                temp.word = "31";
                                items.push_back(temp);
                                temp.row  = 75;
                                temp.col  = 7;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 76;
                                temp.col  = 7;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 77;
                                temp.col  = 7;
                                temp.word = "通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                        }
                        else
                        {
                            ifCanPass = false;
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                ExcelItem temp;
                                temp.row  = 73;
                                temp.col  = 7;
                                temp.word = QString::number(
                                    static_cast<int>(pReceive.Data[3]));
                                items.push_back(temp);
                                temp.row  = 74;
                                temp.col  = 7;
                                temp.word = "31";
                                items.push_back(temp);
                                temp.row  = 75;
                                temp.col  = 7;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 76;
                                temp.col  = 7;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 77;
                                temp.col  = 7;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                        }
                        QTimer *tempTimer = new QTimer(this);
                        connect(tempTimer, SIGNAL(timeout()), this, SLOT(updateTestItem()));
                        tempTimer->setSingleShot(true);
                        tempTimer->start(2000);
                    }
                }
                else if(step == 2)
                {
                    if(pReceive.Data[0] == 0x11)
                    {
                        if(pReceive.Data[3] == 18)
                        {
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                ExcelItem temp;
                                temp.row  = 67;
                                temp.col  = 11;
                                temp.word = QString::number(
                                    static_cast<int>(pReceive.Data[3]));
                                items.push_back(temp);
                                temp.row  = 68;
                                temp.col  = 11;
                                temp.word = "31";
                                items.push_back(temp);
                                temp.row  = 69;
                                temp.col  = 11;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 70;
                                temp.col  = 11;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 71;
                                temp.col  = 11;
                                temp.word = "通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            QTimer *tempTimer = new QTimer(this);
                            connect(tempTimer, SIGNAL(timeout()), this, SLOT(updateTestItem()));
                            tempTimer->setSingleShot(true);
                            tempTimer->start(2000);
                        }
                        else
                        {
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                ExcelItem temp;
                                temp.row  = 67;
                                temp.col  = 11;
                                temp.word = QString::number(
                                    static_cast<int>(pReceive.Data[3]));
                                items.push_back(temp);
                                temp.row  = 68;
                                temp.col  = 11;
                                temp.word = "31";
                                items.push_back(temp);
                                temp.row  = 69;
                                temp.col  = 11;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 70;
                                temp.col  = 11;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 71;
                                temp.col  = 11;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            ifCanPass = false;
                            QTimer *tempTimer = new QTimer(this);
                            connect(tempTimer, SIGNAL(timeout()), this, SLOT(updateTestItem()));
                            tempTimer->setSingleShot(true);
                            tempTimer->start(2000);
                        }
                    }
                }
                else if(step == 3)
                {
                    if(pReceive.Data[0] == 0x11)
                    {
                        if(pReceive.Data[3] == 18)
                        {
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                ExcelItem temp;
                                temp.row  = 73;
                                temp.col  = 11;
                                temp.word = QString::number(
                                    static_cast<int>(pReceive.Data[3]));
                                items.push_back(temp);
                                temp.row  = 74;
                                temp.col  = 11;
                                temp.word = "31";
                                items.push_back(temp);
                                temp.row  = 75;
                                temp.col  = 11;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 76;
                                temp.col  = 11;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 77;
                                temp.col  = 11;
                                temp.word = "通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            step = 0;
                            if (ifCanPass)
                            {
                                step = 0;
                                if(failTimer->isActive())
                                {
                                    failTimer->stop();
                                }
                                emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                            else
                            {
                                step = 0;
                                if(failTimer->isActive())
                                {
                                    failTimer->stop();
                                }
                                emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                            }
                        }
                        else
                        {
                            switch (m_type)
                            {
                            case 0:
                            case 1:
                            {
                                ExcelItem temp;
                                temp.row  = 73;
                                temp.col  = 11;
                                temp.word = QString::number(
                                    static_cast<int>(pReceive.Data[3]));
                                items.push_back(temp);
                                temp.row  = 74;
                                temp.col  = 11;
                                temp.word = "31";
                                items.push_back(temp);
                                temp.row  = 75;
                                temp.col  = 11;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 76;
                                temp.col  = 11;
                                temp.word = "0";
                                items.push_back(temp);
                                temp.row  = 77;
                                temp.col  = 11;
                                temp.word = "未通过";
                                items.push_back(temp);
                                break;
                            }
                            default:
                                break;
                            }
                            step = 0;
                            if(failTimer->isActive())
                            {
                                failTimer->stop();
                            }
                            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
                        }
                    }
                }
                break;
            }
            case TestSystemTemperature:
            {
                emit stopTimerSignal();
                if((static_cast<int>(pReceive.Data[3]) << 8) +
                        static_cast<int>(pReceive.Data[4]) > 32767)
                {
                    m_gztemp = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                    static_cast<int>(pReceive.Data[4])) - 65535) / 10.0;
                }
                else
                {
                    m_gztemp = (((static_cast<int>(pReceive.Data[3]) << 8) +
                                    static_cast<int>(pReceive.Data[4]))) / 10.0;
                }
                updateTestItem();
                break;
            }
            case TestSetHALLCurrent:
            {
                emit stopTimerSignal();
                switch(step)
                {
                case 2:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 4:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 6:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 9:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 11:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 13:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 16:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf11 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 18:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf21 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 20:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf31 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 23:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf12 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 25:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf22 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 27:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf32 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 31:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 33:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 35:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 38:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 40:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 42:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                default:
                {
                    qDebug() << "[HALL] ignore unexpected frame while waiting for step"
                             << step << "locate" << m_locate
                             << "data0" << static_cast<int>(pReceive.Data[0]);
                    break;
                }
                }
                break;
            }
            case TestHALLCurrent:
            {
                emit stopTimerSignal();
                switch(step)
                {
                case 45:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 47:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 49:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaz33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 52:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf13 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 54:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf23 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                case 56:
                {
                    if((static_cast<int>(pReceive.Data[4]) << 8) +
                            static_cast<int>(pReceive.Data[5]) > 32767)
                    {
                        m_gzaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
                    }
                    else
                    {
                        m_gzaf33 = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                        static_cast<int>(pReceive.Data[5]))) / 10.0;
                    }
                    slotCheckIfUpdate();
                    break;
                }
                default:
                {
                    qDebug() << "[HALL] ignore unexpected frame while waiting for step"
                             << step << "locate" << m_locate
                             << "data0" << static_cast<int>(pReceive.Data[0]);
                    break;
                }
                }
                break;
            }
            case TestZDResistance:
            {
                emit stopTimerSignal();
                if(step == 1 || step == 2)
                {
                    QTimer *tempTimer1 = new QTimer(this);
                    connect(tempTimer1, SIGNAL(timeout()), this,
                            SLOT(slotCheckIfUpdate()));
                    tempTimer1->setSingleShot(true);
                    tempTimer1->start(5000);
                }
                break;
            }
            default:
                break;
            }
        }
    }
}

void CANGetAndSend::openDO7()
{
    unsigned char data[8];
    data[0] = 0x01;
    data[1] = 01;
    data[2] = 00;
    data[3] = 00;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
    emit signalReceiveStatus("DO1关闭中。。。");
    data[0] = 0x01;
    data[1] = 07;
    data[2] = 01;
    data[3] = 00;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
    emit signalReceiveStatus("待测板上电中。。。");
}

void CANGetAndSend::closeDO7()
{
    unsigned char data[8];
    data[0] = 0x01;
    data[1] = 07;
    data[2] = 00;
    data[3] = 00;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
    emit signalReceiveStatus("待测板断电中。。。");
}

void CANGetAndSend::slotReceiveData(CAN_OBJ pReceive)
{
    qDebug() << pReceive.ID;
}

void CANGetAndSend::slotReceiveData2(VCI_CAN_OBJ pReceive)
{
    if(static_cast<int>(pReceive.ID) == 0x3C2)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current1 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3C3)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current2 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3C4)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current3 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3C5)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current4 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3C6)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current5 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3C7)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current6 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3C8)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current7 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3C9)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current8 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3CA)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current9 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3CB)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current10 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3CC)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current11 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3CD)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current12 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3CE)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current13 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3CF)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current14 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3D0)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current15 = temp / 1000.0;
    }
    else if(static_cast<int>(pReceive.ID) == 0x3D1)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current16 = temp / 1000.0;
    }
    switch(m_locate)
    {
    case 1:
    {
        m_current = m_current1;
        break;
    }
    case 2:
    {
        m_current = m_current2;
        break;
    }
    case 3:
    {
        m_current = m_current3;
        break;
    }
    case 4:
    {
        m_current = m_current4;
        break;
    }
    case 5:
    {
        m_current = m_current5;
        break;
    }
    case 6:
    {
        m_current = m_current6;
        break;
    }
    case 7:
    {
        m_current = m_current7;
        break;
    }
    case 8:
    {
        m_current = m_current8;
        break;
    }
    case 9:
    {
        m_current = m_current9;
        break;
    }
    case 10:
    {
        m_current = m_current10;
        break;
    }
    case 11:
    {
        m_current = m_current11;
        break;
    }
    case 12:
    {
        m_current = m_current12;
        break;
    }
    case 13:
    {
        m_current = m_current13;
        break;
    }
    case 14:
    {
        m_current = m_current14;
        break;
    }
    case 15:
    {
        m_current = m_current15;
        break;
    }
    case 16:
    {
        m_current = m_current16;
        break;
    }
    default:
        break;
    }
}

void CANGetAndSend::slotReceiveData2(CAN_OBJ pReceive)
{
    if(static_cast<int>(pReceive.ID) == 0x3C2 + m_locate)
    {
        int temp = ((static_cast<int>(pReceive.Data[0]) << 24) + (static_cast<int>(pReceive.Data[1]) << 16)
                + (static_cast<int>(pReceive.Data[2]) << 8) + static_cast<int>(pReceive.Data[3])) - 0x80000000;
        m_current = temp / 1000.0;
    }
}

void CANGetAndSend::slotRetrySend()
{
    if(step > 0)
    {
        step --;
    }
    if(lastTestType != FinishTest)
    {
        updateTestItem();
    }
}

void CANGetAndSend::slotPingSuccess()
{
    qDebug() << "ping success";
    QSettings setting(QDir::currentPath() + "/version.ini",
                      QSettings::IniFormat);
    QString ip = "195.16.19." + QString::number(100 + m_locate);
    QString ip2 = setting.value("ip2").toString();
    if (if_mes)
    {
        QJsonObject Internet;
        QJsonObject InternetOne;
        InternetOne.insert("IP", ip);
        InternetOne.insert("result", "通过");
        QJsonObject InternetTwo;
        InternetTwo.insert("IP", "untested");
        InternetTwo.insert("result", "untested");
        Internet.insert("InternetOne", InternetOne);
        Internet.insert("InternetTwo", InternetTwo);
        testResults.insert("Internet", Internet);
    }
    else
    {
        ExcelItem temp;
        switch (m_type)
        {
        case 0:
        case 1:
        {
            ExcelItem temp;
            temp.row  = 68;
            temp.col  = 15;
            temp.word = "4";
            items.push_back(temp);
            temp.row  = 69;
            temp.col  = 15;
            temp.word = "0";
            items.push_back(temp);
            temp.row  = 70;
            temp.col  = 15;
            temp.word = "通过";
            items.push_back(temp);
            break;
        }
        default:
            break;
        }
    }
    disconnect(ping, SIGNAL(signalSuccess()), this, SLOT(slotPingSuccess()));
    disconnect(ping, SIGNAL(signalFail(int)), this, SLOT(slotPingFail(int)));
    ping->stopPing();
    emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
    step = 0;
}

void CANGetAndSend::slotPingFail(int time)
{
    qDebug() << "ping failed";
    QSettings setting(QDir::currentPath() + "/version.ini",
                      QSettings::IniFormat);
    QString ip  = setting.value("ip1").toString();
    QString ip2 = setting.value("ip2").toString();
    if (if_mes)
    {
        QJsonObject Internet;
        QJsonObject InternetOne;
        InternetOne.insert("IP", ip);
        InternetOne.insert("result", "未通过");
        QJsonObject InternetTwo;
        InternetTwo.insert("IP", "untested");
        InternetTwo.insert("result", "untested");
        Internet.insert("InternetOne", InternetOne);
        Internet.insert("InternetTwo", InternetTwo);
        testResults.insert("Internet", Internet);
    }
    else
    {
        switch (m_type)
        {
        case 0:
        case 1:
        {
            ExcelItem temp;
            temp.row  = 68;
            temp.col  = 15;
            temp.word = QString::number(time);
            items.push_back(temp);
            temp.row  = 69;
            temp.col  = 15;
            temp.word = QString::number(4 - time);
            items.push_back(temp);
            temp.row  = 70;
            temp.col  = 15;
            temp.word = "未通过";
            items.push_back(temp);
            break;
        }
        default:
            break;
        }
    }
    disconnect(ping, SIGNAL(signalSuccess()), this, SLOT(slotPingSuccess()));
    disconnect(ping, SIGNAL(signalFail(int)), this, SLOT(slotPingFail(int)));
    ping->stopPing();
    emit signalReceiveStatus("网口未Ping通");
    emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
    step = 0;
}

void CANGetAndSend::slotCanTimeout()
{
    ExcelItem temp;
    if (if_mes)
    {
        QJsonObject CANCommunication;
        CANCommunication.insert("CAN0", "未通过");
        CANCommunication.insert("CAN1", "未通过");
        CANCommunication.insert("CAN2", "未通过");
        testResults.insert("CANCommunication", CANCommunication);
    }
    else
    {
        switch (m_type)
        {
        case 0:
        case 1:
        {
            ExcelItem temp;
            temp.row  = 77;
            temp.col  = 7;
            temp.word = "未通过";
            items.push_back(temp);
            break;
        }
        default:
            break;
        }
        emit signalReceiveStatus("CAN通信超时");
        step = 0;
        emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
    }
}

void CANGetAndSend::unsigned_to_hex(unsigned int value, std::string &hex_string)
{
    std::strstream buffer;
    buffer.setf(std::ios::showbase);
    buffer << std::hex << value;
    buffer >> hex_string;
}

void CANGetAndSend::slotSetTestMod()
{
    //lastTestType = TestSetMod;
    if (step == 0)
    {
        openDO7();
        QTimer *tempTimer = new QTimer(this);
        connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSetMod()));
        tempTimer->setSingleShot(true);
        tempTimer->start(5000);
    }
    else
    {
        unsigned char data[8];
        data[0] = 0xFA;
        data[1] = 0x55;
        data[2] = 0xAA;
        data[3] = 0x55;
        data[4] = 0xAA;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit signalReceiveStatus("待测板设置权限模式中。。。");
    }
}

void CANGetAndSend::slotSetFail()
{
    if (lastTestType == TestZDResistance)
    {
        emit signalSetModFail2();
    }
    else if(lastTestType == TestSetBatteryVoltage)
    {
        emit signalSetModFail();
    }
}

void CANGetAndSend::slotSetMod()
{
    if(lastTestType == TestZDResistance)
    {
        if(if_zdPass)
        {
            ZDResistance.insert("result", "通过");
        }
        else
        {
            ZDResistance.insert("result", "未通过");
        }
        testResults.insert("ZDResistance", ZDResistance);
    }
    setFailTimer->start(5000);
    unsigned char data[8];
    data[0] = 0xFB;
    data[1] = 0xAA;
    data[2] = 0x55;
    data[3] = 0xAA;
    data[4] = 0x55;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = 0x00;
    TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
    QTimer *tempTimer = new QTimer(this);
    tempTimer->setSingleShot(true);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotResendMod()));
    tempTimer->start(1000);
    emit signalReceiveStatus("待测板设置工装模式中。。。");
}

void CANGetAndSend::slotErrorFinish() { emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0; }

void CANGetAndSend::slotFinish() { emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0; }

void CANGetAndSend::slotReadVolt()
{
    ifVPass = true;
    unsigned char data[8];
    data[0] = 0x06;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    TransmitCAN(((0x1854 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
    emit signalReceiveStatus("电池总压测试中。。。");
    //failTimer->start(10000);15000);
    step = 0;
}

void CANGetAndSend::slotSetGYY()
{
    QByteArray temp;
    if (lastTestType == TestSetBatteryVoltage)
    {
        switch(m_locate)
        {
        case 3:
        {
            temp = QByteArray("\x01\x10\x10\x00\x00\x01\x02\x01\xF4", 9);
                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
            break;
        }
        case 7:
        {
            temp = QByteArray("\x05\x10\x10\x00\x00\x01\x02\x01\xF4", 9);
                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
            break;
        }
        case 11:
        {
            temp = QByteArray("\x09\x10\x10\x00\x00\x01\x02\x01\xF4", 9);
                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
            break;
        }
        case 15:
        {
            temp = QByteArray("\x0D\x10\x10\x00\x00\x01\x02\x01\xF4", 9);
                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
            break;
        }
        default:
            break;
        }
        step = 1;
    }
    else
    {
        switch(m_locate)
        {
        case 3:
        {
            temp = QByteArray("\x01\x10\x10\x00\x00\x01\x02\x03\xE8", 9);
                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
            break;
        }
        case 7:
        {
            temp = QByteArray("\x05\x10\x10\x00\x00\x01\x02\x03\xE8", 9);
                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
            break;
        }
        case 11:
        {
            temp = QByteArray("\x09\x10\x10\x00\x00\x01\x02\x03\xE8", 9);
                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
            break;
        }
        case 15:
        {
            temp = QByteArray("\x0D\x10\x10\x00\x00\x01\x02\x03\xE8", 9);
                Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
            break;
        }
        default:
            break;
        }
        step = 17;
    }
    emit signalReceiveStatus("高压源设置中。。。");
    QTimer *tempTimer1 = new QTimer(this);
    connect(tempTimer1, SIGNAL(timeout()), this,
            SLOT(slotCheckIfUpdate()));
    tempTimer1->setSingleShot(true);
    tempTimer1->start(voltageTime);
}

void CANGetAndSend::slotReceive485Data(QByteArray data)
{
    QByteArray tmp = QByteArray(data.constData(), data.size());
    qDebug() << "[CAN-485] receive"
             << "locate" << m_locate
             << "step" << step
             << "lastTestType" << lastTestType
             << "bytes" << tmp.size()
             << "addr" << (tmp.isEmpty() ? -1 : static_cast<int>(static_cast<unsigned char>(tmp.at(0))))
             << "preview" << tmp.left(24).toHex()
             << "thread" << QThread::currentThread()
             << "threadId" << QThread::currentThreadId();
    if (lastTestType == TestZDResistance)
    {
        if(tmp.size() > 27)
        {
            emit stopTimerSignal();
            float temp1 = (((static_cast<int>(static_cast<unsigned char>(tmp.at(3))) << 24)
                            + (static_cast<int>(static_cast<unsigned char>(tmp.at(4))) << 16)
                    + (static_cast<int>(static_cast<unsigned char>(tmp.at(5))) << 8)
                            + static_cast<int>(static_cast<unsigned char>(tmp.at(6)))) / 100.0);
            float temp2 = (((static_cast<int>(static_cast<unsigned char>(tmp.at(7))) << 24)
                            + (static_cast<int>(static_cast<unsigned char>(tmp.at(8))) << 16)
                    + (static_cast<int>(static_cast<unsigned char>(tmp.at(9))) << 8)
                            + static_cast<int>(static_cast<unsigned char>(tmp.at(10)))) / 100.0);
            float temp3 = (((static_cast<int>(static_cast<unsigned char>(tmp.at(11))) << 24)
                            + (static_cast<int>(static_cast<unsigned char>(tmp.at(12))) << 16)
                    + (static_cast<int>(static_cast<unsigned char>(tmp.at(13))) << 8)
                            + static_cast<int>(static_cast<unsigned char>(tmp.at(14)))) / 100.0);
            float temp4 = (((static_cast<int>(static_cast<unsigned char>(tmp.at(15))) << 24)
                            + (static_cast<int>(static_cast<unsigned char>(tmp.at(16))) << 16)
                    + (static_cast<int>(static_cast<unsigned char>(tmp.at(17))) << 8)
                            + static_cast<int>(static_cast<unsigned char>(tmp.at(18)))) / 100.0);
            float temp5 = (((static_cast<int>(static_cast<unsigned char>(tmp.at(19))) << 24)
                            + (static_cast<int>(static_cast<unsigned char>(tmp.at(20))) << 16)
                    + (static_cast<int>(static_cast<unsigned char>(tmp.at(21))) << 8)
                            + static_cast<int>(static_cast<unsigned char>(tmp.at(22)))) / 100.0);
            float temp6 = (((static_cast<int>(static_cast<unsigned char>(tmp.at(23))) << 24)
                            + (static_cast<int>(static_cast<unsigned char>(tmp.at(24))) << 16)
                    + (static_cast<int>(static_cast<unsigned char>(tmp.at(25))) << 8)
                            + static_cast<int>(static_cast<unsigned char>(tmp.at(26)))) / 100.0);
            //qDebug() << "终端电阻阻值为" << QString::number(temp, 'f', 1);
            if(step == 3)
            {
                if((m_locate == 1 || m_locate == 2) && static_cast<int>(tmp.at(0)) == 3)
                {
                    emit signalReceiveStatus("第一堆第一个模块CAN0阻值为" + QString::number(temp1)
                                            + " 第一堆第一个模块CAN1阻值为" + QString::number(temp2)
                                            + " 第一堆第一个模块CAN2阻值为" + QString::number(temp3)
                                            + " 第一堆第二个模块CAN0阻值为" + QString::number(temp4)
                                            + " 第一堆第二个模块CAN1阻值为" + QString::number(temp5)
                                            + " 第一堆第二个模块CAN2阻值为" + QString::number(temp6));
                    if(m_locate % 2 == 1)
                    {
                        ZDResistance.insert("can0", QString::number(temp1, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp2, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp3, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp1, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp2, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp3, 'f', 2);
                        items.push_back(item);
                        if(abs(temp1 - 124) <= 1.24 && abs(temp2 - 118.6) <= 1.186 && abs(temp3 - 118.6) <= 1.186)
                        {
                            slotCheckIfUpdate();
                        }
                        else
                        {
                            if_zdPass = false;
                            slotCheckIfUpdate();
                        }
                    }
                    else
                    {
                        ZDResistance.insert("can0", QString::number(temp4, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp5, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp6, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp4, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp5, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp6, 'f', 2);
                        items.push_back(item);
                        if(abs(temp4 - 124) <= 1.24 && abs(temp5 - 118.6) <= 1.186 && abs(temp6 - 118.6) <= 1.186)
                        {
                            slotCheckIfUpdate();
                        }
                        else
                        {
                            if_zdPass = false;
                            slotCheckIfUpdate();
                        }
                    }
                }
                else if((m_locate == 5 || m_locate == 6) && static_cast<int>(tmp.at(0)) == 7)
                {
                    emit signalReceiveStatus("第二堆第一个模块CAN0阻值为" + QString::number(temp1)
                                            + " 第二堆第一个模块CAN1阻值为" + QString::number(temp2)
                                            + " 第二堆第一个模块CAN2阻值为" + QString::number(temp3)
                                            + " 第二堆第二个模块CAN0阻值为" + QString::number(temp4)
                                            + " 第二堆第二个模块CAN1阻值为" + QString::number(temp5)
                                            + " 第二堆第二个模块CAN2阻值为" + QString::number(temp6));
                    if(m_locate % 2 == 1)
                    {
                        ZDResistance.insert("can0", QString::number(temp1, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp2, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp3, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp1, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp2, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp3, 'f', 2);
                        items.push_back(item);
                        if(abs(temp1 - 124) <= 1.24 && abs(temp2 - 118.6) <= 1.186 && abs(temp3 - 118.6) <= 1.186)
                        {
                            slotCheckIfUpdate();
                        }
                        else
                        {
                            if_zdPass = false;
                            slotCheckIfUpdate();
                        }
                    }
                    else
                    {
                        ZDResistance.insert("can0", QString::number(temp4, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp5, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp6, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp4, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp5, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp6, 'f', 2);
                        items.push_back(item);
                        if(abs(temp4 - 124) <= 1.24 && abs(temp5 - 118.6) <= 1.186 && abs(temp6 - 118.6) <= 1.186)
                        {
                            slotCheckIfUpdate();
                        }
                        else
                        {
                            if_zdPass = false;
                            slotCheckIfUpdate();
                        }
                    }
                }
                else if((m_locate == 9 || m_locate == 10) && static_cast<int>(tmp.at(0)) == 11)
                {
                    emit signalReceiveStatus("第三堆第一个模块CAN0阻值为" + QString::number(temp1)
                                            + " 第三堆第一个模块CAN1阻值为" + QString::number(temp2)
                                            + " 第三堆第一个模块CAN2阻值为" + QString::number(temp3)
                                            + " 第三堆第二个模块CAN0阻值为" + QString::number(temp4)
                                            + " 第三堆第二个模块CAN1阻值为" + QString::number(temp5)
                                            + " 第三堆第二个模块CAN2阻值为" + QString::number(temp6));
                    if(m_locate % 2 == 1)
                    {
                        ZDResistance.insert("can0", QString::number(temp1, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp2, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp3, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp1, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp2, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp3, 'f', 2);
                        items.push_back(item);
                        if(abs(temp1 - 124) <= 1.24 && abs(temp2 - 118.6) <= 1.186 && abs(temp3 - 118.6) <= 1.186)
                        {
                            slotCheckIfUpdate();
                        }
                        else
                        {
                            if_zdPass = false;
                            slotCheckIfUpdate();
                        }
                    }
                    else
                    {
                        ZDResistance.insert("can0", QString::number(temp4, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp5, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp6, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp4, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp5, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp6, 'f', 2);
                        items.push_back(item);
                        if(abs(temp4 - 124) <= 1.24 && abs(temp5 - 118.6) <= 1.186 && abs(temp6 - 118.6) <= 1.186)
                        {
                            slotCheckIfUpdate();
                        }
                        else
                        {
                            if_zdPass = false;
                            slotCheckIfUpdate();
                        }
                    }
                }
                else if((m_locate == 13 || m_locate == 14) && static_cast<int>(tmp.at(0)) == 15)
                {
                    emit signalReceiveStatus("第四堆第一个模块CAN0阻值为" + QString::number(temp1)
                                            + " 第四堆第一个模块CAN1阻值为" + QString::number(temp2)
                                            + " 第四堆第一个模块CAN2阻值为" + QString::number(temp3)
                                            + " 第四堆第二个模块CAN0阻值为" + QString::number(temp4)
                                            + " 第四堆第二个模块CAN1阻值为" + QString::number(temp5)
                                            + " 第四堆第二个模块CAN2阻值为" + QString::number(temp6));
                    if(m_locate % 2 == 1)
                    {
                        ZDResistance.insert("can0", QString::number(temp1, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp2, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp3, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp1, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp2, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp3, 'f', 2);
                        items.push_back(item);
                        if(abs(temp1 - 124) <= 1.24 && abs(temp2 - 118.6) <= 1.186 && abs(temp3 - 118.6) <= 1.186)
                        {
                            slotCheckIfUpdate();
                        }
                        else
                        {
                            if_zdPass = false;
                            slotCheckIfUpdate();
                        }
                    }
                    else
                    {
                        ZDResistance.insert("can0", QString::number(temp4, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp5, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp6, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp4, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp5, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp6, 'f', 2);
                        items.push_back(item);
                        if(abs(temp4 - 124) <= 1.24 && abs(temp5 - 118.6) <= 1.186 && abs(temp6 - 118.6) <= 1.186)
                        {
                            slotCheckIfUpdate();
                        }
                        else
                        {
                            if_zdPass = false;
                            slotCheckIfUpdate();
                        }
                    }
                }
                else
                {
                    slotCheckIfUpdate();
                }
            }
            else if(step == 4)
            {
                openDO7();
                if((m_locate == 3 || m_locate == 4) && static_cast<int>(tmp.at(0)) == 4)
                {
                    emit signalReceiveStatus("第一堆第三个模块CAN0阻值为" + QString::number(temp1)
                                            + " 第一堆第三个模块CAN1阻值为" + QString::number(temp2)
                                            + " 第一堆第三个模块CAN2阻值为" + QString::number(temp3)
                                            + " 第一堆第四个模块CAN0阻值为" + QString::number(temp4)
                                            + " 第一堆第四个模块CAN1阻值为" + QString::number(temp5)
                                            + " 第一堆第四个模块CAN2阻值为" + QString::number(temp6));
                    if(m_locate % 2 == 1)
                    {
                        ZDResistance.insert("can0", QString::number(temp1, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp2, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp3, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp1, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp2, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp3, 'f', 2);
                        items.push_back(item);
                        if(abs(temp1 - 124) <= 1.24 && abs(temp2 - 118.6) <= 1.186 && abs(temp3 - 118.6) <= 1.186)
                        {
                        }
                        else
                        {
                            if_zdPass = false;
                        }
                    }
                    else
                    {
                        ZDResistance.insert("can0", QString::number(temp4, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp5, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp6, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp4, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp5, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp6, 'f', 2);
                        items.push_back(item);
                        if(abs(temp4 - 124) <= 1.24 && abs(temp5 - 118.6) <= 1.186 && abs(temp6 - 118.6) <= 1.186)
                        {
                        }
                        else
                        {
                            if_zdPass = false;
                        }
                    }
                }
                else if((m_locate == 7 || m_locate == 8) && static_cast<int>(tmp.at(0)) == 8)
                {
                    emit signalReceiveStatus("第二堆第三个模块CAN0阻值为" + QString::number(temp1)
                                            + " 第二堆第三个模块CAN1阻值为" + QString::number(temp2)
                                            + " 第二堆第三个模块CAN2阻值为" + QString::number(temp3)
                                            + " 第二堆第四个模块CAN0阻值为" + QString::number(temp4)
                                            + " 第二堆第四个模块CAN1阻值为" + QString::number(temp5)
                                            + " 第二堆第四个模块CAN2阻值为" + QString::number(temp6));
                    if(m_locate % 2 == 1)
                    {
                        ZDResistance.insert("can0", QString::number(temp1, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp2, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp3, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp1, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp2, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp3, 'f', 2);
                        items.push_back(item);
                        if(abs(temp1 - 124) <= 1.24 && abs(temp2 - 118.6) <= 1.186 && abs(temp3 - 118.6) <= 1.186)
                        {
                        }
                        else
                        {
                            if_zdPass = false;
                        }
                    }
                    else
                    {
                        ZDResistance.insert("can0", QString::number(temp4, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp5, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp6, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp4, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp5, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp6, 'f', 2);
                        items.push_back(item);
                        if(abs(temp4 - 124) <= 1.24 && abs(temp5 - 118.6) <= 1.186 && abs(temp6 - 118.6) <= 1.186)
                        {
                        }
                        else
                        {
                            if_zdPass = false;
                        }
                    }
                }
                else if((m_locate == 11 || m_locate == 12) && static_cast<int>(tmp.at(0)) == 12)
                {
                    emit signalReceiveStatus("第三堆第三个模块CAN0阻值为" + QString::number(temp1)
                                            + " 第三堆第三个模块CAN1阻值为" + QString::number(temp2)
                                            + " 第三堆第三个模块CAN2阻值为" + QString::number(temp3)
                                            + " 第三堆第四个模块CAN0阻值为" + QString::number(temp4)
                                            + " 第三堆第四个模块CAN1阻值为" + QString::number(temp5)
                                            + " 第三堆第四个模块CAN2阻值为" + QString::number(temp6));
                    if(m_locate % 2 == 1)
                    {
                        ZDResistance.insert("can0", QString::number(temp1, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp2, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp3, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp1, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp2, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp3, 'f', 2);
                        items.push_back(item);
                        if(abs(temp1 - 124) <= 1.24 && abs(temp2 - 118.6) <= 1.186 && abs(temp3 - 118.6) <= 1.186)
                        {
                        }
                        else
                        {
                            if_zdPass = false;
                        }
                    }
                    else
                    {
                        ZDResistance.insert("can0", QString::number(temp4, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp5, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp6, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp4, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp5, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp6, 'f', 2);
                        items.push_back(item);
                        if(abs(temp4 - 124) <= 1.24 && abs(temp5 - 118.6) <= 1.186 && abs(temp6 - 118.6) <= 1.186)
                        {
                        }
                        else
                        {
                            if_zdPass = false;
                        }
                    }
                }
                else if((m_locate == 15 || m_locate == 16) && static_cast<int>(tmp.at(0)) == 16)
                {
                    emit signalReceiveStatus("第四堆第三个模块CAN0阻值为" + QString::number(temp1)
                                            + " 第四堆第三个模块CAN1阻值为" + QString::number(temp2)
                                            + " 第四堆第三个模块CAN2阻值为" + QString::number(temp3)
                                            + " 第四堆第四个模块CAN0阻值为" + QString::number(temp4)
                                            + " 第四堆第四个模块CAN1阻值为" + QString::number(temp5)
                                            + " 第四堆第四个模块CAN2阻值为" + QString::number(temp6));
                    if(m_locate % 2 == 1)
                    {
                        ZDResistance.insert("can0", QString::number(temp1, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp2, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp3, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp1, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp2, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp3, 'f', 2);
                        items.push_back(item);
                        if(abs(temp1 - 124) <= 1.24 && abs(temp2 - 118.6) <= 1.186 && abs(temp3 - 118.6) <= 1.186)
                        {
                        }
                        else
                        {
                            if_zdPass = false;
                        }
                    }
                    else
                    {
                        ZDResistance.insert("can0", QString::number(temp4, 'f', 2));
                        ZDResistance.insert("can1", QString::number(temp5, 'f', 2));
                        ZDResistance.insert("can2", QString::number(temp6, 'f', 2));
                        ZDResistance.insert("allowable", "can0 124±1.24 can1can2 118.6±1.186");
                        ExcelItem item;
                        item.row = 2;
                        item.col = 17;
                        item.word = "CAN1终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 17;
                        item.word = QString::number(temp4, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 18;
                        item.word = "CAN2终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 18;
                        item.word = QString::number(temp5, 'f', 2);
                        items.push_back(item);
                        item.row = 2;
                        item.col = 19;
                        item.word = "CAN3终端电阻(Ω)";
                        items.push_back(item);
                        item.row = 3;
                        item.col = 19;
                        item.word = QString::number(temp6, 'f', 2);
                        items.push_back(item);
                        if(abs(temp4 - 124) <= 1.24 && abs(temp5 - 118.6) <= 1.186 && abs(temp6 - 118.6) <= 1.186)
                        {
                        }
                        else
                        {
                            if_zdPass = false;
                        }
                    }
                }
                step = 5;
                QTimer *tempTimer = new QTimer(this);
                connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSetMod()));
                tempTimer->setSingleShot(true);

                QSettings setting(QDir::currentPath() + "/version.ini",
                                  QSettings::IniFormat);
                int time  = setting.value("powerupTime").toInt();
                tempTimer->start(time);
//                step = 2;
//                slotCheckIfUpdate();
            }
        }
    }
}

void CANGetAndSend::slotTestE2()
{
    unsigned char data[8];
    data[0]      = 0x17;
    data[1]      = 0;
    data[2]      = 0;
    data[3]      = 0;
    data[4]      = 0;
    data[5]      = 0;
    data[6]      = 0;
    data[7]      = 0;
    TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
    emit signalReceiveStatus("EEPROM测试中。。。");
    emit startTimerSignal(1000);
}

void CANGetAndSend::slotChange(int type)
{
    switch(type)
    {
    case 1:
    {
        QByteArray temp;
        temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0", 27);

            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
        break;
    }
    case 2:
    {
        QByteArray temp;
        temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20", 27);

            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
        break;
    }
    case 3:
    {
        QByteArray temp;
        temp = QByteArray("\x01\x10\x10\x00\x00\x01\x02\x01\xF4", 9);
            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
        break;
    }
    case 4:
    {
        QByteArray temp;
        temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68", 27);

            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
        break;
    }
    case 5:
    {
        QByteArray temp;
        temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98", 27);

            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
        break;
    }
    case 6:
    {
        QByteArray temp;
        temp = QByteArray("\x01\x10\x10\x00\x00\x01\x02\x05\x78", 9);
            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
        break;
    }
    case 7:
    {
        QByteArray temp;
        temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC", 27);

            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
        break;
    }
    case 8:
    {
        QByteArray temp;
        temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC", 27);

            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
        break;
    }
    case 9:
    {
        QByteArray temp;
        temp = QByteArray("\x01\x10\x10\x00\x00\x01\x02\x03\xE8", 9);
            Rs485Frame req;
    req.sendData = temp;
    req.sendTick = QDateTime::currentMSecsSinceEpoch();

    // 写入全局Tx缓冲，串口控制器线程自动轮询发送
    bool ok = Rs485BufferMgr::GetInstance()->GetTxBuf().push(req);
    if (!ok)
    {
        qWarning() << "485发送缓冲区已满";
    }
        break;
    }
    default:
        break;
    }
}

void CANGetAndSend::slotReceiveSerial(QString serial)
{
    QStringList totalList = serial.split('/');
    if(totalList.size() != 2)
    {
        emit signalSerialError();
    }
    else
    {
        QStringList macList = totalList.at(1).split(':');
        if(macList.size() != 6)
        {
            emit signalSerialError();
        }
        else
        {
            QString qsContent = totalList.at(0);
            QString a= qsContent.left(7).right(6);
            QString b = qsContent.right(4);
            if(!qsContent[0].isUpper() || !isNum(a) || !qsContent[7].isUpper() || !isNum(b) || qsContent.length() != 12)
            {
                emit signalSerialError();
            }
            else
            {
                m_serial = totalList.at(0);
                emit signalSetSerial(m_serial);
                m_macList = totalList.at(1).split(":");
                emit signalSetMAC(totalList.at(1));
                emit signalStartTest();
            }
        }
    }
}

void CANGetAndSend::slotTestInternet()
{
    connect(ping, SIGNAL(signalSuccess()), this,
            SLOT(slotPingSuccess()));
    connect(ping, SIGNAL(signalFail(int)), this,
            SLOT(slotPingFail(int)));
    emit signalReceiveStatus("网络测试中。。。");
    QSettings setting(QDir::currentPath() + "/version.ini",
                      QSettings::IniFormat);
    QString ip = "195.16.19." + QString::number(100 + m_locate);
    switch (m_type)
    {
    case 0:
    case 1:
    {
        ExcelItem temp;
        temp.row  = 66;
        temp.col  = 15;
        temp.word = ip;
        items.push_back(temp);
        temp.row  = 67;
        temp.col  = 15;
        temp.word = ip;
        items.push_back(temp);
        ping->pingIP(ip);
        break;
    }
    default:
        break;
    }
}

void CANGetAndSend::startTimerSlot(int time)
{
//    qDebug() << "[TIMER] start retry"
//             << "locate" << m_locate
//             << "step" << step
//             << "intervalMs" << time
//             << "timerThread" << (retryTimer ? retryTimer->thread() : NULL)
//             << "currentThread" << QThread::currentThread()
//             << "threadId" << QThread::currentThreadId();
    if(retryTimer != NULL)
    {
        if(retryTimer->thread() != QThread::currentThread())
        {
            retryTimer->moveToThread(QThread::currentThread());
        }
        if(retryTimer->isActive())
        {
           retryTimer->stop();
        }
        retryTimer->start(time);
    }
}

void CANGetAndSend::stopTimerSlot()
{
//    qDebug() << "[TIMER] stop retry"
//             << "locate" << m_locate
//             << "step" << step
//             << "timerThread" << (retryTimer ? retryTimer->thread() : NULL)
//             << "currentThread" << QThread::currentThread()
//             << "threadId" << QThread::currentThreadId();
    if(retryTimer != NULL)
    {
        if(retryTimer->thread() != QThread::currentThread())
        {
            retryTimer->moveToThread(QThread::currentThread());
        }
        if(retryTimer->isActive())
        {
           retryTimer->stop();
        }
    }
}

void CANGetAndSend::startFailTimerSlot(int time)
{
//    qDebug() << "[TIMER] start fail"
//             << "locate" << m_locate
//             << "step" << step
//             << "intervalMs" << time
//             << "timerThread" << (failTimer ? failTimer->thread() : NULL)
//             << "currentThread" << QThread::currentThread()
//             << "threadId" << QThread::currentThreadId();
    if(failTimer != NULL)
    {
        if(failTimer->thread() != QThread::currentThread())
        {
            failTimer->moveToThread(QThread::currentThread());
        }
        if(failTimer->isActive())
        {
            failTimer->stop();
        }
        failTimer->start(time);
    }
}

void CANGetAndSend::stopFailTimerSlot()
{
//    qDebug() << "[TIMER] stop fail"
//             << "locate" << m_locate
//             << "step" << step
//             << "timerThread" << (failTimer ? failTimer->thread() : NULL)
//             << "currentThread" << QThread::currentThread()
//             << "threadId" << QThread::currentThreadId();
    if(failTimer != NULL)
    {
        if(failTimer->thread() != QThread::currentThread())
        {
            failTimer->moveToThread(QThread::currentThread());
        }
        if(failTimer->isActive())
        {
            failTimer->stop();
        }
    }
}

void CANGetAndSend::slotResendMod()
{
    if(lastTestType == TestZDResistance || lastTestType == TestSetBatteryVoltage)
    {
        unsigned char data[8];
        data[0] = 0xFB;
        data[1] = 0xAA;
        data[2] = 0x55;
        data[3] = 0xAA;
        data[4] = 0x55;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        QTimer *tempTimer = new QTimer(this);
        tempTimer->setSingleShot(true);
        connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotResendMod()));
        tempTimer->start(1000);
        emit signalReceiveStatus("待测板设置工装模式中。。。");
    }
}

void CANGetAndSend::slotIfCanUpdate(bool status)
{
    if(status)
    {
        updateTestItem();
    }
}

void CANGetAndSend::slotCheckIfUpdate()
{
    emit signalSetUpdateStatus(true);
    emit signalCheckIfCanUpdate();
}

void CANGetAndSend::slotTimeOut()
{
    emit signalTimeout();
}

void CANGetAndSend::slotSetKB(int type, float k, float b)
{
    switch(type)
    {
    case 1:
    {
        unsigned char data[8];
        data[0] = 0x0F;
        data[1] = 1;
        data[2] = 1;
        data[3]  = int(k * 1000) >> 8;
        data[4]  = int(k * 1000);
        int temp = b * 1000;
        if (b > 0)
        {
            data[5] = temp >> 8;
            data[6] = temp;
        }
        else if (b < 0)
        {
            data[5] = (65535 + temp) >> 8;
            data[6] = (65535 + temp);
        }
        else
        {
            data[5] = 0x00;
            data[6] = 0x00;
        }
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 2:
    {
        unsigned char data[8];
        data[0] = 0x0F;
        data[1] = 1;
        data[2] = 2;
        data[3]  = int(k * 1000) >> 8;
        data[4]  = int(k * 1000);
        int temp = b * 1000;
        if (b > 0)
        {
            data[5] = temp >> 8;
            data[6] = temp;
        }
        else if (b < 0)
        {
            data[5] = (65535 + temp) >> 8;
            data[6] = (65535 + temp);
        }
        else
        {
            data[5] = 0x00;
            data[6] = 0x00;
        }
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 3:
    {
        unsigned char data[8];
        data[0] = 0x0D;
        data[1] = 1;
        data[2]  = int(k * 1000) >> 8;
        data[3]  = int(k * 1000);
        int temp = b * 1000;
        if (b > 0)
        {
            data[4] = temp >> 8;
            data[5] = temp;
        }
        else if (b < 0)
        {
            data[4] = (65535 + temp) >> 8;
            data[5] = (65535 + temp);
        }
        else
        {
            data[4] = 0x00;
            data[5] = 0x00;
        }
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 4:
    {
        unsigned char data[8];
        data[0] = 0x0F;
        data[1] = 2;
        data[2] = 1;
        data[3]  = int(k * 1000) >> 8;
        data[4]  = int(k * 1000);
        int temp = b * 1000;
        if (b > 0)
        {
            data[5] = temp >> 8;
            data[6] = temp;
        }
        else if (b < 0)
        {
            data[5] = (65535 + temp) >> 8;
            data[6] = (65535 + temp);
        }
        else
        {
            data[5] = 0x00;
            data[6] = 0x00;
        }
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 5:
    {
        unsigned char data[8];
        data[0] = 0x0F;
        data[1] = 3;
        data[2] = 1;
        data[3]  = int(k * 1000) >> 8;
        data[4]  = int(k * 1000);
        int temp = b * 1000;
        if (b > 0)
        {
            data[5] = temp >> 8;
            data[6] = temp;
        }
        else if (b < 0)
        {
            data[5] = (65535 + temp) >> 8;
            data[6] = (65535 + temp);
        }
        else
        {
            data[5] = 0x00;
            data[6] = 0x00;
        }
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 6:
    {
        unsigned char data[8];
        data[0] = 0x22;
        data[1]  = int(k * 1000) >> 8;
        data[2]  = int(k * 1000);
        int temp = b * 1000;
        if (b > 0)
        {
            data[3] = temp >> 8;
            data[4] = temp;
        }
        else if (b < 0)
        {
            data[3] = (65535 + temp) >> 8;
            data[4] = (65535 + temp);
        }
        else
        {
            data[3] = 0x00;
            data[4] = 0x00;
        }
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 7:
    {
        unsigned char data[8];
        data[0] = 0x0D;
        data[1]  = int(k * 1000) >> 8;
        data[2]  = int(k * 1000);
        int temp = b * 1000;
        if (b > 0)
        {
            data[3] = temp >> 8;
            data[4] = temp;
        }
        else if (b < 0)
        {
            data[3] = (65535 + temp) >> 8;
            data[4] = (65535 + temp);
        }
        else
        {
            data[3] = 0x00;
            data[4] = 0x00;
        }
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    default:
        break;
    }
}

void CANGetAndSend::slotReadInfo(int type)
{
    m_info = type;
    switch(m_info)
    {
    case 1:
    case 2:
    case 4:
    case 5:
    {
        unsigned char data[8];
        data[0] = 0x0E;
        data[1] = 1;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 3:
    case 6:
    {
        unsigned char data[8];
        data[0] = 0x0C;
        data[1] = 1;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 7:
    {
        unsigned char data[8];
        data[0] = 0x0E;
        data[1] = 2;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 8:
    {
        unsigned char data[8];
        data[0] = 0x0E;
        data[1] = 3;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 9:
    {
        unsigned char data[8];
        data[0] = 0x0C;
        data[1] = 3;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 10:
    case 11:
    {
        unsigned char data[8];
        data[0] = 0x0C;
        data[1] = 2;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 101:
    case 102:
    case 104:
    case 105:
    {
        unsigned char data[8];
        data[0] = 0x0E;
        data[1] = 1;
        data[2] = 1;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 103:
    case 106:
    {
        unsigned char data[8];
        data[0] = 0x0C;
        data[1] = 1;
        data[2] = 1;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 107:
    {
        unsigned char data[8];
        data[0] = 0x0E;
        data[1] = 2;
        data[2] = 1;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 108:
    {
        unsigned char data[8];
        data[0] = 0x0E;
        data[1] = 3;
        data[2] = 1;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 109:
    {
        unsigned char data[8];
        data[0] = 0x0C;
        data[1] = 3;
        data[2] = 1;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 110:
    case 111:
    {
        unsigned char data[8];
        data[0] = 0x0C;
        data[1] = 2;
        data[2] = 1;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    default:
        break;
    }
}

void CANGetAndSend::slotTimeout()
{
    step = 0;
    emit stopTimerSignal();
    emit signalReceiveStatus("超时未应答");
    if (if_mes)
    {
        switch (lastTestType)
        {
        case TestSupplyVoltage:
        {
            QJsonObject SupplyVoltage;
            SupplyVoltage.insert("standard", "untested");
            SupplyVoltage.insert("test", "untested");
            SupplyVoltage.insert("difference", "untested");
            SupplyVoltage.insert("allowable", "untested");
            SupplyVoltage.insert("result", "超时未应答");
            testResults.insert("SupplyVoltage", SupplyVoltage);
                emit signalErrorFinish();timeoutTimer.stop();
            break;
        }
        case TestHALLSignal:
        {
            QJsonObject HALLSignal;
            QJsonObject Five;
            Five.insert("standard", "untested");
            Five.insert("test", "untested");
            Five.insert("difference", "untested");
            Five.insert("allowable", "untested");
            Five.insert("result", "工装板超时未应答");
            HALLSignal.insert("Five", Five);
            QJsonObject Twelve;
            Twelve.insert("standard", "untested");
            Twelve.insert("test", "untested");
            Twelve.insert("difference", "untested");
            Twelve.insert("allowable", "untested");
            Twelve.insert("result", "工装板超时未应答");
            HALLSignal.insert("Twelve", Twelve);
            testResults.insert("HALLSignal", HALLSignal);
                emit signalErrorFinish();timeoutTimer.stop();
            break;
        }
        case TestHALLCurrent:
        {
            QJsonObject OneZheng;
            OneZheng.insert("standard", "untested");
            OneZheng.insert("test", "untested");
            OneZheng.insert("difference", "untested");
            OneZheng.insert("allowable", "untested");
            OneZheng.insert("result", "超时未应答");
            HALLCurrent.insert("OneZheng", OneZheng);
            QJsonObject OneFu;
            OneFu.insert("standard", "untested");
            OneFu.insert("test", "untested");
            OneFu.insert("difference", "untested");
            OneFu.insert("allowable", "untested");
            OneFu.insert("result", "超时未应答");
            HALLCurrent.insert("OneFu", OneFu);
            QJsonObject TwoZheng;
            TwoZheng.insert("standard", "untested");
            TwoZheng.insert("test", "untested");
            TwoZheng.insert("difference", "untested");
            TwoZheng.insert("allowable", "untested");
            TwoZheng.insert("result", "超时未应答");
            HALLCurrent.insert("TwoZheng", TwoZheng);
            QJsonObject TwoFu;
            TwoFu.insert("standard", "untested");
            TwoFu.insert("test", "untested");
            TwoFu.insert("difference", "untested");
            TwoFu.insert("allowable", "untested");
            TwoFu.insert("result", "超时未应答");
            HALLCurrent.insert("TwoFu", TwoFu);
            QJsonObject ThreeZheng;
            ThreeZheng.insert("standard", "untested");
            ThreeZheng.insert("test", "untested");
            ThreeZheng.insert("difference", "untested");
            ThreeZheng.insert("allowable", "untested");
            ThreeZheng.insert("result", "超时未应答");
            HALLCurrent.insert("ThreeZheng", ThreeZheng);
            QJsonObject ThreeFu;
            ThreeFu.insert("standard", "untested");
            ThreeFu.insert("test", "untested");
            ThreeFu.insert("difference", "untested");
            ThreeFu.insert("allowable", "untested");
            ThreeFu.insert("result", "超时未应答");
            HALLCurrent.insert("ThreeFu", ThreeFu);
            testResults.insert("HALLCurrent", HALLCurrent);
                emit signalErrorFinish();timeoutTimer.stop();
            break;
        }
        case TestSerialCommunication:
        {
            QJsonObject SerialCommunication;
            SerialCommunication.insert("standard", "untested");
            SerialCommunication.insert("test", "untested");
            SerialCommunication.insert("difference", "untested");
            SerialCommunication.insert("allowable", "untested");
            SerialCommunication.insert("result", "超时未应答");
            testResults.insert("SerialCommunication", SerialCommunication);
            break;
        }
        case TestCANCommunication:
        {
            if(step == 1)
            {
                CANCommunication.insert("CAN0", "未通过");
                updateTestItem();
            }
            else if(step == 2)
            {
                CANCommunication.insert("CAN1", "未通过");
                updateTestItem();
            }
            else if(step == 3)
            {
                CANCommunication.insert("CAN2", "未通过");
                testResults.insert("CANCommunication", CANCommunication);
                emit signalErrorFinish();timeoutTimer.stop();
                step = 0;
            }
            break;
        }
        default:
        {
            emit signalErrorFinish();timeoutTimer.stop();
            break;
        }
        }
    }
    else
    {
        ExcelItem temp;
        switch (lastTestType)
        {
        case TestBatteryVoltage:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestSupplyVoltage:
        {
            switch (m_type)
            {
            case 6:
            case 30:
            case 0:
            case 15:
            case 9:
            {
                ExcelItem temp;
                temp.row  = 101;
                temp.col  = 3;
                temp.word = "超时未应答";
                items.push_back(temp);
                break;
            }
            case 8:
            case 32:
            case 1:
            case 17:
            case 11:
            {
                ExcelItem temp;
                temp.row  = 102;
                temp.col  = 3;
                temp.word = "超时未应答";
                items.push_back(temp);
                break;
            }
            case 2:
            case 36:
            {
                ExcelItem temp;
                temp.row  = 107;
                temp.col  = 7;
                temp.word = "超时未应答";
                items.push_back(temp);
                break;
            }
            case 3:
            case 38:
            {
                ExcelItem temp;
                temp.row  = 102;
                temp.col  = 7;
                temp.word = "超时未应答";
                items.push_back(temp);
                break;
            }
            case 7:
            case 31:
            case 4:
            case 16:
            case 10:
            {
                ExcelItem temp;
                temp.row  = 108;
                temp.col  = 3;
                temp.word = "超时未应答";
                items.push_back(temp);
                break;
            }
            case 5:
            case 37:
            {
                ExcelItem temp;
                temp.row  = 108;
                temp.col  = 7;
                temp.word = "超时未应答";
                items.push_back(temp);
                break;
            }
            default:
                break;
            }
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestHALLSignal:
        {
            switch (m_type)
            {
            case 6:
            case 30:
            case 0:
            case 15:
            case 9:
            {
                temp.row  = 16;
                temp.col  = 3;
                temp.word = "工装板超时未应答";
                items.push_back(temp);
                break;
            }
            case 8:
            case 32:
            case 1:
            case 17:
            case 4:
            case 16:
            case 11:
            {
                temp.row  = 17;
                temp.col  = 3;
                temp.word = "工装板超时未应答";
                items.push_back(temp);
                break;
            }
            case 2:
            case 36:
            {
                temp.row  = 16;
                temp.col  = 3;
                temp.word = "工装板超时未应答";
                items.push_back(temp);
                break;
            }
            case 7:
            case 31:
            case 3:
            case 38:
            case 5:
            case 37:
            case 10:
            {
                temp.row  = 17;
                temp.col  = 3;
                temp.word = "工装板超时未应答";
                items.push_back(temp);
                break;
            }
            default:
                break;
            }
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestHALLCurrent:
        {
            switch (m_type)
            {
            case 0:
            case 15:
            case 2:
            case 36:
            case 6:
            case 30:
            case 9:
            {
                temp.row  = 23;
                temp.col  = 11;
                temp.word = "超时未应答";
                items.push_back(temp);
                temp.row  = 29;
                temp.col  = 11;
                temp.word = "超时未应答";
                items.push_back(temp);
                temp.row  = 35;
                temp.col  = 11;
                temp.word = "超时未应答";
                items.push_back(temp);
                temp.row  = 41;
                temp.col  = 11;
                temp.word = "超时未应答";
                items.push_back(temp);
                temp.row  = 47;
                temp.col  = 11;
                temp.word = "超时未应答";
                items.push_back(temp);
                temp.row  = 53;
                temp.col  = 11;
                temp.word = "超时未应答";
                items.push_back(temp);
                step = 0;

                break;
            }
            case 1:
            case 17:
            case 3:
            case 38:
            case 4:
            case 16:
            case 5:
            case 37:
            case 7:
            case 31:
            case 8:
            case 32:
            case 10:
            case 11:
            {
                temp.row  = 24;
                temp.col  = 11;
                temp.word = "超时未应答";
                items.push_back(temp);
                temp.row  = 30;
                temp.col  = 11;
                temp.word = "超时未应答";
                items.push_back(temp);
                temp.row  = 36;
                temp.col  = 11;
                temp.word = "超时未应答";
                items.push_back(temp);
                temp.row  = 42;
                temp.col  = 11;
                temp.word = "超时未应答";
                items.push_back(temp);
                temp.row  = 48;
                temp.col  = 11;
                temp.word = "超时未应答";
                items.push_back(temp);
                temp.row  = 54;
                temp.col  = 11;
                temp.word = "超时未应答";
                items.push_back(temp);
                step = 0;

                break;
            }
            default:
                break;
            }
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestSerialCommunication:
        {
            switch (m_type)
            {
            case 0:
            case 15:
            case 6:
            case 30:
            {
                temp.row  = 77;
                temp.col  = 3;
                temp.word = "超时未应答";
                break;
            }
            case 1:
            case 17:
            case 4:
            case 16:
            case 7:
            case 31:
            case 8:
            case 32:
            {
                temp.row  = 78;
                temp.col  = 3;
                temp.word = "超时未应答";
                break;
            }
            case 2:
            case 36:
            {
                temp.row  = 71;
                temp.col  = 3;
                temp.word = "超时未应答";
                break;
            }
            case 3:
            case 38:
            case 5:
            case 37:
            {
                temp.row  = 72;
                temp.col  = 3;
                temp.word = "超时未应答";
                break;
            }
            default:
                break;
            }
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestCANCommunication:
        {
            if(step == 1)
            {
                updateTestItem();
            }
            else if(step == 2)
            {
                updateTestItem();
            }
            else if(step == 3)
            {
                emit signalErrorFinish();timeoutTimer.stop();
                step = 0;
            }
            break;
        }
        case TestEEPROM:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestFlash:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestRTC:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestSystemTemperature:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestInternet:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestInsulationResistance:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestSoftWareVersion:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestHardWareVersion:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestSPI:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestSetMAC:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestReadMAC:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestSetSerial:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestReadSerial:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        case TestKB:
        {
            emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0;
            break;
        }
        default:
        {
            emit signalErrorFinish();timeoutTimer.stop();
            break;
        }
        }
    }
}

void CANGetAndSend::slotCurrentTimeout()
{
    step = 1;
    updateTestItem();
}

void CANGetAndSend::slotSleepTimeout()
{
    unsigned char data[8];
    data[0] = 0x0B;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    TransmitCAN(((0x1854 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
    emit signalReceiveStatus("功耗读取中。。。");
    //failTimer->start(10000);5000);
}

void CANGetAndSend::slotStorageTimeout()
{
    if (step == 1)
    {
        unsigned char data[8];
        data[0] = 0xFF;
        data[1] = 2;
        data[2] = 7;
        data[3] = 1;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1854 << 8) | ((m_locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        step = 0;
        emit signalReceiveStatus("DO7开启中。。。");
        QTimer *tempTimer = new QTimer(this);
        connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotStorageTimeout()));
        tempTimer->setSingleShot(true);
        tempTimer->start(10000);
    }
    else
    {
        unsigned char data[8];
        data[0] = 0x0F;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1854 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
        emit signalReceiveStatus("掉电存储标志读取中。。。");
    }
}

void CANGetAndSend::slotCPTimeout()
{
    unsigned char data[8];
    data[0] = 0x09;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    TransmitCAN(((0x1830 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
    emit signalReceiveStatus("CP读取中。。。");
}

void CANGetAndSend::slotWatchDogTimeout()
{
    step = 0;
    unsigned char data[8];
    data[0] = 0x10;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    TransmitCAN(((0x1854 << 8) | ((m_locate + 0x20) & 0xFF)) << 8 | 0xF4, data);
    emit signalReceiveStatus("读取复位次数中。。。");
}

void CANGetAndSend::slotSendSuccess() { emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0; }

void CANGetAndSend::slotSendFail() { emit signalErrorFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0; }

void CANGetAndSend::TransmitCAN(int ID, unsigned char data[8])
{
    VCI_CAN_OBJ vco[1];
    CAN_OBJ co;
    vco[0].ID         = ID;
    vco[0].RemoteFlag = 0;
    vco[0].ExternFlag = 1;
    vco[0].DataLen    = 8;
    co.DataLen        = 8;
    co.ID             = ID;
    co.RemoteFlag     = 0;
    co.ExternFlag     = 1;
    co.DataLen        = 8;
    for (int i = 0; i < 8; i++)
    {
        vco[0].Data[i] = data[i];
        co.Data[i]     = data[i];
    }

    // 直接写入全局共享Tx缓冲区，LCANBusFXYNEW内部线程自动读取发送
    bool ret = CanBufferMgr::GetInstance()->GetTxBuf().push(vco);
    if (!ret)
    {
        qDebug() << "缓冲区已满，CAN发送失败！";
    }
}

void CANGetAndSend::slotSetSoft(QString id) { m_software = id; }

void CANGetAndSend::slotSetHard(QString id) { m_hardware = id; }

void CANGetAndSend::slotVolOut() { updateTestItem(); }

void CANGetAndSend::slotMACTimeout() { emit signalFinish();timeoutTimer.stop();emit stopFailTimerSignal();step = 0; }

void CANGetAndSend::slotSetMod(bool mod)
{
    if_mes = mod;
}
