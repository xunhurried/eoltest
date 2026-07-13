#ifndef CANBUSZLG_H
#define CANBUSZLG_H

#include "ControlCAN.h"
#include <QObject>
#include <windows.h>

enum CANZLGRETURNCODE
{
    ZLGOPENCANSUCCESS        = 1,
    ZLGOPENCANFAILED         = 2,
    ZLGCLOSECANSUCCESS       = 3,
    ZLGCLOSECANFAILED        = 4,
    ZLGCLOSEBEFOREOPEN       = 5,
    ZLGSTARTCANSUCCESS       = 6,
    ZLGSTARTCANFAILED        = 7,
    ZLGSTARTBEFOREOPEN       = 8,
    ZLGRESETCANSUCCESS       = 9,
    ZLGRESETCANFAILED        = 10,
    ZLGRESETBEFOREOPEN       = 11,
    ZLGRESETBEFORECONNECT    = 12,
    ZLGCONNECTCANFILED       = 13,
    ZLGCONNECTCANSUCCESS     = 14,
    ZLGCANERRORDATAFORM      = 15,
    ZLGCANERRORSHIEDCODEFORM = 16,
    ZLGCANERRORTIMER0FORM    = 17,
    ZLGCANERRORTIMER1FORM    = 18,
    ZLGINITIALCANFAILED      = 19,
    ZLGREADBEFOREOPEN        = 20,
    ZLGREADBEFORECONN        = 21,
    ZLGREADBEFORESTART       = 22,
    ZLGREADCANERROR          = 23,
    ZLGCRCERROR              = 24,
    ZLGREADCANSUCCESS        = 25
};

class LCANBusZLG : public QObject
{
    Q_OBJECT
public:
    LCANBusZLG(QObject *parent = nullptr);
    ~LCANBusZLG();
    bool changePort(int port);

private:
    CANZLGRETURNCODE onConnect(int m_devCANIndex, int port);
    int charToInt(unsigned char chr, unsigned char *cint);
    int strToData(unsigned char *str, unsigned char *data,int len,int flag);
    void unsigned_to_hex(unsigned int value, std::string& hex_string);

public slots:
    void slotOpenCAN(int m_devtypeno, int m_devindexno);
    void slotCloseCAN(int m_devtypeno, int m_devindexno);
    void slotStartCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex, int port);
    void slotResetCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex);
    void slotReceiveCAN();
    void slotTransmitCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex, PVCI_CAN_OBJ pSend, ULONG Len);
    void slotChangePort();

private:
    bool isOpen;
    bool isConn;
    bool isStart;
    bool isReset;
    int mDevMode;
    int mDevFilterType;
    int mDevTypeno; //设备类型号
    int mDevIndexno; //设备索引号
    int mDevCANIndex; //CAN通道号
    DWORD mTiming;
    char mSzCode[10];
    char mSzMask[10];
    char mSzTiming0[10];
    char mSzTiming1[10];
    QByteArray currentData;
    int m_port = 10;

signals:
    void signalCANStatus(CANZLGRETURNCODE status);
    void signalReceiveData(VCI_CAN_OBJ pReceive);
    void signalReceiveData2(VCI_CAN_OBJ pReceive);
};

#endif // CANBusZLG_H
