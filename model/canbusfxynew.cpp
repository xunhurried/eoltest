#include "canbusfxynew.h"
#include <QString>
#include <QTimer>
#include <QDebug>
#include <strstream>

void CanSendPollThreadNEW::run()
{
    auto pollTask = [this]()
    {
        if (!m_canObj) return;
        // 直接访问全局共享Tx缓冲区，所有业务线程写入这里
        CanRingBuffer& txBuf = CanBufferMgr::GetInstance()->GetTxBuf();
        PVCI_CAN_OBJ frame;

        while (m_running)
        {
            // 非阻塞取出一条待发报文，取出后缓冲自动移除本条数据
            if (txBuf.popNoBlock(frame))
            {
                // 调用底层硬件发送
                m_canObj->slotTransmitCAN(
                    m_canObj->mDevTypeno,
                    m_canObj->mDevIndexno,
                    m_canObj->mDevCANIndex,
                    frame,
                    1
                );
            }
            QThread::usleep(10); // 微秒高精度轮询
        }
        qDebug() << "CAN发送轮询线程退出";
    };
    pollTask();
}

LCANBusFXYNEW::LCANBusFXYNEW(QObject *parent) : QObject(parent), isOpen(false), isConn(false), isStart(false),
    isReset(false), mDevMode(0), mDevTypeno(4), mDevIndexno(0), mDevCANIndex(0)
{
    m_sendPollThread.SetCanObj(this);
}

LCANBusFXYNEW::~LCANBusFXYNEW()
{
    StopSendPollThread();
    CanBufferMgr::GetInstance()->ClearAll();
}

void LCANBusFXYNEW::StartSendPollThread()
{
    m_sendPollThread.StartPoll();
}

void LCANBusFXYNEW::StopSendPollThread()
{
    m_sendPollThread.StopPoll();
}

CANFXYNEWRETURNCODE LCANBusFXYNEW::onConnect(int m_devCANIndex, int port)
{
    if (!isOpen)
    {
        isConn = false;
        return FXYNEWSTARTBEFOREOPEN;
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
            return FXYNEWINITIALCANFAILED;
        }
    }
    else
    {
        if(VCI_SetReference(mDevTypeno, mDevIndexno, mDevCANIndex, 0, &(mTiming)) != STATUS_OK)
        {
            VCI_CloseDevice(mDevTypeno, mDevIndexno);
            isOpen = false;
            isConn = false;
            return FXYNEWINITIALCANFAILED;
        }

        if(VCI_InitCAN(mDevTypeno, mDevIndexno, mDevCANIndex, &vic) != STATUS_OK)
        {
            VCI_CloseDevice(mDevTypeno, mDevIndexno);
            isOpen = false;
            isConn = false;
            return FXYNEWINITIALCANFAILED;
        }
    }
    StartSendPollThread();
    isConn = true;
    return FXYNEWCONNECTCANSUCCESS;
}

int LCANBusFXYNEW::charToInt(unsigned char chr, unsigned char *cint)
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

int LCANBusFXYNEW::strToData(unsigned char *str, unsigned char *data,int len,int flag)
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

void LCANBusFXYNEW::unsigned_to_hex(unsigned int value, std::string &hex_string)
{
    std::strstream buffer;
    buffer.setf(std::ios::showbase);
    buffer <<std::hex << value;
    buffer >> hex_string;
}

void LCANBusFXYNEW::slotOpenCAN(int m_devtypeno, int m_devindexno)
{
    mDevTypeno = m_devtypeno;
    mDevIndexno = m_devindexno;
    DWORD ret = VCI_OpenDevice(m_devtypeno, m_devindexno, 0);
    if(ret != STATUS_OK)
    {
        isOpen = false;
        emit signalCANStatus(FXYNEWOPENCANFAILED);
    }
    else
    {
        isOpen = true;
        emit signalCANStatus(FXYNEWOPENCANSUCCESS);
    }
}

