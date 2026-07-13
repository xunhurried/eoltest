#include "canbuszlg.h"
#include <QString>
#include <QTimer>
#include <QDebug>
#include <strstream>

LCANBusZLG::LCANBusZLG(QObject *parent) : QObject(parent), isOpen(false), isConn(false), isStart(false),
    isReset(false), mDevMode(0), mDevTypeno(4), mDevIndexno(0), mDevCANIndex(0)
{

}

LCANBusZLG::~LCANBusZLG()
{

}

bool LCANBusZLG::changePort(int port)
{
    m_port = port;
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotChangePort()));
    tempTimer->setSingleShot(true);
    tempTimer->start(200);
    VCI_CloseDevice(mDevTypeno, mDevIndexno);
    return true;
}

CANZLGRETURNCODE LCANBusZLG::onConnect(int m_devCANIndex, int port)
{
    if (!isOpen)
    {
        isConn = false;
        return ZLGSTARTBEFOREOPEN;
    }
    mDevCANIndex = m_devCANIndex;
    VCI_INIT_CONFIG vic;
    vic.AccCode = 0x00000000;
    vic.AccMask = 0xFFFFFFFF;
    vic.Filter = 1;
    switch(port)
    {
    case 0:
    {
        vic.Timing0 = 0x03;
        vic.Timing1 = 0x1C;
        break;
    }
    case 1:
    {
        vic.Timing0 = 0x01;
        vic.Timing1 = 0x1C;
        break;
    }
    case 2:
    {
        vic.Timing0 = 0x00;
        vic.Timing1 = 0x1C;
        break;
    }
    case 3:
    {
        vic.Timing0 = 0x00;
        vic.Timing1 = 0x16;
        break;
    }
    case 4:
    {
        vic.Timing0 = 0x00;
        vic.Timing1 = 0x14;
        break;
    }
    default:
        break;
    }
    vic.Mode = 0;
    if (mDevTypeno < 10)
    {
        if(VCI_InitCAN(mDevTypeno, mDevIndexno, mDevCANIndex, &vic) != STATUS_OK)
        {
            VCI_CloseDevice(mDevTypeno, mDevIndexno);
            isOpen = false;
            isConn = false;
            return ZLGINITIALCANFAILED;
        }
    }
    else
    {
        if(VCI_SetReference(mDevTypeno, mDevIndexno, mDevCANIndex, 0, &(mTiming)) != STATUS_OK)
        {
            VCI_CloseDevice(mDevTypeno, mDevIndexno);
            isOpen = false;
            isConn = false;
            return ZLGINITIALCANFAILED;
        }

        if(VCI_InitCAN(mDevTypeno, mDevIndexno, mDevCANIndex, &vic) != STATUS_OK)
        {
            VCI_CloseDevice(mDevTypeno, mDevIndexno);
            isOpen = false;
            isConn = false;
            return ZLGINITIALCANFAILED;
        }
    }
    isConn = true;
    QTimer *readTimer = new QTimer;
    readTimer->setSingleShot(false);
    connect(readTimer, SIGNAL(timeout()), this, SLOT(slotReceiveCAN()));
    readTimer->start(100);
    return ZLGCONNECTCANSUCCESS;
}

int LCANBusZLG::charToInt(unsigned char chr, unsigned char *cint)
{
    unsigned char cTmp;
    cTmp = chr - 48;
    if(cTmp >= 0 && cTmp <= 9)
    {
        *cint = cTmp;
        return 0;
    }
    cTmp = chr - 65;
    if(cTmp >= 0 && cTmp <= 5)
    {
        *cint= (cTmp + 10);
        return 0;
    }
    cTmp = chr - 97;
    if(cTmp >= 0 && cTmp <= 5)
    {
        *cint = (cTmp + 10);
        return 0;
    }
    return 1;
}

int LCANBusZLG::strToData(unsigned char *str, unsigned char *data,int len,int flag)
{
    unsigned char cTmp = 0;
    int i = 0;
    for(int j = 0;j < len; j++)
    {
        if(charToInt(str[i++], &cTmp))
        {
            return 1;
        }
        data[j] = cTmp;
        if(charToInt(str[i++], &cTmp))
        {
            return 1;
        }
        data[j] = (data[j] << 4) + cTmp;
        if(flag == 1)
        {
            i++;
        }
    }
    return 0;
}

void LCANBusZLG::unsigned_to_hex(unsigned int value, std::string &hex_string)
{
    std::strstream buffer;
    buffer.setf(std::ios::showbase);
    buffer <<std::hex << value;
    buffer >> hex_string;
}

