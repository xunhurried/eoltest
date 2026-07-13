#ifndef PING_H
#define PING_H
#pragma once

#include <QObject>
#include <QTimer>
#include <QProcess>

class Ping : public QObject
{
    Q_OBJECT
public:
    Ping(QObject *parent = nullptr);
    virtual ~Ping();
    bool pingIP(QString ip);
    bool pingIP(QString ip1, QString ip2);
    void stopPing();

private:
    QProcess*  cmd;
    QTimer *timer;

private:
    void sendMsgBox(bool is_ok);

signals:
    void signalSuccess();
    void signalFail(int);
};

#endif // PING_H

