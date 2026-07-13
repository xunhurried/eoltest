#include "serialportcontroller.h"
#include "model/SerialPort.hpp"
#include <QDebug>
#include <QThread>
#include <QDateTime>

SerialPortController::SerialPortController(QObject *parent)
    : QObject{parent}, serialPort(NULL), serialPortThread(NULL)
{

}


SerialPortController::~SerialPortController()
{
    stopSerialPort();
}

void SerialPortController::slotStartSerialPort(const QString &id, int port)
{
    qDebug() << "[SERIAL-CTRL] start request"
             << "port" << id
             << "baud" << port
             << "thread" << QThread::currentThread()
             << "threadId" << QThread::currentThreadId();
    stopSerialPort();
    serialPort = new SerialPort(id.toLatin1(), port);
    serialPortThread = new QThread(this);
    QThread *threadPtr = serialPortThread;
    serialPort->moveToThread(serialPortThread);
    connect(this, SIGNAL(signalSetMod(int)), serialPort, SLOT(slotSetMod(int)));
    connect(serialPort, SIGNAL(signalTimeout()), this, SLOT(slotTimeout()));
    connect(serialPortThread, SIGNAL(finished()), serialPort, SLOT(deleteLater()));
    connect(serialPortThread, &QThread::finished, this, [this, threadPtr]()
    {
        qDebug() << "[SERIAL-CTRL] serial thread finished"
                 << "threadObj" << threadPtr;
    });
    serialPortThread->start(QThread::TimeCriticalPriority);
    qDebug() << "[SERIAL-CTRL] start thread"
             << "threadObj" << serialPortThread;
}

void SerialPortController::slotTimeout()
{
    emit signalTimeout();
}

void SerialPortController::setMod(int mod)
{
    emit signalSetMod(mod);
}

void SerialPortController::stopSerialPort()
{
    qDebug() << "[SERIAL-CTRL] stop request"
             << "hasPort" << (serialPort != NULL)
             << "hasThread" << (serialPortThread != NULL)
             << "thread" << QThread::currentThread()
             << "threadId" << QThread::currentThreadId();

    // 先停止485发送线程

    if (serialPort)
    {
        if (serialPort->thread() == QThread::currentThread() || !serialPortThread || !serialPortThread->isRunning())
        {
            serialPort->slotStop();
        }
        else
        {
            QMetaObject::invokeMethod(serialPort, "slotStop", Qt::BlockingQueuedConnection);
        }
    }
    if (serialPortThread)
    {
        serialPortThread->quit();
        serialPortThread->wait();
        delete serialPortThread;
        qDebug() << "[SERIAL-CTRL] thread stopped";
        serialPortThread = NULL;
    }
    serialPort = NULL;
}