void LCANBusZLG::slotOpenCAN(int m_devtypeno, int m_devindexno)
{
    mDevTypeno = m_devtypeno;
    mDevIndexno = m_devindexno;
    DWORD ret = VCI_OpenDevice(m_devtypeno, m_devindexno, 0);
    if(ret != STATUS_OK)
    {
        isOpen = false;
        emit signalCANStatus(ZLGOPENCANFAILED);
    }
    else
    {
        isOpen = true;
        emit signalCANStatus(ZLGOPENCANSUCCESS);
    }
}

void LCANBusZLG::slotCloseCAN(int m_devtypeno, int m_devindexno)
{
    mDevTypeno = m_devtypeno;
    mDevIndexno = m_devindexno;
    if(isOpen)
    {
        DWORD ret = VCI_CloseDevice(m_devtypeno, m_devindexno);
        if(ret != STATUS_OK)
        {
            emit signalCANStatus(ZLGCLOSECANFAILED);
        }
        {
            isOpen = false;
            emit signalCANStatus(ZLGCLOSECANSUCCESS);
        }
    }
    else
    {
        emit signalCANStatus(ZLGCLOSEBEFOREOPEN);
    }
}

void LCANBusZLG::slotStartCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex, int port)
{
    mDevTypeno = m_devtypeno;
    mDevIndexno = m_devindexno;
    mDevCANIndex = m_devCANIndex;
    m_port = port;
    qDebug() << "mDevTypeno is "  << m_devtypeno <<
                " mDevIndexno is " <<  m_devindexno <<
                " mDevCANIndex is " <<  m_devCANIndex;
    CANZLGRETURNCODE temp = onConnect(m_devCANIndex, m_port);
    if(ZLGCONNECTCANSUCCESS != temp)
    {
        emit signalCANStatus(temp);
    }
    if(!isConn)
    {
        emit signalCANStatus(ZLGCONNECTCANFILED);
    }
    if(VCI_StartCAN(m_devtypeno, m_devindexno, 0) == STATUS_OK && VCI_StartCAN(m_devtypeno, m_devindexno, 1) == STATUS_OK)
    {
        isStart = true;
        isReset = false;
        emit signalCANStatus(ZLGSTARTCANSUCCESS);
    }
    else
    {
        isStart = false;
        emit signalCANStatus(ZLGSTARTCANFAILED);
    }
}

void LCANBusZLG::slotResetCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex)
{
    mDevTypeno = m_devtypeno;
    mDevIndexno = m_devindexno;
    mDevCANIndex = m_devCANIndex;
    if (!isOpen)
    {
        emit signalCANStatus(ZLGRESETBEFOREOPEN);
    }

    if(!isConn)
    {
        emit signalCANStatus(ZLGRESETBEFORECONNECT);
    }

    if(VCI_ResetCAN(m_devtypeno, m_devindexno, m_devCANIndex) == STATUS_OK)
    {
        isReset = true;
        isStart = false;
        emit signalCANStatus(ZLGRESETCANSUCCESS);
    }
    else
    {
        isReset = false;
        emit signalCANStatus(ZLGRESETCANFAILED);
    }
}

void LCANBusZLG::slotReceiveCAN()
{
    if(!isOpen)
    {
        emit signalCANStatus(ZLGREADBEFOREOPEN);
    }
    if(!isConn)
    {
        emit signalCANStatus(ZLGREADBEFORECONN);
    }
    if(!isStart)
    {
        emit signalCANStatus(ZLGREADBEFORESTART);
    }
    VCI_CAN_OBJ frameinfo[2500];
    for (int i = 0; i < 2500; i++)
    {
        memset(&(frameinfo[i]), 0, sizeof(VCI_CAN_OBJ));
    }
    int framelen = 0;
    framelen = VCI_Receive(mDevTypeno, mDevIndexno, 0, frameinfo, 2500, 0);
    if(0xFFFFFFFF != framelen && framelen >= 0)
    {
        for(int i = 0; i < framelen; i++)
        {
            if(0x18 == frameinfo[i].ID >> 24)
            {
                emit signalReceiveData(frameinfo[i]);
                emit signalCANStatus(ZLGREADCANSUCCESS);
            }
        }
    }
    framelen = VCI_Receive(mDevTypeno, mDevIndexno, 1, frameinfo, 2500, 0);
    if(0xFFFFFFFF != framelen && framelen >= 0)
    {
        for(int i = 0; i < framelen; i++)
        {
            emit signalReceiveData2(frameinfo[i]);
            emit signalCANStatus(ZLGREADCANSUCCESS);
        }
    }
}