void LCANBusFXYNEW::slotCloseCAN(int m_devtypeno, int m_devindexno)
{
    StopSendPollThread();
    mDevTypeno = m_devtypeno;
    mDevIndexno = m_devindexno;
    if(isOpen)
    {
        DWORD ret = VCI_CloseDevice(m_devtypeno, m_devindexno);
        if(ret != STATUS_OK)
        {
            emit signalCANStatus(FXYNEWCLOSECANFAILED);
        }
        {
            isOpen = false;
            emit signalCANStatus(FXYNEWCLOSECANSUCCESS);
        }
    }
    else
    {
        emit signalCANStatus(FXYNEWCLOSEBEFOREOPEN);
    }
}

void LCANBusFXYNEW::slotStartCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex, int port)
{
    mDevTypeno = m_devtypeno;
    mDevIndexno = m_devindexno;
    mDevCANIndex = m_devCANIndex;
    m_port = port;
    qDebug() << "mDevTypeno is "  << m_devtypeno <<
                " mDevIndexno is " <<  m_devindexno <<
                " mDevCANIndex is " <<  m_devCANIndex;
    CANFXYNEWRETURNCODE temp = onConnect(m_devCANIndex, m_port);
    if(FXYNEWCONNECTCANSUCCESS != temp)
    {
        emit signalCANStatus(temp);
    }
    if(!isConn)
    {
        emit signalCANStatus(FXYNEWCONNECTCANFILED);
    }
    if(VCI_StartCAN(m_devtypeno, m_devindexno, m_devCANIndex) == STATUS_OK)
    {
        isStart = true;
        isReset = false;
        emit signalCANStatus(FXYNEWSTARTCANSUCCESS);
    }
    else
    {
        isStart = false;
        emit signalCANStatus(FXYNEWSTARTCANFAILED);
    }
}

void LCANBusFXYNEW::slotResetCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex)
{
    mDevTypeno = m_devtypeno;
    mDevIndexno = m_devindexno;
    mDevCANIndex = m_devCANIndex;
    if (!isOpen)
    {
        emit signalCANStatus(FXYNEWRESETBEFOREOPEN);
    }

    if(!isConn)
    {
        emit signalCANStatus(FXYNEWRESETBEFORECONNECT);
    }

    if(VCI_ResetCAN(m_devtypeno, m_devindexno, m_devCANIndex) == STATUS_OK)
    {
        isReset = true;
        isStart = false;
        emit signalCANStatus(FXYNEWRESETCANSUCCESS);
    }
    else
    {
        isReset = false;
        emit signalCANStatus(FXYNEWRESETCANFAILED);
    }
}

void LCANBusFXYNEW::slotReceiveCAN()
{
    if(!isOpen)
    {
        emit signalCANStatus(FXYNEWREADBEFOREOPEN);
        return;
    }
    if(!isConn)
    {
        emit signalCANStatus(FXYNEWREADBEFORECONN);
        return;
    }
    if(!isStart)
    {
        emit signalCANStatus(FXYNEWREADBEFORESTART);
        return;
    }
    VCI_CAN_OBJ frameinfo[2500];
    memset(frameinfo, 0, sizeof(frameinfo));
    int framelen = VCI_Receive(mDevTypeno, mDevIndexno, mDevCANIndex, frameinfo, 2500, 0);
    if(0xFFFFFFFF != framelen && framelen >= 0)
    {
        for(int i = 0; i < framelen; i++)
        {
            // 存入全局共享接收缓冲区
            CanBufferMgr::GetInstance()->GetRxBuf().push(&frameinfo[i]);
        }
        emit signalCANStatus(FXYNEWREADCANSUCCESS);
    }
}

void LCANBusFXYNEW::slotTransmitCAN(int m_devtypeno, int m_devindexno, int m_devCANIndex, PVCI_CAN_OBJ pSend, ULONG Len)
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
    VCI_Transmit(mDevTypeno, mDevIndexno, mDevCANIndex, pSend, Len);
}

void LCANBusFXYNEW::slotChangePort()
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
