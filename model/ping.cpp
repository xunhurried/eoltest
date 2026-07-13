#include "ping.h"
#include <QDebug>

Ping::Ping(QObject *parent) : QObject(parent)
{
}

Ping::~Ping()
{
    if(timer->isActive())
    {
        timer->stop();
        delete timer;
    }
    cmd->close();
    cmd->kill();
    delete cmd;
}

bool Ping::pingIP(QString ip)
{
    cmd = new QProcess();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [=]()
    {
        QString arp;
        arp = "arp -d " + ip;
        cmd->start(arp);
        while (cmd->waitForFinished())
        {
        }
        QString ping = "ping ";
        ping += ip;
        ping += " -l 1024 -n 4";
        cmd->start(ping);
        // 等待ping 的结果
        while (cmd->waitForFinished())
        {
            QString result = QString::fromLocal8Bit(cmd->readAll());
            if (result.count("TTL") != 4)
            {
                qDebug() << "cmd out put " << result;
                emit signalFail(result.count("TTL"));
            }
            else
            {
                emit signalSuccess();
            }
        }
    });
    timer->start(1000);
}

bool Ping::pingIP(QString ip1, QString ip2)
{
    cmd = new QProcess();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [=]()
    {
        QString arp;
        arp = "arp -d " + ip1;
        cmd->start(arp);
        while (cmd->waitForFinished())
        {
        }
        arp = "arp -d " + ip2;
        cmd->start(arp);
        while (cmd->waitForFinished())
        {
        }
        QString ping = "ping ";
        ping += ip1;
        ping += " -l 1024 -n 4";
        cmd->start(ping);
        // 等待ping 的结果
        while (cmd->waitForFinished())
        {
            QString result = QString::fromLocal8Bit(cmd->readAll());
            if (result.count("TTL") != 4)
            {
                qDebug() << "cmd out put " << result;
                emit signalFail(result.count("TTL"));
            }
            else
            {
                QString ping = "ping ";
                ping += ip2;
                ping += " -l 1024 -n 4";
                cmd->start(ping);
                // 等待ping 的结果
                while (cmd->waitForFinished())
                {
                    QString result = QString::fromLocal8Bit(cmd->readAll());
                    if (result.count("TTL") != 4)
                    {
                        qDebug() << "cmd out put " << result;
                        emit signalFail(result.count("TTL"));
                    }
                    else
                    {
                        emit signalSuccess();
                    }
                }
            }
        }
    });
    timer->start(1000);
}

void Ping::stopPing()
{
    if(timer)
    {
        if(timer->isActive())
        {
            timer->stop();
        }
    }
    if(cmd)
    {
        if(cmd->isOpen())
        {
            cmd->close();
            cmd->kill();
        }
    }
}

