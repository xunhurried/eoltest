#ifndef CANBUSGC_H
#define CANBUSGC_H

#include "ECanVci.h"
#include <QObject>
#include <windows.h>

enum CANGCRETURNCODE
{
    GCOPENCANSUCCESS        = 1,
    GCOPENCANFAILED         = 2,
    GCCLOSECANSUCCESS       = 3,
    GCCLOSECANFAILED        = 4,
    GCCLOSEBEFOREOPEN       = 5,
    GCSTARTCANSUCCESS       = 6,
    GCSTARTCANFAILED        = 7,
    GCSTARTBEFOREOPEN       = 8,
    GCRESETCANSUCCESS       = 9,
    GCRESETCANFAILED        = 10,
    GCRESETBEFOREOPEN       = 11,
    GCRESETBEFORECONNECT    = 12,
    GCCONNECTCANFILED       = 13,
    GCCONNECTCANSUCCESS     = 14,
    GCCANERRORDATAFORM      = 15,
    GCCANERRORSHIEDCODEFORM = 16,
    GCCANERRORTIMER0FORM    = 17,
    GCCANERRORTIMER1FORM    = 18,
    GCINITIALCANFAILED      = 19,
    GCREADBEFOREOPEN        = 20,
    GCREADBEFORECONN        = 21,
    GCREADBEFORESTART       = 22,
    GCREADCANERROR          = 23,
    GCCRCERROR              = 24,
    GCREADCANSUCCESS        = 25
};

class LCANBusGC : public QObject
{
    Q_OBJECT
public:
    LCANBusGC(QObject *parent = nullptr);
    ~LCANBusGC();
    bool changePort(int port);

private:
    CANGCRETURNCODE onConnect(int m_devCANIndex, int port);
    int charToInt(unsigned char chr, unsigned char *cint);
    int strToData(unsigned char *str, unsigned char *data,int len,int flag);
    void unsigned_to_hex(unsigned int value, std::string& hex_string);

public slots:
    void slotOpenCAN(int m_devtypeno, int m_devindexno);
    void slotCloseCAN(int m_devtypeno, int m_devindexno);
    void slotStartCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex, int port);
    void slotResetCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex);
    void slotReceiveCAN();
    void slotTransmitCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex, CAN_OBJ pSend, ULONG Len);
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
    int m_port = 0;

signals:
    void signalCANStatus(CANGCRETURNCODE status);
    void signalReceiveData(CAN_OBJ pReceive);
    void signalReceiveData2(CAN_OBJ pReceive);
};

#endif // CANBUSGC_H
