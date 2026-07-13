#ifndef CANBUSFXYNEW_H
#define CANBUSFXYNEW_H

#include <QObject>
#include <QThread>
#include <atomic>
#include "canringbuffer.h"
#include "include/ControlCAN.h"

enum CANFXYNEWRETURNCODE
{
    FXYNEWOPENCANSUCCESS        = 1,
    FXYNEWOPENCANFAILED         = 2,
    FXYNEWCLOSECANSUCCESS       = 3,
    FXYNEWCLOSECANFAILED        = 4,
    FXYNEWCLOSEBEFOREOPEN       = 5,
    FXYNEWSTARTCANSUCCESS       = 6,
    FXYNEWSTARTCANFAILED        = 7,
    FXYNEWSTARTBEFOREOPEN       = 8,
    FXYNEWRESETCANSUCCESS       = 9,
    FXYNEWRESETCANFAILED        = 10,
    FXYNEWRESETBEFOREOPEN       = 11,
    FXYNEWRESETBEFORECONNECT    = 12,
    FXYNEWCONNECTCANFILED       = 13,
    FXYNEWCONNECTCANSUCCESS     = 14,
    FXYNEWCANERRORDATAFORM      = 15,
    FXYNEWCANERRORSHIEDCODEFORM = 16,
    FXYNEWCANERRORTIMER0FORM    = 17,
    FXYNEWCANERRORTIMER1FORM    = 18,
    FXYNEWINITIALCANFAILED      = 19,
    FXYNEWREADBEFOREOPEN        = 20,
    FXYNEWREADBEFORECONN        = 21,
    FXYNEWREADBEFORESTART       = 22,
    FXYNEWREADCANERROR          = 23,
    FXYNEWCRCERROR              = 24,
    FXYNEWREADCANSUCCESS        = 25
};

class LCANBusFXYNEW;
class CanSendPollThreadNEW : public QThread
{
public:
    std::atomic<bool> m_running{false};
    LCANBusFXYNEW* m_canObj = nullptr;

    void SetCanObj(LCANBusFXYNEW* p) { m_canObj = p; }
    void StartPoll()
    {
        m_running = true;
        start();
    }
    void StopPoll()
    {
        m_running = false;
        CanBufferMgr::GetInstance()->GetRxBuf().wakeAll();
        if (!wait(500))
        {
            terminate();
            wait();
        }
    }

protected:
    void run() override;
};

class LCANBusFXYNEW : public QObject
{
    Q_OBJECT
public:
    explicit LCANBusFXYNEW(QObject *parent = nullptr);
    ~LCANBusFXYNEW() override;

    // 启停内部发送轮询线程
    void StartSendPollThread();
    void StopSendPollThread();
    void slotTransmitCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex, PVCI_CAN_OBJ pSend, ULONG Len);

signals:
    void signalCANStatus(CANFXYNEWRETURNCODE code);
    void signalReceiveData(PVCI_CAN_OBJ frame);

private slots:
    void slotOpenCAN(int m_devtypeno, int m_devindexno);
    void slotCloseCAN(int m_devtypeno, int m_devindexno);
    void slotStartCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex, int port);
    void slotResetCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex);
    void slotReceiveCAN();
    void slotChangePort();

private:
    CANFXYNEWRETURNCODE onConnect(int m_devCANIndex, int port);
    int charToInt(unsigned char chr, unsigned char *cint);
    int strToData(unsigned char *str, unsigned char *data,int len,int flag);
    void unsigned_to_hex(unsigned int value, std::string &hex_string);

public:
    bool isOpen;
    bool isConn;
    bool isStart;
    bool isReset;
    int mDevMode;
    int mDevTypeno;
    int mDevIndexno;
    int mDevCANIndex;
    int m_port;
    QByteArray currentData;
    DWORD mTiming;

    CanSendPollThreadNEW m_sendPollThread;
};


#endif // CANBUSFXYNEW_H
