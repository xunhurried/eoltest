#ifndef SERIALPORTCONTROLLER_H
#define SERIALPORTCONTROLLER_H

#include <QObject>
#include <QThread>
#include <atomic>
#include "model/Rs485RingBuf.h"

class SerialPort;
class SerialPortController : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortController(QObject *parent = nullptr);
    ~SerialPortController() override;

    void slotStartSerialPort(const QString &id, int port);
    void stopSerialPort();
    void setMod(int mod);

signals:
    void signalSetMod(int);
    void signalTimeout();

private slots:
    void slotTimeout();

private:
    SerialPort* serialPort = nullptr;
    QThread* serialPortThread = nullptr;
};

#endif // SERIALPORTCONTROLLER_H