void LCANBusZLG::slotTransmitCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex, PVCI_CAN_OBJ pSend, ULONG Len)
{
    mDevTypeno = m_devtypeno;
    mDevIndexno = m_devindexno;
    mDevCANIndex = m_devCANIndex;
    currentData.clear();
    for(int i = 0; i < 8; i++)
    {
        currentData.append(pSend->Data[i]);
    }
    currentData = currentData.toHex();
    std::string tempString;
    unsigned_to_hex(pSend->ID, tempString);
    qDebug() << "send " << currentData << " to " << QString::fromStdString(tempString);
    int lenth = VCI_Transmit(mDevTypeno, mDevIndexno, mDevCANIndex, pSend, Len);
}

void LCANBusZLG::slotChangePort()
{
    switch(m_port)
    {
    case 1:
    {
        VCI_INIT_CONFIG vic;
        vic.AccCode = 0x00000000;
        vic.AccMask = 0xFFFFFFFF;
        vic.Filter = 1;
        vic.Timing0 = 0x03;
        vic.Timing1 = 0x1C;
        vic.Mode = 0;
        if (mDevTypeno < 10)
        {
            DWORD ret = VCI_OpenDevice(mDevTypeno, mDevIndexno, 0);
            if(ret != STATUS_OK)
            {
                isOpen = false;
            }
            else
            {
                isOpen = true;
                if(VCI_InitCAN(mDevTypeno, mDevIndexno, mDevCANIndex, &vic) != STATUS_OK)
                {
                    VCI_CloseDevice(mDevTypeno, mDevIndexno);
                    isOpen = false;
                    isConn = false;
                }
                else
                {
                    if(VCI_StartCAN(mDevTypeno, mDevIndexno, mDevCANIndex) == STATUS_OK)
                    {
                        isStart = true;
                        isReset = false;
                    }
                    else
                    {
                        isStart = false;
                    }
                }
            }
        }
        else
        {
            if(VCI_SetReference(mDevTypeno, mDevIndexno, mDevCANIndex, 0, &(mTiming)) != STATUS_OK)
            {
                VCI_CloseDevice(mDevTypeno, mDevIndexno);
                isOpen = false;
                isConn = false;
            }
            else
            {
            }
            if(VCI_InitCAN(mDevTypeno, mDevIndexno, mDevCANIndex, &vic) != STATUS_OK)
            {
                VCI_CloseDevice(mDevTypeno, mDevIndexno);
                isOpen = false;
                isConn = false;
            }
            else
            {
            }
        }
        break;
    }
    case 2:
    {
        VCI_INIT_CONFIG vic;
        vic.AccCode = 0x00000000;
        vic.AccMask = 0xFFFFFFFF;
        vic.Filter = 1;
        vic.Timing0 = 0x01;
        vic.Timing1 = 0x1C;
        vic.Mode = 0;
        if (mDevTypeno < 10)
        {
            DWORD ret = VCI_OpenDevice(mDevTypeno, mDevIndexno, 0);
            if(ret != STATUS_OK)
            {
                isOpen = false;
            }
            else
            {
                isOpen = true;
                if(VCI_InitCAN(mDevTypeno, mDevIndexno, mDevCANIndex, &vic) != STATUS_OK)
                {
                    VCI_CloseDevice(mDevTypeno, mDevIndexno);
                    isOpen = false;
                    isConn = false;
                }
                else
                {
                    if(VCI_StartCAN(mDevTypeno, mDevIndexno, mDevCANIndex) == STATUS_OK)
                    {
                        isStart = true;
                        isReset = false;
                    }
                    else
                    {
                        isStart = false;
                    }
                }
            }
        }
        else
        {
            if(VCI_SetReference(mDevTypeno, mDevIndexno, mDevCANIndex, 0, &(mTiming)) != STATUS_OK)
            {
                VCI_CloseDevice(mDevTypeno, mDevIndexno);
                isOpen = false;
                isConn = false;
            }
            else
            {
            }
            if(VCI_InitCAN(mDevTypeno, mDevIndexno, mDevCANIndex, &vic) != STATUS_OK)
            {
                VCI_CloseDevice(mDevTypeno, mDevIndexno);
                isOpen = false;
                isConn = false;
            }
            else
            {
            }
        }
        break;
    }
    case 3:
    {
        VCI_INIT_CONFIG vic;
        vic.AccCode = 0x00000000;
        vic.AccMask = 0xFFFFFFFF;
        vic.Filter = 1;
        vic.Timing0 = 0x00;
        vic.Timing1 = 0x1C;
        vic.Mode = 0;
        if (mDevTypeno < 10)
        {
            DWORD ret = VCI_OpenDevice(mDevTypeno, mDevIndexno, 0);
            if(ret != STATUS_OK)
            {
                isOpen = false;
            }
            else
            {
                isOpen = true;
                if(VCI_InitCAN(mDevTypeno, mDevIndexno, mDevCANIndex, &vic) != STATUS_OK)
                {
                    VCI_CloseDevice(mDevTypeno, mDevIndexno);
                    isOpen = false;
                    isConn = false;
                }
                else
                {
                    if(VCI_StartCAN(mDevTypeno, mDevIndexno, mDevCANIndex) == STATUS_OK)
                    {
                        isStart = true;
                        isReset = false;
                    }
                    else
                    {
                        isStart = false;
                    }
                }
            }
        }
        else
        {
            if(VCI_SetReference(mDevTypeno, mDevIndexno, mDevCANIndex, 0, &(mTiming)) != STATUS_OK)
            {
                VCI_CloseDevice(mDevTypeno, mDevIndexno);
                isOpen = false;
                isConn = false;
            }
            else
            {
            }
            if(VCI_InitCAN(mDevTypeno, mDevIndexno, mDevCANIndex, &vic) != STATUS_OK)
            {
                VCI_CloseDevice(mDevTypeno, mDevIndexno);
                isOpen = false;
                isConn = false;
            }
            else
            {
            }
        }
        break;
    }
    case 4:
    {
        VCI_INIT_CONFIG vic;
        vic.AccCode = 0x00000000;
        vic.AccMask = 0xFFFFFFFF;
        vic.Filter = 1;
        vic.Timing0 = 0x00;
        vic.Timing1 = 0x16;
        vic.Mode = 0;
        if (mDevTypeno < 10)
        {
            DWORD ret = VCI_OpenDevice(mDevTypeno, mDevIndexno, 0);
            if(ret != STATUS_OK)
            {
                isOpen = false;
            }
            else
            {
                isOpen = true;
                if(VCI_InitCAN(mDevTypeno, mDevIndexno, mDevCANIndex, &vic) != STATUS_OK)
                {
                    VCI_CloseDevice(mDevTypeno, mDevIndexno);
                    isOpen = false;
                    isConn = false;
                }
                else
                {
                    if(VCI_StartCAN(mDevTypeno, mDevIndexno, mDevCANIndex) == STATUS_OK)
                    {
                        isStart = true;
                        isReset = false;
                    }
                    else
                    {
                        isStart = false;
                    }
                }
            }
        }
        else
        {
            if(VCI_SetReference(mDevTypeno, mDevIndexno, mDevCANIndex, 0, &(mTiming)) != STATUS_OK)
            {
                VCI_CloseDevice(mDevTypeno, mDevIndexno);
                isOpen = false;
                isConn = false;
            }
            else
            {
            }
            if(VCI_InitCAN(mDevTypeno, mDevIndexno, mDevCANIndex, &vic) != STATUS_OK)
            {
                VCI_CloseDevice(mDevTypeno, mDevIndexno);
                isOpen = false;
                isConn = false;
            }
            else
            {
            }
        }
        break;
    }
    case 5:
    {
        VCI_INIT_CONFIG vic;
        vic.AccCode = 0x00000000;
        vic.AccMask = 0xFFFFFFFF;
        vic.Filter = 1;
        vic.Timing0 = 0x00;
        vic.Timing1 = 0x14;
        vic.Mode = 0;
        if (mDevTypeno < 10)
        {
            DWORD ret = VCI_OpenDevice(mDevTypeno, mDevIndexno, 0);
            if(ret != STATUS_OK)
            {
                isOpen = false;
            }
            else
            {
                isOpen = true;
                if(VCI_InitCAN(mDevTypeno, mDevIndexno, mDevCANIndex, &vic) != STATUS_OK)
                {
                    VCI_CloseDevice(mDevTypeno, mDevIndexno);
                    isOpen = false;
                    isConn = false;
                }
                else
                {
                    if(VCI_StartCAN(mDevTypeno, mDevIndexno, mDevCANIndex) == STATUS_OK)
                    {
                        isStart = true;
                        isReset = false;
                    }
                    else
                    {
                        isStart = false;
                    }
                }
            }
        }
        else
        {
            if(VCI_SetReference(mDevTypeno, mDevIndexno, mDevCANIndex, 0, &(mTiming)) != STATUS_OK)
            {
                VCI_CloseDevice(mDevTypeno, mDevIndexno);
                isOpen = false;
                isConn = false;
            }
            else
            {
            }
            if(VCI_InitCAN(mDevTypeno, mDevIndexno, mDevCANIndex, &vic) != STATUS_OK)
            {
                VCI_CloseDevice(mDevTypeno, mDevIndexno);
                isOpen = false;
                isConn = false;
            }
            else
            {
            }
        }
        break;
    }
    default:
        break;
    }
}
