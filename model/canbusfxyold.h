#ifndef CANBUSFXYOLD_H
#define CANBUSFXYOLD_H

#include <QObject>
#include <QThread>
#include <atomic>
#include "canringbuffer.h"
#include "include/ControlCAN.h"

enum CANFXYOLDRETURNCODE
{
    FXYOLDOPENCANSUCCESS        = 1,
    FXYOLDOPENCANFAILED         = 2,
    FXYOLDCLOSECANSUCCESS       = 3,
    FXYOLDCLOSECANFAILED        = 4,
    FXYOLDCLOSEBEFOREOPEN       = 5,
    FXYOLDSTARTCANSUCCESS       = 6,
    FXYOLDSTARTCANFAILED        = 7,
    FXYOLDSTARTBEFOREOPEN       = 8,
    FXYOLDRESETCANSUCCESS       = 9,
    FXYOLDRESETCANFAILED        = 10,
    FXYOLDRESETBEFOREOPEN       = 11,
    FXYOLDRESETBEFORECONNECT    = 12,
    FXYOLDCONNECTCANFILED       = 13,
    FXYOLDCONNECTCANSUCCESS     = 14,
    FXYOLDCANERRORDATAFORM      = 15,
    FXYOLDCANERRORSHIEDCODEFORM = 16,
    FXYOLDCANERRORTIMER0FORM    = 17,
    FXYOLDCANERRORTIMER1FORM    = 18,
    FXYOLDINITIALCANFAILED      = 19,
    FXYOLDREADBEFOREOPEN        = 20,
    FXYOLDREADBEFORECONN        = 21,
    FXYOLDREADBEFORESTART       = 22,
    FXYOLDREADCANERROR          = 23,
    FXYOLDCRCERROR              = 24,
    FXYOLDREADCANSUCCESS        = 25
};

class LCANBusFXYOLD;
class CanSendPollThreadOLD : public QThread
{
public:
    std::atomic<bool> m_running{false};
    LCANBusFXYOLD* m_canObj = nullptr;

    void SetCanObj(LCANBusFXYOLD* p) { m_canObj = p; }
    void StartPoll()
    {
        m_running = true;
        start();
    }
    void StopPoll()
    {
        m_running = false;
        CanBufferMgr::GetInstance()->GetRxBuf().wakeAll();
        // 等待，最多500ms超时，避免永久阻塞
        if (!wait(500))
        {
            // 超时还没退出，强制终止（兜底）
            terminate();
            wait();
        }
    }

protected:
    void run() override;
};

class LCANBusFXYOLD : public QObject
{
    Q_OBJECT
public:
    explicit LCANBusFXYOLD(QObject *parent = nullptr);
    ~LCANBusFXYOLD() override;

    // 启停内部发送轮询线程
    void StartSendPollThread();
    void StopSendPollThread();
    void slotTransmitCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex, PVCI_CAN_OBJ pSend, ULONG Len);

signals:
    void signalCANStatus(CANFXYOLDRETURNCODE code);
    void signalReceiveData(PVCI_CAN_OBJ frame);

private slots:
    void slotOpenCAN(int m_devtypeno, int m_devindexno);
    void slotCloseCAN(int m_devtypeno, int m_devindexno);
    void slotStartCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex, int port);
    void slotResetCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex);
    void slotReceiveCAN();
    void slotChangePort();

private:
    CANFXYOLDRETURNCODE onConnect(int m_devCANIndex, int port);
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

    CanSendPollThreadOLD m_sendPollThread;
};


#endif // CANBUSFXYOLD_H
