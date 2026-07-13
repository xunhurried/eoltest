#include "canbuscontrol.h"
#include "model/ping.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QSettings>
#include <QString>
#include <QThread>
#include <QTimer>
#include <string>
#include <strstream>
#include <vector>
#include <cmath>


CanRxParseThread::~CanRxParseThread()
{
    StopParse();
}

void CanRxParseThread::StartParse()
{
    m_running = true;
    start();
}

void CanRxParseThread::StopParse()
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

void CanRxParseThread::run()
{
    // lambda 循环读取共享Rx缓冲区
    auto parseLoop = [this]()
    {
        CanRingBuffer& rxBuf = CanBufferMgr::GetInstance()->GetRxBuf();
        PVCI_CAN_OBJ frame;

        while (m_running)
        {
            // 阻塞读取：无数据自动休眠，降低CPU占用
            if (rxBuf.popBlock(frame))
            {
                // 执行业务解析
                emit sigNewRxFrame(frame);
            }
        }
        qDebug() << "CAN接收解析线程退出";
    };

    parseLoop();
}

CanBusControl::CanBusControl(int type, int id, int pass, int port)
    : canType(type), deviceType(0), m_locate(id), m_pass(pass), m_port(port),
      canBusFO1(NULL), canBusFN1(NULL), canBusZLG1(NULL),
      canBusGC1(NULL)
{
    can1 = NULL;
    can2 = NULL;
    can3 = NULL;
    can4 = NULL;
    can5 = NULL;
    can6 = NULL;
    can7 = NULL;
    can8 = NULL;
    can9 = NULL;
    can10 = NULL;
    can11 = NULL;
    can12 = NULL;
    can13 = NULL;
    can14 = NULL;
    can15 = NULL;
    can16 = NULL;
    canBusFO1   = new LCANBusFXYOLD();
    canBusFN1   = new LCANBusFXYNEW();
    canBusZLG1  = new LCANBusZLG();
    canBusGC1   = new LCANBusGC();
    canBusFO2 = new LCANBusFXYOLD();
    canBusFN2 = new LCANBusFXYNEW();
    canBusZLG2 = new LCANBusZLG();
    canBusGC2 = new LCANBusGC();
    switch(type)
    {
    case 0:       //老版分析仪
    {
        connect(this, SIGNAL(signalOpenCAN(int, int)), canBusFO1, SLOT(slotOpenCAN(int,int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalCloseCAN(int, int)), canBusFO1, SLOT(slotCloseCAN(int,int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalStartCAN(int, int, int, int)), canBusFO1, SLOT(slotStartCAN(int, int, int, int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalResetCAN(int, int, int)), canBusFO1, SLOT(slotResetCAN(int, int, int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalOpenCAN2(int, int)), canBusFO2, SLOT(slotOpenCAN(int,int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalCloseCAN2(int, int)), canBusFO2, SLOT(slotCloseCAN(int,int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalStartCAN2(int, int, int, int)), canBusFO2, SLOT(slotStartCAN(int, int, int, int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalResetCAN2(int, int, int)), canBusFO2, SLOT(slotResetCAN(int, int, int)), Qt::DirectConnection);
        deviceType = 4;
        break;
    }
    case 1:       //新版分析仪
    {
        connect(this, SIGNAL(signalOpenCAN(int, int)), canBusFN1, SLOT(slotOpenCAN(int,int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalCloseCAN(int, int)), canBusFN1, SLOT(slotCloseCAN(int,int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalStartCAN(int, int, int, int)), canBusFN1, SLOT(slotStartCAN(int, int, int, int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalResetCAN(int, int, int)), canBusFN1, SLOT(slotResetCAN(int, int, int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalOpenCAN2(int, int)), canBusFN2, SLOT(slotOpenCAN(int   ,int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalCloseCAN2(int, int)), canBusFN2, SLOT(slotCloseCAN(int,int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalStartCAN2(int, int, int, int)), canBusFN2, SLOT(slotStartCAN(int, int, int, int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalResetCAN2(int, int, int)), canBusFN2, SLOT(slotResetCAN(int, int, int)), Qt::DirectConnection);
        deviceType = 4;
        break;
    }
    case 2:       //周立功CAN卡
    {
        connect(this, SIGNAL(signalOpenCAN(int, int)), canBusZLG1, SLOT(slotOpenCAN(int,int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalCloseCAN(int, int)), canBusZLG1, SLOT(slotCloseCAN(int,int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalStartCAN(int, int, int, int)), canBusZLG1, SLOT(slotStartCAN(int, int, int, int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalResetCAN(int, int, int)), canBusZLG1, SLOT(slotResetCAN(int, int, int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalTransmitCAN(int, int, int, PVCI_CAN_OBJ, ULONG)),
                canBusZLG1, SLOT(slotTransmitCAN(int, int, int, PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
        connect(canBusZLG1, SIGNAL(signalCANStatus(CANZLGRETURNCODE)),
                this, SLOT(slotCANStatus(CANZLGRETURNCODE)), Qt::DirectConnection);
        connect(canBusZLG1, SIGNAL(signalReceiveData(VCI_CAN_OBJ)),
                this, SLOT(slotReceiveData(VCI_CAN_OBJ)), Qt::DirectConnection);
        break;
    }
    case 3:       //广成CAN卡
    {
        connect(this, SIGNAL(signalOpenCAN(int, int)), canBusGC1, SLOT(slotOpenCAN(int,int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalCloseCAN(int, int)), canBusGC1, SLOT(slotCloseCAN(int,int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalStartCAN(int, int, int, int)), canBusGC1, SLOT(slotStartCAN(int, int, int, int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalResetCAN(int, int, int)), canBusGC1, SLOT(slotResetCAN(int, int, int)), Qt::DirectConnection);
        connect(this, SIGNAL(signalTransmitCAN(int, int, int, CAN_OBJ, ULONG)),
                canBusGC1, SLOT(slotTransmitCAN(int, int, int, CAN_OBJ, ULONG)), Qt::DirectConnection);
        connect(canBusGC1, SIGNAL(signalCANStatus(CANGCRETURNCODE)),
                this, SLOT(slotCANStatus(CANGCRETURNCODE)), Qt::DirectConnection);
        connect(canBusGC1, SIGNAL(signalReceiveData(CAN_OBJ)),
                this, SLOT(slotReceiveData(CAN_OBJ)), Qt::DirectConnection);
        break;
    }
    default:
        break;
    }
}

CanBusControl::~CanBusControl()
{
    emit signalCloseCAN(deviceType, 0);
    emit signalCloseCAN2(deviceType, 0);
}

void CanBusControl::testConn()
{
    switch(canType)
    {
    case 0:       //老版分析仪
    {
        emit signalOpenCAN(4, m_locate);
        emit signalStartCAN(4, m_locate, 0, m_port);
        emit signalOpenCAN2(4, m_locate);
        emit signalStartCAN2(4, m_locate, 1, m_port);
        break;
    }
    case 1:       //新版分析仪
    {
        emit signalOpenCAN(4, m_locate);
        emit signalStartCAN(4, m_locate, 0, m_port);
        emit signalOpenCAN2(4, m_locate);
        emit signalStartCAN2(4, m_locate, 1, m_port);
        break;
    }
    case 2:       //周立功CAN卡
    {
        emit signalChooseZLGDeviceType();
        emit signalStartCAN(4, m_locate, m_pass, m_port);
        break;
    }
    case 3:       //广成CAN卡
    {
        emit signalChooseGCDeviceType();
        emit signalStartCAN(4, m_locate, m_pass, m_port);
        break;
    }
    default:
        break;
    }
    g_rxParseThread.StartParse();

    // 绑定UI刷新信号（主线程接收）
    connect(&g_rxParseThread, SIGNAL(sigNewRxFrame()), this, SLOT(slotReceiveData()));
    for(int i = 1; i < 17; i ++)
    {
        unsigned char data[8];
        data[0]      = 3;
        data[1]      = 0;
        data[2]      = 0;
        data[3]      = 0;
        data[4]      = 0;
        data[5]      = 0;
        data[6]      = 0;
        data[7]      = 0;
        TransmitCAN(((0x1857 << 8) | (i & 0xFF)) << 8 | 0xF4, data);
    }
    testTimer = new QTimer(this);
    connect(testTimer, SIGNAL(timeout()), this, SLOT(slotCheckStatus()));
    testTimer->setSingleShot(false);
    testTimer->start(5000);
}

bool CanBusControl::isFinish(int status)
{
    return true;
}

int CanBusControl::currentTestStep() { return 1;}

void CanBusControl::setType(int type1, int type2, int type3, int type4, int type5, int type6, int type7, int type8, int type9
                            , int type10, int type11, int type12, int type13, int type14, int type15, int type16)
{
    m_type1 = type1;
    m_type2 = type2;
    m_type3 = type3;
    m_type4 = type4;
    m_type5 = type5;
    m_type6 = type6;
    m_type7 = type7;
    m_type8 = type8;
    m_type9 = type9;
    m_type10 = type10;
    m_type11 = type11;
    m_type12 = type12;
    m_type13 = type13;
    m_type14 = type14;
    m_type15 = type15;
    m_type16 = type16;

    can1 = new CANGetAndSend(m_type1, 1);
    can2 = new CANGetAndSend(m_type2, 2);
    can3 = new CANGetAndSend(m_type3, 3);
    can4 = new CANGetAndSend(m_type4, 4);
    can5 = new CANGetAndSend(m_type5, 5);
    can6 = new CANGetAndSend(m_type6, 6);
    can7 = new CANGetAndSend(m_type7, 7);
    can8 = new CANGetAndSend(m_type8, 8);
    can9 = new CANGetAndSend(m_type9, 9);
    can10 = new CANGetAndSend(m_type10, 10);
    can11 = new CANGetAndSend(m_type11, 11);
    can12 = new CANGetAndSend(m_type12, 12);
    can13 = new CANGetAndSend(m_type13, 13);
    can14 = new CANGetAndSend(m_type14, 14);
    can15 = new CANGetAndSend(m_type15, 15);
    can16 = new CANGetAndSend(m_type16, 16);

    connect(can1, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet1()));
    connect(can2, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet2()));
    connect(can3, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet3()));
    connect(can4, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet4()));
    connect(can5, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet5()));
    connect(can6, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet6()));
    connect(can7, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet7()));
    connect(can8, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet8()));
    connect(can9, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet9()));
    connect(can10, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet10()));
    connect(can11, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet11()));
    connect(can12, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet12()));
    connect(can13, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet13()));
    connect(can14, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet14()));
    connect(can15, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet15()));
    connect(can16, SIGNAL(signalFinishSet()), this, SLOT(slotFinishSet16()));

    connect(can1, SIGNAL(signalTimeout()), this, SLOT(slotTimeout1()));
    connect(can2, SIGNAL(signalTimeout()), this, SLOT(slotTimeout2()));
    connect(can3, SIGNAL(signalTimeout()), this, SLOT(slotTimeout3()));
    connect(can4, SIGNAL(signalTimeout()), this, SLOT(slotTimeout4()));
    connect(can5, SIGNAL(signalTimeout()), this, SLOT(slotTimeout5()));
    connect(can6, SIGNAL(signalTimeout()), this, SLOT(slotTimeout6()));
    connect(can7, SIGNAL(signalTimeout()), this, SLOT(slotTimeout7()));
    connect(can8, SIGNAL(signalTimeout()), this, SLOT(slotTimeout8()));
    connect(can9, SIGNAL(signalTimeout()), this, SLOT(slotTimeout9()));
    connect(can10, SIGNAL(signalTimeout()), this, SLOT(slotTimeout10()));
    connect(can11, SIGNAL(signalTimeout()), this, SLOT(slotTimeout11()));
    connect(can12, SIGNAL(signalTimeout()), this, SLOT(slotTimeout12()));
    connect(can13, SIGNAL(signalTimeout()), this, SLOT(slotTimeout13()));
    connect(can14, SIGNAL(signalTimeout()), this, SLOT(slotTimeout14()));
    connect(can15, SIGNAL(signalTimeout()), this, SLOT(slotTimeout15()));
    connect(can16, SIGNAL(signalTimeout()), this, SLOT(slotTimeout16()));

    connect(can1, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet21()));
    connect(can2, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet22()));
    connect(can3, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet23()));
    connect(can4, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet24()));
    connect(can5, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet25()));
    connect(can6, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet26()));
    connect(can7, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet27()));
    connect(can8, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet28()));
    connect(can9, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet29()));
    connect(can10, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet210()));
    connect(can11, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet211()));
    connect(can12, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet212()));
    connect(can13, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet213()));
    connect(can14, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet214()));
    connect(can15, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet215()));
    connect(can16, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet216()));

    connect(can1, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail1()));
    connect(can2, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail2()));
    connect(can3, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail3()));
    connect(can4, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail4()));
    connect(can5, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail5()));
    connect(can6, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail6()));
    connect(can7, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail7()));
    connect(can8, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail8()));
    connect(can9, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail9()));
    connect(can10, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail10()));
    connect(can11, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail11()));
    connect(can12, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail12()));
    connect(can13, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail13()));
    connect(can14, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail14()));
    connect(can15, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail15()));
    connect(can16, SIGNAL(signalSetModFail()), this, SLOT(slotSetModFail16()));

    connect(can1, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail21()));
    connect(can2, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail22()));
    connect(can3, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail23()));
    connect(can4, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail24()));
    connect(can5, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail25()));
    connect(can6, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail26()));
    connect(can7, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail27()));
    connect(can8, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail28()));
    connect(can9, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail29()));
    connect(can10, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail210()));
    connect(can11, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail211()));
    connect(can12, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail212()));
    connect(can13, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail213()));
    connect(can14, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail214()));
    connect(can15, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail215()));
    connect(can16, SIGNAL(signalSetModFail2()), this, SLOT(slotSetModFail216()));

    connect(can1, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus1(const QString &)));
    connect(can2, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus2(const QString &)));
    connect(can3, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus3(const QString &)));
    connect(can4, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus4(const QString &)));
    connect(can5, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus5(const QString &)));
    connect(can6, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus6(const QString &)));
    connect(can7, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus7(const QString &)));
    connect(can8, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus8(const QString &)));
    connect(can9, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus9(const QString &)));
    connect(can10, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus10(const QString &)));
    connect(can11, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus11(const QString &)));
    connect(can12, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus12(const QString &)));
    connect(can13, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus13(const QString &)));
    connect(can14, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus14(const QString &)));
    connect(can15, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus15(const QString &)));
    connect(can16, SIGNAL(signalReceiveStatus(const QString &)), this, SLOT(slotReceiveStatus16(const QString &)));

    connect(can1, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can2, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can3, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can4, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can5, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can6, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can7, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can8, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can9, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can10, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can11, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can12, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can13, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can14, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can15, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can16, SIGNAL(signalTransmitCAN(PVCI_CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(PVCI_CAN_OBJ, ULONG)), Qt::DirectConnection);

    connect(can1, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can2, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can3, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can4, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can5, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can6, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can7, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can8, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can9, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can10, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can11, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can12, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can13, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can14, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can15, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);
    connect(can16, SIGNAL(signalTransmitCAN(CAN_OBJ, ULONG)), this, SLOT(slotTransmitCAN(CAN_OBJ, ULONG)), Qt::DirectConnection);

    connect(can1, SIGNAL(signalFinish()), this, SLOT(slotFinish1()));
    connect(can2, SIGNAL(signalFinish()), this, SLOT(slotFinish2()));
    connect(can3, SIGNAL(signalFinish()), this, SLOT(slotFinish3()));
    connect(can4, SIGNAL(signalFinish()), this, SLOT(slotFinish4()));
    connect(can5, SIGNAL(signalFinish()), this, SLOT(slotFinish5()));
    connect(can6, SIGNAL(signalFinish()), this, SLOT(slotFinish6()));
    connect(can7, SIGNAL(signalFinish()), this, SLOT(slotFinish7()));
    connect(can8, SIGNAL(signalFinish()), this, SLOT(slotFinish8()));
    connect(can9, SIGNAL(signalFinish()), this, SLOT(slotFinish9()));
    connect(can10, SIGNAL(signalFinish()), this, SLOT(slotFinish10()));
    connect(can11, SIGNAL(signalFinish()), this, SLOT(slotFinish11()));
    connect(can12, SIGNAL(signalFinish()), this, SLOT(slotFinish12()));
    connect(can13, SIGNAL(signalFinish()), this, SLOT(slotFinish13()));
    connect(can14, SIGNAL(signalFinish()), this, SLOT(slotFinish14()));
    connect(can15, SIGNAL(signalFinish()), this, SLOT(slotFinish15()));
    connect(can16, SIGNAL(signalFinish()), this, SLOT(slotFinish16()));

    connect(can1, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish1()));
    connect(can2, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish2()));
    connect(can3, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish3()));
    connect(can4, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish4()));
    connect(can5, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish5()));
    connect(can6, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish6()));
    connect(can7, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish7()));
    connect(can8, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish8()));
    connect(can9, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish9()));
    connect(can10, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish10()));
    connect(can11, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish11()));
    connect(can12, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish12()));
    connect(can13, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish13()));
    connect(can14, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish14()));
    connect(can15, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish15()));
    connect(can16, SIGNAL(signalErrorFinish()), this, SLOT(slotErrorFinish16()));

    connect(can1, SIGNAL(signalSerialError()), this, SLOT(slotSerialError1()));
    connect(can2, SIGNAL(signalSerialError()), this, SLOT(slotSerialError2()));
    connect(can3, SIGNAL(signalSerialError()), this, SLOT(slotSerialError3()));
    connect(can4, SIGNAL(signalSerialError()), this, SLOT(slotSerialError4()));
    connect(can5, SIGNAL(signalSerialError()), this, SLOT(slotSerialError5()));
    connect(can6, SIGNAL(signalSerialError()), this, SLOT(slotSerialError6()));
    connect(can7, SIGNAL(signalSerialError()), this, SLOT(slotSerialError7()));
    connect(can8, SIGNAL(signalSerialError()), this, SLOT(slotSerialError8()));
    connect(can9, SIGNAL(signalSerialError()), this, SLOT(slotSerialError9()));
    connect(can10, SIGNAL(signalSerialError()), this, SLOT(slotSerialError10()));
    connect(can11, SIGNAL(signalSerialError()), this, SLOT(slotSerialError11()));
    connect(can12, SIGNAL(signalSerialError()), this, SLOT(slotSerialError12()));
    connect(can13, SIGNAL(signalSerialError()), this, SLOT(slotSerialError13()));
    connect(can14, SIGNAL(signalSerialError()), this, SLOT(slotSerialError14()));
    connect(can15, SIGNAL(signalSerialError()), this, SLOT(slotSerialError15()));
    connect(can16, SIGNAL(signalSerialError()), this, SLOT(slotSerialError16()));

    connect(can1, SIGNAL(signalStartTest()), this, SLOT(slotStartTest1()));
    connect(can2, SIGNAL(signalStartTest()), this, SLOT(slotStartTest2()));
    connect(can3, SIGNAL(signalStartTest()), this, SLOT(slotStartTest3()));
    connect(can4, SIGNAL(signalStartTest()), this, SLOT(slotStartTest4()));
    connect(can5, SIGNAL(signalStartTest()), this, SLOT(slotStartTest5()));
    connect(can6, SIGNAL(signalStartTest()), this, SLOT(slotStartTest6()));
    connect(can7, SIGNAL(signalStartTest()), this, SLOT(slotStartTest7()));
    connect(can8, SIGNAL(signalStartTest()), this, SLOT(slotStartTest8()));
    connect(can9, SIGNAL(signalStartTest()), this, SLOT(slotStartTest9()));
    connect(can10, SIGNAL(signalStartTest()), this, SLOT(slotStartTest10()));
    connect(can11, SIGNAL(signalStartTest()), this, SLOT(slotStartTest11()));
    connect(can12, SIGNAL(signalStartTest()), this, SLOT(slotStartTest12()));
    connect(can13, SIGNAL(signalStartTest()), this, SLOT(slotStartTest13()));
    connect(can14, SIGNAL(signalStartTest()), this, SLOT(slotStartTest14()));
    connect(can15, SIGNAL(signalStartTest()), this, SLOT(slotStartTest15()));
    connect(can16, SIGNAL(signalStartTest()), this, SLOT(slotStartTest16()));

    connect(can1, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial1(QString)));
    connect(can2, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial2(QString)));
    connect(can3, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial3(QString)));
    connect(can4, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial4(QString)));
    connect(can5, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial5(QString)));
    connect(can6, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial6(QString)));
    connect(can7, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial7(QString)));
    connect(can8, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial8(QString)));
    connect(can9, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial9(QString)));
    connect(can10, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial10(QString)));
    connect(can11, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial11(QString)));
    connect(can12, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial12(QString)));
    connect(can13, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial13(QString)));
    connect(can14, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial14(QString)));
    connect(can15, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial15(QString)));
    connect(can16, SIGNAL(signalSetSerial(QString)), this, SLOT(slotSetSerial16(QString)));

    connect(can1, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC1(QString)));
    connect(can2, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC2(QString)));
    connect(can3, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC3(QString)));
    connect(can4, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC4(QString)));
    connect(can5, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC5(QString)));
    connect(can6, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC6(QString)));
    connect(can7, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC7(QString)));
    connect(can8, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC8(QString)));
    connect(can9, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC9(QString)));
    connect(can10, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC10(QString)));
    connect(can11, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC11(QString)));
    connect(can12, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC12(QString)));
    connect(can13, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC13(QString)));
    connect(can14, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC14(QString)));
    connect(can15, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC15(QString)));
    connect(can16, SIGNAL(signalSetMAC(QString)), this, SLOT(slotSetMAC16(QString)));

    connect(&m_rs485Parse, SIGNAL(signalSend485Data1(QByteArray)), can1, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data2(QByteArray)), can2, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data3(QByteArray)), can3, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data4(QByteArray)), can4, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data5(QByteArray)), can5, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data6(QByteArray)), can6, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data7(QByteArray)), can7, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data8(QByteArray)), can8, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data9(QByteArray)), can9, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data10(QByteArray)), can10, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data11(QByteArray)), can11, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data12(QByteArray)), can12, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data13(QByteArray)), can13, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data14(QByteArray)), can14, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data15(QByteArray)), can15, SLOT(slotReceive485Data(QByteArray)));
    connect(&m_rs485Parse, SIGNAL(signalSend485Data16(QByteArray)), can16, SLOT(slotReceive485Data(QByteArray)));

    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can1, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can2, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can3, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can4, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can5, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can6, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can7, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can8, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can9, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can10, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can11, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can12, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can13, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can14, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can15, SLOT(slotReceiveData(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(VCI_CAN_OBJ)), can16, SLOT(slotReceiveData(VCI_CAN_OBJ)));

    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can1, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can2, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can3, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can4, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can5, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can6, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can7, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can8, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can9, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can10, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can11, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can12, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can13, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can14, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can15, SLOT(slotReceiveData2(VCI_CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(VCI_CAN_OBJ)), can16, SLOT(slotReceiveData2(VCI_CAN_OBJ)));

    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can1, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can2, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can3, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can4, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can5, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can6, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can7, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can8, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can9, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can10, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can11, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can12, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can13, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can14, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can15, SLOT(slotReceiveData(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData(CAN_OBJ)), can16, SLOT(slotReceiveData(CAN_OBJ)));

    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can1, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can2, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can3, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can4, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can5, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can6, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can7, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can8, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can9, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can10, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can11, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can12, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can13, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can14, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can15, SLOT(slotReceiveData2(CAN_OBJ)));
    connect(this, SIGNAL(signalReceiveData2(CAN_OBJ)), can16, SLOT(slotReceiveData2(CAN_OBJ)));

    connect(this, SIGNAL(signalReceiveSerial1(QString)), can1, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial2(QString)), can2, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial3(QString)), can3, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial4(QString)), can4, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial5(QString)), can5, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial6(QString)), can6, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial7(QString)), can7, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial8(QString)), can8, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial9(QString)), can9, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial10(QString)), can10, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial11(QString)), can11, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial12(QString)), can12, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial13(QString)), can13, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial14(QString)), can14, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial15(QString)), can15, SLOT(slotReceiveSerial(QString)));
    connect(this, SIGNAL(signalReceiveSerial16(QString)), can16, SLOT(slotReceiveSerial(QString)));


    connect(this, SIGNAL(signalSetSoft1(QString)), can1, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft2(QString)), can2, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft3(QString)), can3, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft4(QString)), can4, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft5(QString)), can5, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft6(QString)), can6, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft7(QString)), can7, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft8(QString)), can8, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft9(QString)), can9, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft10(QString)), can10, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft11(QString)), can11, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft12(QString)), can12, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft13(QString)), can13, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft14(QString)), can14, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft15(QString)), can15, SLOT(slotSetSoft(QString)));
    connect(this, SIGNAL(signalSetSoft16(QString)), can16, SLOT(slotSetSoft(QString)));

    connect(this, SIGNAL(signalSetHard1(QString)), can1, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard2(QString)), can2, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard3(QString)), can3, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard4(QString)), can4, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard5(QString)), can5, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard6(QString)), can6, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard7(QString)), can7, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard8(QString)), can8, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard9(QString)), can9, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard10(QString)), can10, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard11(QString)), can11, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard12(QString)), can12, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard13(QString)), can13, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard14(QString)), can14, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard15(QString)), can15, SLOT(slotSetHard(QString)));
    connect(this, SIGNAL(signalSetHard16(QString)), can16, SLOT(slotSetHard(QString)));

    connect(this, SIGNAL(signalSetCurrentTestStep1(int)),
               can1, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep2(int)),
               can2, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep3(int)),
               can3, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep4(int)),
               can4, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep5(int)),
               can5, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep6(int)),
               can6, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep7(int)),
               can7, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep8(int)),
               can8, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep9(int)),
               can9, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep10(int)),
               can10, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep11(int)),
               can11, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep12(int)),
               can12, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep13(int)),
               can13, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep14(int)),
               can14, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep15(int)),
               can15, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSetCurrentTestStep16(int)),
               can16, SLOT(slotSetCurrentTestStep(int)), Qt::DirectConnection);

    connect(can1, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem1(std::vector<ExcelItem>)));
    connect(can2, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem2(std::vector<ExcelItem>)));
    connect(can3, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem3(std::vector<ExcelItem>)));
    connect(can4, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem4(std::vector<ExcelItem>)));
    connect(can5, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem5(std::vector<ExcelItem>)));
    connect(can6, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem6(std::vector<ExcelItem>)));
    connect(can7, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem7(std::vector<ExcelItem>)));
    connect(can8, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem8(std::vector<ExcelItem>)));
    connect(can9, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem9(std::vector<ExcelItem>)));
    connect(can10, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem10(std::vector<ExcelItem>)));
    connect(can11, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem11(std::vector<ExcelItem>)));
    connect(can12, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem12(std::vector<ExcelItem>)));
    connect(can13, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem13(std::vector<ExcelItem>)));
    connect(can14, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem14(std::vector<ExcelItem>)));
    connect(can15, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem15(std::vector<ExcelItem>)));
    connect(can16, SIGNAL(signalSetExcelItem(std::vector<ExcelItem>)), this, SLOT(slotSetExcelItem16(std::vector<ExcelItem>)));


    connect(can1, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult1(QJsonObject)));
    connect(can2, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult2(QJsonObject)));
    connect(can3, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult3(QJsonObject)));
    connect(can4, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult4(QJsonObject)));
    connect(can5, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult5(QJsonObject)));
    connect(can6, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult6(QJsonObject)));
    connect(can7, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult7(QJsonObject)));
    connect(can8, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult8(QJsonObject)));
    connect(can9, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult9(QJsonObject)));
    connect(can10, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult10(QJsonObject)));
    connect(can11, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult11(QJsonObject)));
    connect(can12, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult12(QJsonObject)));
    connect(can13, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult13(QJsonObject)));
    connect(can14, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult14(QJsonObject)));
    connect(can15, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult15(QJsonObject)));
    connect(can16, SIGNAL(signalSendTestResult(QJsonObject)), this, SLOT(slotSendTestResult16(QJsonObject)));

    connect(this, SIGNAL(signalSetTestMod1()), can1, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod2()), can2, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod3()), can3, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod4()), can4, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod5()), can5, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod6()), can6, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod7()), can7, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod8()), can8, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod9()), can9, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod10()), can10, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod11()), can11, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod12()), can12, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod13()), can13, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod14()), can14, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod15()), can15, SLOT(slotSetTestMod()));
    connect(this, SIGNAL(signalSetTestMod16()), can16, SLOT(slotSetTestMod()));

    connect(can1, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate1()));
    connect(can2, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate2()));
    connect(can3, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate3()));
    connect(can4, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate4()));
    connect(can5, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate5()));
    connect(can6, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate6()));
    connect(can7, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate7()));
    connect(can8, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate8()));
    connect(can9, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate9()));
    connect(can10, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate10()));
    connect(can11, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate11()));
    connect(can12, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate12()));
    connect(can13, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate13()));
    connect(can14, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate14()));
    connect(can15, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate15()));
    connect(can16, SIGNAL(signalCheckIfCanUpdate()), this, SLOT(slotCheckIfCanUpdate16()));

    connect(can1, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus1(bool)));
    connect(can2, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus2(bool)));
    connect(can3, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus3(bool)));
    connect(can4, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus4(bool)));
    connect(can5, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus5(bool)));
    connect(can6, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus6(bool)));
    connect(can7, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus7(bool)));
    connect(can8, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus8(bool)));
    connect(can9, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus9(bool)));
    connect(can10, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus10(bool)));
    connect(can11, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus11(bool)));
    connect(can12, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus12(bool)));
    connect(can13, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus13(bool)));
    connect(can14, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus14(bool)));
    connect(can15, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus15(bool)));
    connect(can16, SIGNAL(signalSetUpdateStatus(bool)), this, SLOT(slotSetUpdateStatus16(bool)));

    connect(this, SIGNAL(signalIfCanUpdate1To4(bool)), can1, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate1To4(bool)), can2, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate1To4(bool)), can3, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate1To4(bool)), can4, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate5To8(bool)), can5, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate5To8(bool)), can6, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate5To8(bool)), can7, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate5To8(bool)), can8, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate9To12(bool)), can9, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate9To12(bool)), can10, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate9To12(bool)), can11, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate9To12(bool)), can12, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate13To16(bool)), can13, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate13To16(bool)), can14, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate13To16(bool)), can15, SLOT(slotIfCanUpdate(bool)));
    connect(this, SIGNAL(signalIfCanUpdate13To16(bool)), can16, SLOT(slotIfCanUpdate(bool)));
    m_rs485Parse.Start();
}

void CanBusControl::testJson()
{
    QJsonObject test;
    //emit signalSendTestResult(test);
}

void CanBusControl::TransmitCAN(int ID, unsigned char data[])
{
    VCI_CAN_OBJ vco[1];
    CAN_OBJ co;
    vco[0].ID         = ID;
    vco[0].RemoteFlag = 0;
    vco[0].ExternFlag = 1;
    vco[0].DataLen    = 8;
    co.DataLen        = 8;
    co.ID             = ID;
    co.RemoteFlag     = 0;
    co.ExternFlag     = 1;
    co.DataLen        = 8;
    for (int i = 0; i < 8; i++)
    {
        vco[0].Data[i] = data[i];
        co.Data[i]     = data[i];
    }
    bool ret = CanBufferMgr::GetInstance()->GetTxBuf().push(vco);
    if (!ret)
    {
        qDebug() << "缓冲区已满，CAN发送失败！";
    }
}

void CanBusControl::unsigned_to_hex(unsigned int value, std::string &hex_string)
{
    std::strstream buffer;
    buffer.setf(std::ios::showbase);
    buffer << std::hex << value;
    buffer >> hex_string;
}

void CanBusControl::slotSetCurrentTestStep1(int type)
{
    emit signalSetCurrentTestStep1(type);
}

void CanBusControl::slotSetCurrentTestStep2(int type)
{
    emit signalSetCurrentTestStep2(type);
}

void CanBusControl::slotSetCurrentTestStep3(int type)
{
    emit signalSetCurrentTestStep3(type);
}

void CanBusControl::slotSetCurrentTestStep4(int type)
{
    emit signalSetCurrentTestStep4(type);
}

void CanBusControl::slotSetCurrentTestStep5(int type)
{
    emit signalSetCurrentTestStep5(type);
}

void CanBusControl::slotSetCurrentTestStep6(int type)
{
    emit signalSetCurrentTestStep6(type);
}

void CanBusControl::slotSetCurrentTestStep7(int type)
{
    emit signalSetCurrentTestStep7(type);
}

void CanBusControl::slotSetCurrentTestStep8(int type)
{
    emit signalSetCurrentTestStep8(type);
}

void CanBusControl::slotSetCurrentTestStep9(int type)
{
    emit signalSetCurrentTestStep9(type);
}

void CanBusControl::slotSetCurrentTestStep10(int type)
{
    emit signalSetCurrentTestStep10(type);
}

void CanBusControl::slotSetCurrentTestStep11(int type)
{
    emit signalSetCurrentTestStep11(type);
}

void CanBusControl::slotSetCurrentTestStep12(int type)
{
    emit signalSetCurrentTestStep12(type);
}

void CanBusControl::slotSetCurrentTestStep13(int type)
{
    emit signalSetCurrentTestStep13(type);
}

void CanBusControl::slotSetCurrentTestStep14(int type)
{
    emit signalSetCurrentTestStep14(type);
}

void CanBusControl::slotSetCurrentTestStep15(int type)
{
    emit signalSetCurrentTestStep15(type);
}

void CanBusControl::slotSetCurrentTestStep16(int type)
{
    emit signalSetCurrentTestStep16(type);
}

void CanBusControl::slotSetSoft(const QString &id)
{
    m_software = id;
    emit signalSetSoft1(id);
    emit signalSetSoft2(id);
    emit signalSetSoft3(id);
    emit signalSetSoft4(id);
    emit signalSetSoft5(id);
    emit signalSetSoft6(id);
    emit signalSetSoft7(id);
    emit signalSetSoft8(id);
    emit signalSetSoft9(id);
    emit signalSetSoft10(id);
    emit signalSetSoft11(id);
    emit signalSetSoft12(id);
    emit signalSetSoft13(id);
    emit signalSetSoft14(id);
    emit signalSetSoft15(id);
    emit signalSetSoft16(id);
}

void CanBusControl::slotSetHard(const QString &id)
{
    m_hardware = id;
    emit signalSetHard1(id);
    emit signalSetHard2(id);
    emit signalSetHard3(id);
    emit signalSetHard4(id);
    emit signalSetHard5(id);
    emit signalSetHard6(id);
    emit signalSetHard7(id);
    emit signalSetHard8(id);
    emit signalSetHard9(id);
    emit signalSetHard10(id);
    emit signalSetHard11(id);
    emit signalSetHard12(id);
    emit signalSetHard13(id);
    emit signalSetHard14(id);
    emit signalSetHard15(id);
    emit signalSetHard16(id);
}


void CanBusControl::slotSetMod(bool mod)
{
    if_mes = mod;
    can1->slotSetMod(mod);
    can2->slotSetMod(mod);
    can3->slotSetMod(mod);
    can4->slotSetMod(mod);
    can5->slotSetMod(mod);
    can6->slotSetMod(mod);
    can7->slotSetMod(mod);
    can8->slotSetMod(mod);
    can9->slotSetMod(mod);
    can10->slotSetMod(mod);
    can11->slotSetMod(mod);
    can12->slotSetMod(mod);
    can13->slotSetMod(mod);
    can14->slotSetMod(mod);
    can15->slotSetMod(mod);
    can16->slotSetMod(mod);
    qDebug() << "mod is " << if_mes;
}

void CanBusControl::slotFinish1()
{
    emit signalFinish1();
}

void CanBusControl::slotFinish2()
{
    emit signalFinish2();
}

void CanBusControl::slotFinish3()
{
    emit signalFinish3();
}

void CanBusControl::slotFinish4()
{
    emit signalFinish4();
}

void CanBusControl::slotFinish5()
{
    emit signalFinish5();
}

void CanBusControl::slotFinish6()
{
    emit signalFinish6();
}

void CanBusControl::slotFinish7()
{
    emit signalFinish7();
}

void CanBusControl::slotFinish8()
{
    emit signalFinish8();
}

void CanBusControl::slotFinish9()
{
    emit signalFinish9();
}

void CanBusControl::slotFinish10()
{
    emit signalFinish10();
}

void CanBusControl::slotFinish11()
{
    emit signalFinish11();
}

void CanBusControl::slotFinish12()
{
    emit signalFinish12();
}

void CanBusControl::slotFinish13()
{
    emit signalFinish13();
}

void CanBusControl::slotFinish14()
{
    emit signalFinish14();
}

void CanBusControl::slotFinish15()
{
    emit signalFinish15();
}

void CanBusControl::slotFinish16()
{
    emit signalFinish16();
}

void CanBusControl::slotErrorFinish1()
{
    emit signalErrorFinish1();
}

void CanBusControl::slotErrorFinish2()
{
    emit signalErrorFinish2();
}

void CanBusControl::slotErrorFinish3()
{
    emit signalErrorFinish3();
}

void CanBusControl::slotErrorFinish4()
{
    emit signalErrorFinish4();
}

void CanBusControl::slotErrorFinish5()
{
    emit signalErrorFinish5();
}

void CanBusControl::slotErrorFinish6()
{
    emit signalErrorFinish6();
}

void CanBusControl::slotErrorFinish7()
{
    emit signalErrorFinish7();
}

void CanBusControl::slotErrorFinish8()
{
    emit signalErrorFinish8();
}

void CanBusControl::slotErrorFinish9()
{
    emit signalErrorFinish9();
}

void CanBusControl::slotErrorFinish10()
{
    emit signalErrorFinish10();
}

void CanBusControl::slotErrorFinish11()
{
    emit signalErrorFinish11();
}

void CanBusControl::slotErrorFinish12()
{
    emit signalErrorFinish12();
}

void CanBusControl::slotErrorFinish13()
{
    emit signalErrorFinish13();
}

void CanBusControl::slotErrorFinish14()
{
    emit signalErrorFinish14();
}

void CanBusControl::slotErrorFinish15()
{
    emit signalErrorFinish15();
}

void CanBusControl::slotErrorFinish16()
{
    emit signalErrorFinish16();
}

void CanBusControl::slotCheckStatus()
{
    if(!ifTesting1)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A101F4, data);
    }
    if(!ifTesting2)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A102F4, data);
    }
    if(!ifTesting3)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A103F4, data);
    }
    if(!ifTesting4)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A104F4, data);
    }
    if(!ifTesting5)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A105F4, data);
    }
    if(!ifTesting6)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A106F4, data);
    }
    if(!ifTesting7)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A107F4, data);
    }
    if(!ifTesting8)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A108F4, data);
    }
    if(!ifTesting9)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A109F4, data);
    }
    if(!ifTesting10)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A10AF4, data);
    }
    if(!ifTesting11)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A10BF4, data);
    }
    if(!ifTesting12)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A10CF4, data);
    }
    if(!ifTesting13)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A10DF4, data);
    }
    if(!ifTesting14)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A10EF4, data);
    }
    if(!ifTesting15)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A10FF4, data);
    }
    if(!ifTesting16)
    {
        unsigned char data[8];
        data[0]      = 1;
        data[1]      = 0xFF;
        data[2]      = 0xFF;
        data[3]      = 0xFF;
        data[4]      = 0xFF;
        data[5]      = 0xFF;
        data[6]      = 0xFF;
        data[7]      = 0xFF;
        TransmitCAN(0x18A110F4, data);
    }
}

void CanBusControl::slotTransmitCAN(PVCI_CAN_OBJ pSend, ULONG Len)
{
    emit signalTransmitCAN(deviceType, m_locate, m_pass, pSend, 1);
}

void CanBusControl::slotTransmitCAN(CAN_OBJ pSend, ULONG Len)
{
    emit signalTransmitCAN(deviceType, m_locate, m_pass, pSend, 1);
}

void CanBusControl::slotTestResult1(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A101F4, data);
    ifTesting1 = false;
}

void CanBusControl::slotTestResult2(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A102F4, data);
    ifTesting2 = false;
}

void CanBusControl::slotTestResult3(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A103F4, data);
    ifTesting3 = false;
}

void CanBusControl::slotTestResult4(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A104F4, data);
    ifTesting4 = false;
}

void CanBusControl::slotTestResult5(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A105F4, data);
    ifTesting5 = false;
}

void CanBusControl::slotTestResult6(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A106F4, data);
    ifTesting6 = false;
}

void CanBusControl::slotTestResult7(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A107F4, data);
    ifTesting7 = false;
}

void CanBusControl::slotTestResult8(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A108F4, data);
    ifTesting8 = false;
}

void CanBusControl::slotTestResult9(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A109F4, data);
    ifTesting9 = false;
}

void CanBusControl::slotTestResult10(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A10AF4, data);
    ifTesting10 = false;
}

void CanBusControl::slotTestResult11(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A10BF4, data);
    ifTesting11 = false;
}

void CanBusControl::slotTestResult12(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A10CF4, data);
    ifTesting12 = false;
}

void CanBusControl::slotTestResult13(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A10DF4, data);
    ifTesting13 = false;
}

void CanBusControl::slotTestResult14(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A10EF4, data);
    ifTesting14 = false;
}

void CanBusControl::slotTestResult15(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A10FF4, data);
    ifTesting15 = false;
}

void CanBusControl::slotTestResult16(bool result)
{
    unsigned char data[8];
    data[0]      = 3;
    data[1]      = result ? 1 : 2;
    data[2]      = 0xFF;
    data[3]      = 0xFF;
    data[4]      = 0xFF;
    data[5]      = 0xFF;
    data[6]      = 0xFF;
    data[7]      = 0xFF;
    TransmitCAN(0x18A110F4, data);
    ifTesting16 = false;
}

void CanBusControl::slotSerialError1()
{
    emit signalSerialError1();
}

void CanBusControl::slotSerialError2()
{
    emit signalSerialError2();
}

void CanBusControl::slotSerialError3()
{
    emit signalSerialError3();
}

void CanBusControl::slotSerialError4()
{
    emit signalSerialError4();
}

void CanBusControl::slotSerialError5()
{
    emit signalSerialError5();
}

void CanBusControl::slotSerialError6()
{
    emit signalSerialError6();
}

void CanBusControl::slotSerialError7()
{
    emit signalSerialError7();
}

void CanBusControl::slotSerialError8()
{
    emit signalSerialError8();
}

void CanBusControl::slotSerialError9()
{
    emit signalSerialError9();
}

void CanBusControl::slotSerialError10()
{
    emit signalSerialError10();
}

void CanBusControl::slotSerialError11()
{
    emit signalSerialError11();
}

void CanBusControl::slotSerialError12()
{
    emit signalSerialError12();
}

void CanBusControl::slotSerialError13()
{
    emit signalSerialError13();
}

void CanBusControl::slotSerialError14()
{
    emit signalSerialError14();
}

void CanBusControl::slotSerialError15()
{
    emit signalSerialError15();
}

void CanBusControl::slotSerialError16()
{
    emit signalSerialError16();
}

void CanBusControl::slotStartTest1()
{
    emit signalStartTest1();
    ifTesting1 = true;
    if_test1 = true;
}

void CanBusControl::slotStartTest2()
{
    emit signalStartTest2();
    ifTesting2 = true;
    if_test2 = true;
}

void CanBusControl::slotStartTest3()
{
    emit signalStartTest3();
    ifTesting3 = true;
    if_test3 = true;
}

void CanBusControl::slotStartTest4()
{
    emit signalStartTest4();
    ifTesting4 = true;
    if_test4 = true;
}

void CanBusControl::slotStartTest5()
{
    emit signalStartTest5();
    ifTesting5 = true;
    if_test5 = true;
}

void CanBusControl::slotStartTest6()
{
    emit signalStartTest6();
    ifTesting6 = true;
    if_test6 = true;
}

void CanBusControl::slotStartTest7()
{
    emit signalStartTest7();
    ifTesting7 = true;
    if_test7 = true;
}

void CanBusControl::slotStartTest8()
{
    emit signalStartTest8();
    ifTesting8 = true;
    if_test8 = true;
}

void CanBusControl::slotStartTest9()
{
    emit signalStartTest9();
    ifTesting9 = true;
    if_test9 = true;
}

void CanBusControl::slotStartTest10()
{
    emit signalStartTest10();
    ifTesting10 = true;
    if_test10 = true;
}

void CanBusControl::slotStartTest11()
{
    emit signalStartTest11();
    ifTesting11 = true;
    if_test11 = true;
}

void CanBusControl::slotStartTest12()
{
    emit signalStartTest12();
    ifTesting12 = true;
    if_test12 = true;
}

void CanBusControl::slotStartTest13()
{
    emit signalStartTest13();
    ifTesting13 = true;
    if_test13 = true;
}

void CanBusControl::slotStartTest14()
{
    emit signalStartTest14();
    ifTesting14 = true;
    if_test14 = true;
}

void CanBusControl::slotStartTest15()
{
    emit signalStartTest15();
    ifTesting15 = true;
    if_test15 = true;
}

void CanBusControl::slotStartTest16()
{
    emit signalStartTest16();
    ifTesting16 = true;
    if_test16 = true;
}

void CanBusControl::slotSetTestMod1()
{
    emit signalSetTestMod1();
}

void CanBusControl::slotSetTestMod2()
{
    emit signalSetTestMod2();
}

void CanBusControl::slotSetTestMod3()
{
    emit signalSetTestMod3();
}

void CanBusControl::slotSetTestMod4()
{
    emit signalSetTestMod4();
}

void CanBusControl::slotSetTestMod5()
{
    emit signalSetTestMod5();
}

void CanBusControl::slotSetTestMod6()
{
    emit signalSetTestMod6();
}

void CanBusControl::slotSetTestMod7()
{
    emit signalSetTestMod7();
}

void CanBusControl::slotSetTestMod8()
{
    emit signalSetTestMod8();
}

void CanBusControl::slotSetTestMod9()
{
    emit signalSetTestMod9();
}

void CanBusControl::slotSetTestMod10()
{
    emit signalSetTestMod10();
}

void CanBusControl::slotSetTestMod11()
{
    emit signalSetTestMod11();
}

void CanBusControl::slotSetTestMod12()
{
    emit signalSetTestMod12();
}

void CanBusControl::slotSetTestMod13()
{
    emit signalSetTestMod13();
}

void CanBusControl::slotSetTestMod14()
{
    emit signalSetTestMod14();
}

void CanBusControl::slotSetTestMod15()
{
    emit signalSetTestMod15();
}

void CanBusControl::slotSetTestMod16()
{
    emit signalSetTestMod16();
}

void CanBusControl::slotSetSerial1(QString serial)
{
    emit signalSetSerial1(serial);
}

void CanBusControl::slotSetSerial2(QString serial)
{
    emit signalSetSerial2(serial);
}

void CanBusControl::slotSetSerial3(QString serial)
{
    emit signalSetSerial3(serial);
}

void CanBusControl::slotSetSerial4(QString serial)
{
    emit signalSetSerial4(serial);
}

void CanBusControl::slotSetSerial5(QString serial)
{
    emit signalSetSerial5(serial);
}

void CanBusControl::slotSetSerial6(QString serial)
{
    emit signalSetSerial6(serial);
}

void CanBusControl::slotSetSerial7(QString serial)
{
    emit signalSetSerial7(serial);
}

void CanBusControl::slotSetSerial8(QString serial)
{
    emit signalSetSerial8(serial);
}

void CanBusControl::slotSetSerial9(QString serial)
{
    emit signalSetSerial9(serial);
}

void CanBusControl::slotSetSerial10(QString serial)
{
    emit signalSetSerial10(serial);
}

void CanBusControl::slotSetSerial11(QString serial)
{
    emit signalSetSerial11(serial);
}

void CanBusControl::slotSetSerial12(QString serial)
{
    emit signalSetSerial12(serial);
}

void CanBusControl::slotSetSerial13(QString serial)
{
    emit signalSetSerial13(serial);
}

void CanBusControl::slotSetSerial14(QString serial)
{
    emit signalSetSerial14(serial);
}

void CanBusControl::slotSetSerial15(QString serial)
{
    emit signalSetSerial15(serial);
}

void CanBusControl::slotSetSerial16(QString serial)
{
    emit signalSetSerial16(serial);
}

void CanBusControl::slotSetMAC1(QString serial)
{
    emit signalSetMAC1(serial);
}

void CanBusControl::slotSetMAC2(QString serial)
{
    emit signalSetMAC2(serial);
}

void CanBusControl::slotSetMAC3(QString serial)
{
    emit signalSetMAC3(serial);
}

void CanBusControl::slotSetMAC4(QString serial)
{
    emit signalSetMAC4(serial);
}

void CanBusControl::slotSetMAC5(QString serial)
{
    emit signalSetMAC5(serial);
}

void CanBusControl::slotSetMAC6(QString serial)
{
    emit signalSetMAC6(serial);
}

void CanBusControl::slotSetMAC7(QString serial)
{
    emit signalSetMAC7(serial);
}

void CanBusControl::slotSetMAC8(QString serial)
{
    emit signalSetMAC8(serial);
}

void CanBusControl::slotSetMAC9(QString serial)
{
    emit signalSetMAC9(serial);
}

void CanBusControl::slotSetMAC10(QString serial)
{
    emit signalSetMAC10(serial);
}

void CanBusControl::slotSetMAC11(QString serial)
{
    emit signalSetMAC11(serial);
}

void CanBusControl::slotSetMAC12(QString serial)
{
    emit signalSetMAC12(serial);
}

void CanBusControl::slotSetMAC13(QString serial)
{
    emit signalSetMAC13(serial);
}

void CanBusControl::slotSetMAC14(QString serial)
{
    emit signalSetMAC14(serial);
}

void CanBusControl::slotSetMAC15(QString serial)
{
    emit signalSetMAC15(serial);
}

void CanBusControl::slotSetMAC16(QString serial)
{
    emit signalSetMAC16(serial);
}

void CanBusControl::slotSetExcelItem1(std::vector<ExcelItem> items)
{
    qDebug() << "save excel1";
    emit signalSetExcelItem1(items);
}

void CanBusControl::slotSetExcelItem2(std::vector<ExcelItem> items)
{
    qDebug() << "save excel2";
    emit signalSetExcelItem2(items);
}

void CanBusControl::slotSetExcelItem3(std::vector<ExcelItem> items)
{
    qDebug() << "save excel3";
    emit signalSetExcelItem3(items);
}

void CanBusControl::slotSetExcelItem4(std::vector<ExcelItem> items)
{
    qDebug() << "save excel4";
    emit signalSetExcelItem4(items);
}

void CanBusControl::slotSetExcelItem5(std::vector<ExcelItem> items)
{
    qDebug() << "save excel5";
    emit signalSetExcelItem5(items);
}

void CanBusControl::slotSetExcelItem6(std::vector<ExcelItem> items)
{
    qDebug() << "save excel6";
    emit signalSetExcelItem6(items);
}

void CanBusControl::slotSetExcelItem7(std::vector<ExcelItem> items)
{
    qDebug() << "save excel7";
    emit signalSetExcelItem7(items);
}

void CanBusControl::slotSetExcelItem8(std::vector<ExcelItem> items)
{
    qDebug() << "save excel8";
    emit signalSetExcelItem8(items);
}

void CanBusControl::slotSetExcelItem9(std::vector<ExcelItem> items)
{
    qDebug() << "save excel9";
    emit signalSetExcelItem9(items);
}

void CanBusControl::slotSetExcelItem10(std::vector<ExcelItem> items)
{
    qDebug() << "save excel10";
    emit signalSetExcelItem10(items);
}

void CanBusControl::slotSetExcelItem11(std::vector<ExcelItem> items)
{
    qDebug() << "save excel11";
    emit signalSetExcelItem11(items);
}

void CanBusControl::slotSetExcelItem12(std::vector<ExcelItem> items)
{
    qDebug() << "save excel12";
    emit signalSetExcelItem12(items);
}

void CanBusControl::slotSetExcelItem13(std::vector<ExcelItem> items)
{
    qDebug() << "save excel13";
    emit signalSetExcelItem13(items);
}

void CanBusControl::slotSetExcelItem14(std::vector<ExcelItem> items)
{
    qDebug() << "save excel14";
    emit signalSetExcelItem14(items);
}

void CanBusControl::slotSetExcelItem15(std::vector<ExcelItem> items)
{
    qDebug() << "save excel15";
    emit signalSetExcelItem15(items);
}

void CanBusControl::slotSetExcelItem16(std::vector<ExcelItem> items)
{
    qDebug() << "save excel16";
    emit signalSetExcelItem16(items);
}

void CanBusControl::slotReceiveStatus1(const QString &status)
{
    emit signalReceiveStatus1(status);
}

void CanBusControl::slotReceiveStatus2(const QString &status)
{
    emit signalReceiveStatus2(status);
}

void CanBusControl::slotReceiveStatus3(const QString &status)
{
    emit signalReceiveStatus3(status);
}

void CanBusControl::slotReceiveStatus4(const QString &status)
{
    emit signalReceiveStatus4(status);
}

void CanBusControl::slotReceiveStatus5(const QString &status)
{
    emit signalReceiveStatus5(status);
}

void CanBusControl::slotReceiveStatus6(const QString &status)
{
    emit signalReceiveStatus6(status);
}

void CanBusControl::slotReceiveStatus7(const QString &status)
{
    emit signalReceiveStatus7(status);
}

void CanBusControl::slotReceiveStatus8(const QString &status)
{
    emit signalReceiveStatus8(status);
}

void CanBusControl::slotReceiveStatus9(const QString &status)
{
    emit signalReceiveStatus9(status);
}

void CanBusControl::slotReceiveStatus10(const QString &status)
{
    emit signalReceiveStatus10(status);
}

void CanBusControl::slotReceiveStatus11(const QString &status)
{
    emit signalReceiveStatus11(status);
}

void CanBusControl::slotReceiveStatus12(const QString &status)
{
    emit signalReceiveStatus12(status);
}

void CanBusControl::slotReceiveStatus13(const QString &status)
{
    emit signalReceiveStatus13(status);
}

void CanBusControl::slotReceiveStatus14(const QString &status)
{
    emit signalReceiveStatus14(status);
}

void CanBusControl::slotReceiveStatus15(const QString &status)
{
    emit signalReceiveStatus15(status);
}

void CanBusControl::slotReceiveStatus16(const QString &status)
{
    emit signalReceiveStatus16(status);
}

void CanBusControl::slotSendTestResult1(QJsonObject data)
{
    emit signalSendTestResult1(data);
}

void CanBusControl::slotSendTestResult2(QJsonObject data)
{
    emit signalSendTestResult2(data);
}

void CanBusControl::slotSendTestResult3(QJsonObject data)
{
    emit signalSendTestResult3(data);
}

void CanBusControl::slotSendTestResult4(QJsonObject data)
{
    emit signalSendTestResult4(data);
}

void CanBusControl::slotSendTestResult5(QJsonObject data)
{
    emit signalSendTestResult5(data);
}

void CanBusControl::slotSendTestResult6(QJsonObject data)
{
    emit signalSendTestResult6(data);
}

void CanBusControl::slotSendTestResult7(QJsonObject data)
{
    emit signalSendTestResult7(data);
}

void CanBusControl::slotSendTestResult8(QJsonObject data)
{
    emit signalSendTestResult8(data);
}

void CanBusControl::slotSendTestResult9(QJsonObject data)
{
    emit signalSendTestResult9(data);
}

void CanBusControl::slotSendTestResult10(QJsonObject data)
{
    emit signalSendTestResult10(data);
}

void CanBusControl::slotSendTestResult11(QJsonObject data)
{
    emit signalSendTestResult11(data);
}

void CanBusControl::slotSendTestResult12(QJsonObject data)
{
    emit signalSendTestResult12(data);
}

void CanBusControl::slotSendTestResult13(QJsonObject data)
{
    emit signalSendTestResult13(data);
}


void CanBusControl::slotSendTestResult14(QJsonObject data)
{
    emit signalSendTestResult14(data);
}


void CanBusControl::slotSendTestResult15(QJsonObject data)
{
    emit signalSendTestResult15(data);
}


void CanBusControl::slotSendTestResult16(QJsonObject data)
{
    emit signalSendTestResult16(data);
}


void CanBusControl::slotSetModFail1()
{
    if_test1 = false;
    emit signalSetModFail1();
}

void CanBusControl::slotSetModFail2()
{
    if_test2 = false;
    emit signalSetModFail2();
}

void CanBusControl::slotSetModFail3()
{
    if_test3 = false;
    emit signalSetModFail3();
}

void CanBusControl::slotSetModFail4()
{
    if_test4 = false;
    emit signalSetModFail4();
}

void CanBusControl::slotSetModFail5()
{
    if_test5 = false;
    emit signalSetModFail5();
}

void CanBusControl::slotSetModFail6()
{
    if_test6 = false;
    emit signalSetModFail6();
}

void CanBusControl::slotSetModFail7()
{
    if_test7 = false;
    emit signalSetModFail7();
}

void CanBusControl::slotSetModFail8()
{
    if_test8 = false;
    emit signalSetModFail8();
}

void CanBusControl::slotSetModFail9()
{
    if_test9 = false;
    emit signalSetModFail9();
}

void CanBusControl::slotSetModFail10()
{
    if_test10 = false;
    emit signalSetModFail10();
}

void CanBusControl::slotSetModFail11()
{
    if_test11 = false;
    emit signalSetModFail11();
}

void CanBusControl::slotSetModFail12()
{
    if_test12 = false;
    emit signalSetModFail12();
}

void CanBusControl::slotSetModFail13()
{
    if_test13 = false;
    emit signalSetModFail13();
}

void CanBusControl::slotSetModFail14()
{
    if_test14 = false;
    emit signalSetModFail14();
}

void CanBusControl::slotSetModFail15()
{
    if_test15 = false;
    emit signalSetModFail15();
}

void CanBusControl::slotSetModFail16()
{
    if_test16 = false;
    emit signalSetModFail16();
}


void CanBusControl::slotTimeout1()
{
    if_test1 = false;
    emit signalTimeout1();
}

void CanBusControl::slotTimeout2()
{
    if_test2 = false;
    emit signalTimeout2();
}

void CanBusControl::slotTimeout3()
{
    if_test3 = false;
    emit signalTimeout3();
}

void CanBusControl::slotTimeout4()
{
    if_test4 = false;
    emit signalTimeout4();
}

void CanBusControl::slotTimeout5()
{
    if_test5 = false;
    emit signalTimeout5();
}

void CanBusControl::slotTimeout6()
{
    if_test6 = false;
    emit signalTimeout6();
}

void CanBusControl::slotTimeout7()
{
    if_test7 = false;
    emit signalTimeout7();
}

void CanBusControl::slotTimeout8()
{
    if_test8 = false;
    emit signalTimeout8();
}

void CanBusControl::slotTimeout9()
{
    if_test9 = false;
    emit signalTimeout9();
}

void CanBusControl::slotTimeout10()
{
    if_test10 = false;
    emit signalTimeout10();
}

void CanBusControl::slotTimeout11()
{
    if_test11 = false;
    emit signalTimeout11();
}

void CanBusControl::slotTimeout12()
{
    if_test12 = false;
    emit signalTimeout12();
}

void CanBusControl::slotTimeout13()
{
    if_test13 = false;
    emit signalTimeout13();
}

void CanBusControl::slotTimeout14()
{
    if_test14 = false;
    emit signalTimeout14();
}

void CanBusControl::slotTimeout15()
{
    if_test15 = false;
    emit signalTimeout15();
}

void CanBusControl::slotTimeout16()
{
    if_test16 = false;
    emit signalTimeout16();
}

void CanBusControl::slotFinishSet1()
{
    emit signalFinishSet1();
}

void CanBusControl::slotFinishSet2()
{
    emit signalFinishSet2();
}

void CanBusControl::slotFinishSet3()
{
    emit signalFinishSet3();
}

void CanBusControl::slotFinishSet4()
{
    emit signalFinishSet4();
}

void CanBusControl::slotFinishSet5()
{
    emit signalFinishSet5();
}

void CanBusControl::slotFinishSet6()
{
    emit signalFinishSet6();
}

void CanBusControl::slotFinishSet7()
{
    emit signalFinishSet7();
}

void CanBusControl::slotFinishSet8()
{
    emit signalFinishSet8();
}

void CanBusControl::slotFinishSet9()
{
    emit signalFinishSet9();
}

void CanBusControl::slotFinishSet10()
{
    emit signalFinishSet10();
}

void CanBusControl::slotFinishSet11()
{
    emit signalFinishSet11();
}

void CanBusControl::slotFinishSet12()
{
    emit signalFinishSet12();
}

void CanBusControl::slotFinishSet13()
{
    emit signalFinishSet13();
}

void CanBusControl::slotFinishSet14()
{
    emit signalFinishSet14();
}

void CanBusControl::slotFinishSet15()
{
    emit signalFinishSet15();
}

void CanBusControl::slotFinishSet16()
{
    emit signalFinishSet16();
}


void CanBusControl::slotSetModFail21()
{
    if_test1 = false;
    emit signalSetModFail21();
}

void CanBusControl::slotSetModFail22()
{
    if_test2 = false;
    emit signalSetModFail22();
}

void CanBusControl::slotSetModFail23()
{
    if_test3 = false;
    emit signalSetModFail23();
}

void CanBusControl::slotSetModFail24()
{
    if_test4 = false;
    emit signalSetModFail24();
}

void CanBusControl::slotSetModFail25()
{
    if_test5 = false;
    emit signalSetModFail25();
}

void CanBusControl::slotSetModFail26()
{
    if_test6 = false;
    emit signalSetModFail26();
}

void CanBusControl::slotSetModFail27()
{
    if_test7 = false;
    emit signalSetModFail27();
}

void CanBusControl::slotSetModFail28()
{
    if_test8 = false;
    emit signalSetModFail28();
}

void CanBusControl::slotSetModFail29()
{
    if_test9 = false;
    emit signalSetModFail29();
}

void CanBusControl::slotSetModFail210()
{
    if_test10 = false;
    emit signalSetModFail210();
}

void CanBusControl::slotSetModFail211()
{
    if_test11 = false;
    emit signalSetModFail211();
}

void CanBusControl::slotSetModFail212()
{
    if_test12 = false;
    emit signalSetModFail212();
}

void CanBusControl::slotSetModFail213()
{
    if_test13 = false;
    emit signalSetModFail213();
}

void CanBusControl::slotSetModFail214()
{
    if_test14 = false;
    emit signalSetModFail214();
}

void CanBusControl::slotSetModFail215()
{
    if_test15 = false;
    emit signalSetModFail215();
}

void CanBusControl::slotSetModFail216()
{
    if_test16 = false;
    emit signalSetModFail216();
}

void CanBusControl::slotFinishSet21()
{
    emit signalFinishSet21();
}

void CanBusControl::slotFinishSet22()
{
    emit signalFinishSet22();
}

void CanBusControl::slotFinishSet23()
{
    emit signalFinishSet23();
}

void CanBusControl::slotFinishSet24()
{
    emit signalFinishSet24();
}

void CanBusControl::slotFinishSet25()
{
    emit signalFinishSet25();
}

void CanBusControl::slotFinishSet26()
{
    emit signalFinishSet26();
}

void CanBusControl::slotFinishSet27()
{
    emit signalFinishSet27();
}

void CanBusControl::slotFinishSet28()
{
    emit signalFinishSet28();
}

void CanBusControl::slotFinishSet29()
{
    emit signalFinishSet29();
}

void CanBusControl::slotFinishSet210()
{
    emit signalFinishSet210();
}

void CanBusControl::slotFinishSet211()
{
    emit signalFinishSet211();
}

void CanBusControl::slotFinishSet212()
{
    emit signalFinishSet212();
}

void CanBusControl::slotFinishSet213()
{
    emit signalFinishSet213();
}

void CanBusControl::slotFinishSet214()
{
    emit signalFinishSet214();
}

void CanBusControl::slotFinishSet215()
{
    emit signalFinishSet215();
}

void CanBusControl::slotFinishSet216()
{
    emit signalFinishSet216();
}


void CanBusControl::slotReadKB(int cf, int type, int locate)
{

}

void CanBusControl::slotSetKB(int cf, int type, float k, float b, int locate)
{
    switch(type)
    {
    case 1:
    {
        unsigned char data[8];
        data[0] = 0x0F;
        data[1] = 1;
        data[2] = 1;
        data[3]  = int(k * 1000) >> 8;
        data[4]  = int(k * 1000);
        int temp = b * 1000;
        if (b > 0)
        {
            data[5] = temp >> 8;
            data[6] = temp;
        }
        else if (b < 0)
        {
            data[5] = (65535 + temp) >> 8;
            data[6] = (65535 + temp);
        }
        else
        {
            data[5] = 0x00;
            data[6] = 0x00;
        }
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 2:
    {
        unsigned char data[8];
        data[0] = 0x0F;
        data[1] = 1;
        data[2] = 2;
        data[3]  = int(k * 1000) >> 8;
        data[4]  = int(k * 1000);
        int temp = b * 1000;
        if (b > 0)
        {
            data[5] = temp >> 8;
            data[6] = temp;
        }
        else if (b < 0)
        {
            data[5] = (65535 + temp) >> 8;
            data[6] = (65535 + temp);
        }
        else
        {
            data[5] = 0x00;
            data[6] = 0x00;
        }
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 3:
    {
        unsigned char data[8];
        data[0] = 0x0D;
        data[1] = 1;
        data[2]  = int(k * 1000) >> 8;
        data[3]  = int(k * 1000);
        int temp = b * 1000;
        if (b > 0)
        {
            data[4] = temp >> 8;
            data[5] = temp;
        }
        else if (b < 0)
        {
            data[4] = (65535 + temp) >> 8;
            data[5] = (65535 + temp);
        }
        else
        {
            data[4] = 0x00;
            data[5] = 0x00;
        }
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 4:
    {
        unsigned char data[8];
        data[0] = 0x0F;
        data[1] = 2;
        data[2] = 1;
        data[3]  = int(k * 1000) >> 8;
        data[4]  = int(k * 1000);
        int temp = b * 1000;
        if (b > 0)
        {
            data[5] = temp >> 8;
            data[6] = temp;
        }
        else if (b < 0)
        {
            data[5] = (65535 + temp) >> 8;
            data[6] = (65535 + temp);
        }
        else
        {
            data[5] = 0x00;
            data[6] = 0x00;
        }
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 5:
    {
        unsigned char data[8];
        data[0] = 0x0F;
        data[1] = 3;
        data[2] = 1;
        data[3]  = int(k * 1000) >> 8;
        data[4]  = int(k * 1000);
        int temp = b * 1000;
        if (b > 0)
        {
            data[5] = temp >> 8;
            data[6] = temp;
        }
        else if (b < 0)
        {
            data[5] = (65535 + temp) >> 8;
            data[6] = (65535 + temp);
        }
        else
        {
            data[5] = 0x00;
            data[6] = 0x00;
        }
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 6:
    {
        unsigned char data[8];
        data[0] = 0x22;
        data[1]  = int(k * 1000) >> 8;
        data[2]  = int(k * 1000);
        int temp = b * 1000;
        if (b > 0)
        {
            data[3] = temp >> 8;
            data[4] = temp;
        }
        else if (b < 0)
        {
            data[3] = (65535 + temp) >> 8;
            data[4] = (65535 + temp);
        }
        else
        {
            data[3] = 0x00;
            data[4] = 0x00;
        }
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 7:
    {
        unsigned char data[8];
        data[0] = 0x22;
        data[1]  = int(k * 1000) >> 8;
        data[2]  = int(k * 1000);
        int temp = b * 1000;
        if (b > 0)
        {
            data[3] = temp >> 8;
            data[4] = temp;
        }
        else if (b < 0)
        {
            data[3] = (65535 + temp) >> 8;
            data[4] = (65535 + temp);
        }
        else
        {
            data[3] = 0x00;
            data[4] = 0x00;
        }
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    default:
        break;
    }
}

void CanBusControl::slotReadInfo(int type, int locate)
{
    m_info = type;
    switch(m_info)
    {
    case 1:
    case 2:
    case 4:
    case 5:
    {
        unsigned char data[8];
        data[0] = 0x0E;
        data[1] = 1;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 3:
    case 6:
    {
        unsigned char data[8];
        data[0] = 0x0C;
        data[1] = 1;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 7:
    {
        unsigned char data[8];
        data[0] = 0x0E;
        data[1] = 2;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 8:
    {
        unsigned char data[8];
        data[0] = 0x0E;
        data[1] = 3;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 9:
    {
        unsigned char data[8];
        data[0] = 0x0C;
        data[1] = 3;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 10:
    case 11:
    {
        unsigned char data[8];
        data[0] = 0x0C;
        data[1] = 2;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 101:
    case 102:
    case 104:
    case 105:
    {
        unsigned char data[8];
        data[0] = 0x0E;
        data[1] = 1;
        data[2] = 1;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 103:
    case 106:
    {
        unsigned char data[8];
        data[0] = 0x0C;
        data[1] = 1;
        data[2] = 1;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 107:
    {
        unsigned char data[8];
        data[0] = 0x0E;
        data[1] = 2;
        data[2] = 1;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 108:
    {
        unsigned char data[8];
        data[0] = 0x0E;
        data[1] = 3;
        data[2] = 1;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 109:
    {
        unsigned char data[8];
        data[0] = 0x0C;
        data[1] = 3;
        data[2] = 1;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    case 110:
    case 111:
    {
        unsigned char data[8];
        data[0] = 0x0C;
        data[1] = 2;
        data[2] = 1;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
        break;
    }
    default:
        break;
    }
}

void CanBusControl::slotOpenDO(int locate)
{
    unsigned char data[8];
    data[0] = 0x01;
    data[1] = 0x07;
    data[2] = 0x01;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
}

void CanBusControl::slotCloseDO(int locate)
{
    unsigned char data[8];
    data[0] = 0x01;
    data[1] = 0x07;
    data[2] = 0x00;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    TransmitCAN(((0x1855 << 8) | ((locate + 0x60 - 1) & 0xFF)) << 8 | 0xF4, data);
}

void CanBusControl::slotChange(int type, int locate)
{
    switch(type)
    {
    case 1:
    {
        QByteArray temp;
        switch(locate)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        {
            temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0", 27);
            emit signalSendData(temp);
            break;
        }
        case 5:
        case 6:
        case 7:
        case 8:
        {
            temp = QByteArray("\x06\x10\x00\x00\x00\x0A\x14\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0", 27);
            emit signalSendData(temp);
            break;  
        }
        case 9:
        case 10:
        case 11:
        case 12:
        {
            temp = QByteArray("\x0A\x10\x00\x00\x00\x0A\x14\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0", 27);
            emit signalSendData(temp);
            break;
        }
        case 13:
        case 14:
        case 15:
        case 16:
        {
            temp = QByteArray("\x0E\x10\x00\x00\x00\x0A\x14\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0\x0A\xF0", 27);
            emit signalSendData(temp);
            break;
        }
        default:
            break;
        }
        break;
    }
    case 2:
    {
        QByteArray temp;
        switch(locate)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        {
            temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20", 27);
            emit signalSendData(temp);
            break;
        }
        case 5:
        case 6:
        case 7:
        case 8:
        {
            temp = QByteArray("\x06\x10\x00\x00\x00\x0A\x14\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20", 27);

            emit signalSendData(temp);
            break;
        }
        case 9:
        case 10:
        case 11:
        case 12:
        {
            temp = QByteArray("\x0A\x10\x00\x00\x00\x0A\x14\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20", 27);

            emit signalSendData(temp);
            break;
        }
        case 13:
        case 14:
        case 15:
        case 16:
        {
            temp = QByteArray("\x0E\x10\x00\x00\x00\x0A\x14\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20\x03\x20", 27);

            emit signalSendData(temp);
            break;
        }
        default:
            break;
        }
        break;
    }
    case 3:
    {
        QByteArray temp;
        switch(locate)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        {
            temp = QByteArray("\x01\x10\x10\x00\x00\x01\x02\x01\xF4", 9);
            emit signalSendData(temp);
            break;
        }
        case 5:
        case 6:
        case 7:
        case 8:
        {
            temp = QByteArray("\x05\x10\x10\x00\x00\x01\x02\x01\xF4", 9);
            emit signalSendData(temp);
            break;
        }
        case 9:
        case 10:
        case 11:
        case 12:
        {
            temp = QByteArray("\x09\x10\x10\x00\x00\x01\x02\x01\xF4", 9);
            emit signalSendData(temp);
            break;
        }
        case 13:
        case 14:
        case 15:
        case 16:
        {
            temp = QByteArray("\x0D\x10\x10\x00\x00\x01\x02\x01\xF4", 9);
            emit signalSendData(temp);
            break;
        }
        default:
            break;
        }
        emit signalSendData(temp);
        break;
    }
    case 4:
    {
        QByteArray temp;
        switch(locate)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        {
            temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68", 27);

            emit signalSendData(temp);
            break;
        }
        case 5:
        case 6:
        case 7:
        case 8:
        {
            temp = QByteArray("\x06\x10\x00\x00\x00\x0A\x14\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68", 27);

            emit signalSendData(temp);
            break;
        }
        case 9:
        case 10:
        case 11:
        case 12:
        {
            temp = QByteArray("\x0A\x10\x00\x00\x00\x0A\x14\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68", 27);

            emit signalSendData(temp);
            break;
        }
        case 13:
        case 14:
        case 15:
        case 16:
        {
            temp = QByteArray("\x0E\x10\x00\x00\x00\x0A\x14\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68\x10\x68", 27);

            emit signalSendData(temp);
            break;
        }
        default:
            break;
        }
        break;
    }
    case 5:
    {
        QByteArray temp;
        switch(locate)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        {
            temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98", 27);

            emit signalSendData(temp);
            break;
        }
        case 5:
        case 6:
        case 7:
        case 8:
        {
            temp = QByteArray("\x06\x10\x00\x00\x00\x0A\x14\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98", 27);

            emit signalSendData(temp);
            break;
        }
        case 9:
        case 10:
        case 11:
        case 12:
        {
            temp = QByteArray("\x0A\x10\x00\x00\x00\x0A\x14\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98", 27);

            emit signalSendData(temp);
            break;
        }
        case 13:
        case 14:
        case 15:
        case 16:
        {
            temp = QByteArray("\x0E\x10\x00\x00\x00\x0A\x14\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98\x08\x98", 27);

            emit signalSendData(temp);
            break;
        }
        default:
            break;
        }
        break;
    }
    case 6:
    {
        QByteArray temp;
        switch(locate)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        {
            temp = QByteArray("\x01\x10\x10\x00\x00\x01\x02\x05\x78", 9);
            emit signalSendData(temp);
            break;
        }
        case 5:
        case 6:
        case 7:
        case 8:
        {
            temp = QByteArray("\x05\x10\x10\x00\x00\x01\x02\x05\x78", 9);
            emit signalSendData(temp);
            break;
        }
        case 9:
        case 10:
        case 11:
        case 12:
        {
            temp = QByteArray("\x09\x10\x10\x00\x00\x01\x02\x05\x78", 9);
            emit signalSendData(temp);
            break;
        }
        case 13:
        case 14:
        case 15:
        case 16:
        {
            temp = QByteArray("\x0D\x10\x10\x00\x00\x01\x02\x05\x78", 9);
            emit signalSendData(temp);
            break;
        }
        default:
            break;
        }
        emit signalSendData(temp);
        break;
    }
    case 7:
    {
        QByteArray temp;
        switch(locate)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        {
            temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC", 27);

            emit signalSendData(temp);
            break;
        }
        case 5:
        case 6:
        case 7:
        case 8:
        {
            temp = QByteArray("\x06\x10\x00\x00\x00\x0A\x14\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC", 27);

            emit signalSendData(temp);
            break;
        }
        case 9:
        case 10:
        case 11:
        case 12:
        {
            temp = QByteArray("\x0A\x10\x00\x00\x00\x0A\x14\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC", 27);

            emit signalSendData(temp);
            break;
        }
        case 13:
        case 14:
        case 15:
        case 16:
        {
            temp = QByteArray("\x0E\x10\x00\x00\x00\x0A\x14\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC\x0D\xAC", 27);

            emit signalSendData(temp);
            break;
        }
        default:
            break;
        }
        break;
    }
    case 8:
    {
        QByteArray temp;
        switch(locate)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        {
            temp = QByteArray("\x02\x10\x00\x00\x00\x0A\x14\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC", 27);

            emit signalSendData(temp);
            break;
        }
        case 5:
        case 6:
        case 7:
        case 8:
        {
            temp = QByteArray("\x06\x10\x00\x00\x00\x0A\x14\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC", 27);

            emit signalSendData(temp);
            break;
        }
        case 9:
        case 10:
        case 11:
        case 12:
        {
            temp = QByteArray("\x0A\x10\x00\x00\x00\x0A\x14\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC", 27);

            emit signalSendData(temp);
            break;
        }
        case 13:
        case 14:
        case 15:
        case 16:
        {
            temp = QByteArray("\x0E\x10\x00\x00\x00\x0A\x14\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC\x05\xDC", 27);

            emit signalSendData(temp);
            break;
        }
        default:
            break;
        }
        break;
    }
    case 9:
    {
        QByteArray temp;
        switch(locate)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        {
            temp = QByteArray("\x01\x10\x10\x00\x00\x01\x02\x03\xE8", 9);
            emit signalSendData(temp);
            break;
        }
        case 5:
        case 6:
        case 7:
        case 8:
        {
            temp = QByteArray("\x05\x10\x10\x00\x00\x01\x02\x03\xE8", 9);
            emit signalSendData(temp);
            break;
        }
        case 9:
        case 10:
        case 11:
        case 12:
        {
            temp = QByteArray("\x09\x10\x10\x00\x00\x01\x02\x03\xE8", 9);
            emit signalSendData(temp);
            break;
        }
        case 13:
        case 14:
        case 15:
        case 16:
        {
            temp = QByteArray("\x0D\x10\x10\x00\x00\x01\x02\x03\xE8", 9);
            emit signalSendData(temp);
            break;
        }
        default:
            break;
        }
        emit signalSendData(temp);
        break;
    }
    default:
        break;
    }
}

void CanBusControl::slotSetUpdateStatus1(bool status)
{
    ifCanUpdate1 = status;
}

void CanBusControl::slotSetUpdateStatus2(bool status)
{
    ifCanUpdate2 = status;
}

void CanBusControl::slotSetUpdateStatus3(bool status)
{
    ifCanUpdate3 = status;
}

void CanBusControl::slotSetUpdateStatus4(bool status)
{
    ifCanUpdate4 = status;
}

void CanBusControl::slotSetUpdateStatus5(bool status)
{
    ifCanUpdate5 = status;
}

void CanBusControl::slotSetUpdateStatus6(bool status)
{
    ifCanUpdate6 = status;
}

void CanBusControl::slotSetUpdateStatus7(bool status)
{
    ifCanUpdate7 = status;
}

void CanBusControl::slotSetUpdateStatus8(bool status)
{
    ifCanUpdate8 = status;
}

void CanBusControl::slotSetUpdateStatus9(bool status)
{
    ifCanUpdate9 = status;
}

void CanBusControl::slotSetUpdateStatus10(bool status)
{
    ifCanUpdate10 = status;
}

void CanBusControl::slotSetUpdateStatus11(bool status)
{
    ifCanUpdate11 = status;
}

void CanBusControl::slotSetUpdateStatus12(bool status)
{
    ifCanUpdate12 = status;
}

void CanBusControl::slotSetUpdateStatus13(bool status)
{
    ifCanUpdate13 = status;
}

void CanBusControl::slotSetUpdateStatus14(bool status)
{
    ifCanUpdate14 = status;
}

void CanBusControl::slotSetUpdateStatus15(bool status)
{
    ifCanUpdate15 = status;
}

void CanBusControl::slotSetUpdateStatus16(bool status)
{
    ifCanUpdate16 = status;
}

void CanBusControl::slotCheckIfCanUpdate1()
{
    if((ifCanUpdate1 || !if_test1) && (ifCanUpdate2 || !if_test2) && (ifCanUpdate3 || !if_test3) && (ifCanUpdate4 || !if_test4))
    {
        emit signalIfCanUpdate1To4(true);
        ifCanUpdate1 = false;
        ifCanUpdate2 = false;
        ifCanUpdate3 = false;
        ifCanUpdate4 = false;
    }
    else
    {
        emit signalIfCanUpdate1To4(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate2()
{
    if((ifCanUpdate1 || !if_test1) && (ifCanUpdate2 || !if_test2) && (ifCanUpdate3 || !if_test3) && (ifCanUpdate4 || !if_test4))
    {
        emit signalIfCanUpdate1To4(true);
        ifCanUpdate1 = false;
        ifCanUpdate2 = false;
        ifCanUpdate3 = false;
        ifCanUpdate4 = false;
    }
    else
    {
        emit signalIfCanUpdate1To4(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate3()
{
    if((ifCanUpdate1 || !if_test1) && (ifCanUpdate2 || !if_test2) && (ifCanUpdate3 || !if_test3) && (ifCanUpdate4 || !if_test4))
    {
        emit signalIfCanUpdate1To4(true);
        ifCanUpdate1 = false;
        ifCanUpdate2 = false;
        ifCanUpdate3 = false;
        ifCanUpdate4 = false;
    }
    else
    {
        emit signalIfCanUpdate1To4(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate4()
{
    if((ifCanUpdate1 || !if_test1) && (ifCanUpdate2 || !if_test2) && (ifCanUpdate3 || !if_test3) && (ifCanUpdate4 || !if_test4))
    {
        emit signalIfCanUpdate1To4(true);
        ifCanUpdate1 = false;
        ifCanUpdate2 = false;
        ifCanUpdate3 = false;
        ifCanUpdate4 = false;
    }
    else
    {
        emit signalIfCanUpdate1To4(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate5()
{
    if((ifCanUpdate5 || !if_test5) && (ifCanUpdate6 || !if_test6) && (ifCanUpdate7 || !if_test7) && (ifCanUpdate8 || !if_test8))
    {
        emit signalIfCanUpdate5To8(true);
        ifCanUpdate5 = false;
        ifCanUpdate6 = false;
        ifCanUpdate7 = false;
        ifCanUpdate8 = false;
    }
    else
    {
        emit signalIfCanUpdate5To8(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate6()
{
    if((ifCanUpdate5 || !if_test5) && (ifCanUpdate6 || !if_test6) && (ifCanUpdate7 || !if_test7) && (ifCanUpdate8 || !if_test8))
    {
        emit signalIfCanUpdate5To8(true);
        ifCanUpdate5 = false;
        ifCanUpdate6 = false;
        ifCanUpdate7 = false;
        ifCanUpdate8 = false;
    }
    else
    {
        emit signalIfCanUpdate5To8(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate7()
{
    if((ifCanUpdate5 || !if_test5) && (ifCanUpdate6 || !if_test6) && (ifCanUpdate7 || !if_test7) && (ifCanUpdate8 || !if_test8))
    {
        emit signalIfCanUpdate5To8(true);
        ifCanUpdate5 = false;
        ifCanUpdate6 = false;
        ifCanUpdate7 = false;
        ifCanUpdate8 = false;
    }
    else
    {
        emit signalIfCanUpdate5To8(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate8()
{
    if((ifCanUpdate5 || !if_test5) && (ifCanUpdate6 || !if_test6) && (ifCanUpdate7 || !if_test7) && (ifCanUpdate8 || !if_test8))
    {
        emit signalIfCanUpdate5To8(true);
        ifCanUpdate5 = false;
        ifCanUpdate6 = false;
        ifCanUpdate7 = false;
        ifCanUpdate8 = false;
    }
    else
    {
        emit signalIfCanUpdate5To8(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate9()
{
    if((ifCanUpdate9 || !if_test9) && (ifCanUpdate10 || !if_test10) && (ifCanUpdate11 || !if_test11) && (ifCanUpdate12 || !if_test12))
    {
        emit signalIfCanUpdate9To12(true);
        ifCanUpdate9 = false;
        ifCanUpdate10 = false;
        ifCanUpdate11 = false;
        ifCanUpdate12 = false;
    }
    else
    {
        emit signalIfCanUpdate9To12(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate10()
{
    if((ifCanUpdate9 || !if_test9) && (ifCanUpdate10 || !if_test10) && (ifCanUpdate11 || !if_test11) && (ifCanUpdate12 || !if_test12))
    {
        emit signalIfCanUpdate9To12(true);
        ifCanUpdate9 = false;
        ifCanUpdate10 = false;
        ifCanUpdate11 = false;
        ifCanUpdate12 = false;
    }
    else
    {
        emit signalIfCanUpdate9To12(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate11()
{
    if((ifCanUpdate9 || !if_test9) && (ifCanUpdate10 || !if_test10) && (ifCanUpdate11 || !if_test11) && (ifCanUpdate12 || !if_test12))
    {
        emit signalIfCanUpdate9To12(true);
        ifCanUpdate9 = false;
        ifCanUpdate10 = false;
        ifCanUpdate11 = false;
        ifCanUpdate12 = false;
    }
    else
    {
        emit signalIfCanUpdate9To12(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate12()
{
    if((ifCanUpdate9 || !if_test9) && (ifCanUpdate10 || !if_test10) && (ifCanUpdate11 || !if_test11) && (ifCanUpdate12 || !if_test12))
    {
        emit signalIfCanUpdate9To12(true);
        ifCanUpdate9 = false;
        ifCanUpdate10 = false;
        ifCanUpdate11 = false;
        ifCanUpdate12 = false;
    }
    else
    {
        emit signalIfCanUpdate9To12(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate13()
{
    if((ifCanUpdate13 || !if_test13) && (ifCanUpdate14 || !if_test14) && (ifCanUpdate15 || !if_test15) && (ifCanUpdate16 || !if_test16))
    {
        emit signalIfCanUpdate13To16(true);
        ifCanUpdate13 = false;
        ifCanUpdate14 = false;
        ifCanUpdate15 = false;
        ifCanUpdate16 = false;
    }
    else
    {
        emit signalIfCanUpdate13To16(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate14()
{
    if((ifCanUpdate13 || !if_test13) && (ifCanUpdate14 || !if_test14) && (ifCanUpdate15 || !if_test15) && (ifCanUpdate16 || !if_test16))
    {
        emit signalIfCanUpdate13To16(true);
        ifCanUpdate13 = false;
        ifCanUpdate14 = false;
        ifCanUpdate15 = false;
        ifCanUpdate16 = false;
    }
    else
    {
        emit signalIfCanUpdate13To16(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate15()
{
    if((ifCanUpdate13 || !if_test13) && (ifCanUpdate14 || !if_test14) && (ifCanUpdate15 || !if_test15) && (ifCanUpdate16 || !if_test16))
    {
        emit signalIfCanUpdate13To16(true);
        ifCanUpdate13 = false;
        ifCanUpdate14 = false;
        ifCanUpdate15 = false;
        ifCanUpdate16 = false;
    }
    else
    {
        emit signalIfCanUpdate13To16(false);
    }
}

void CanBusControl::slotCheckIfCanUpdate16()
{
    if((ifCanUpdate13 || !if_test13) && (ifCanUpdate14 || !if_test14) && (ifCanUpdate15 || !if_test15) && (ifCanUpdate16 || !if_test16))
    {
        emit signalIfCanUpdate13To16(true);
        ifCanUpdate13 = false;
        ifCanUpdate14 = false;
        ifCanUpdate15 = false;
        ifCanUpdate16 = false;
    }
    else
    {
        emit signalIfCanUpdate13To16(false);
    }
}

void CanBusControl::updateTestItem1()
{
    can1->updateTestItem();
}

void CanBusControl::updateTestItem2()
{
    can2->updateTestItem();
}

void CanBusControl::updateTestItem3()
{
    can3->updateTestItem();
}

void CanBusControl::updateTestItem4()
{
    can4->updateTestItem();
}

void CanBusControl::updateTestItem5()
{
    can5->updateTestItem();
}

void CanBusControl::updateTestItem6()
{
    can6->updateTestItem();
}

void CanBusControl::updateTestItem7()
{
    can7->updateTestItem();
}

void CanBusControl::updateTestItem8()
{
    can8->updateTestItem();
}

void CanBusControl::updateTestItem9()
{
    can9->updateTestItem();
}

void CanBusControl::updateTestItem10()
{
    can10->updateTestItem();
}

void CanBusControl::updateTestItem11()
{
    can11->updateTestItem();
}

void CanBusControl::updateTestItem12()
{
    can12->updateTestItem();
}

void CanBusControl::updateTestItem13()
{
    can13->updateTestItem();
}

void CanBusControl::updateTestItem14()
{
    can14->updateTestItem();
}

void CanBusControl::updateTestItem15()
{
    can15->updateTestItem();
}

void CanBusControl::updateTestItem16()
{
    can16->updateTestItem();
}

void CanBusControl::slotMac(const QString &mac)
{
}

void CanBusControl::slotSetSerial(const QString &serial)
{
}

void CanBusControl::slotCANStatus(CANFXYOLDRETURNCODE status)
{
    switch (status)
    {
    case FXYOLDOPENCANSUCCESS:
    {
        emit signalReceiveStatus1("打开设备成功！");
        break;
    }
    case FXYOLDOPENCANFAILED:
    {
        emit signalReceiveErrorStatus("打开设备失败，请重试！");
        break;
    }
    case FXYOLDCLOSECANSUCCESS:
    {
        emit signalReceiveStatus1("关闭设备成功！");
        break;
    }
    case FXYOLDCLOSECANFAILED:
    {
        emit signalReceiveErrorStatus("关闭设备失败，请重试！");
        break;
    }
    case FXYOLDCLOSEBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在打开设备前关闭设备");
        break;
    }
    case FXYOLDSTARTCANSUCCESS:
    {
        emit signalReceiveStatus1("连接CAN通道成功！");
        break;
    }
    case FXYOLDSTARTCANFAILED:
    {
        emit signalReceiveErrorStatus("连接CAN通道失败，请重试！");
        break;
    }
    case FXYOLDSTARTBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在打开设备前连接CAN通道");
        break;
    }
    case FXYOLDRESETCANSUCCESS:
    {
        emit signalReceiveStatus1("重置CAN通道成功！");
        break;
    }
    case FXYOLDRESETCANFAILED:
    {
        emit signalReceiveErrorStatus("重置CAN通道失败，请重试！");
        break;
    }
    case FXYOLDRESETBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在连接通道前重置CAN通道");
        break;
    }
    case FXYOLDRESETBEFORECONNECT:
    {
        emit signalReceiveErrorStatus("无法在启动通信前重置CAN通道");
        break;
    }
    case FXYOLDCONNECTCANFILED:
    {
        emit signalReceiveErrorStatus("启动CAN通信失败，请重试！");
        break;
    }
    case FXYOLDCONNECTCANSUCCESS:
    {
        emit signalReceiveStatus1("启动CAN通信成功！");
        break;
    }
    case FXYOLDCANERRORDATAFORM:
    {
        emit signalReceiveStatus1("数据格式错误！");
        break;
    }
    case FXYOLDCANERRORSHIEDCODEFORM:
    {
        emit signalReceiveStatus1("掩码错误！");
        break;
    }
    case FXYOLDCANERRORTIMER0FORM:
    {
        emit signalReceiveErrorStatus("波特率错误，请重试！");
        break;
    }
    case FXYOLDCANERRORTIMER1FORM:
    {
        emit signalReceiveErrorStatus("波特率错误，请重试！");
        break;
    }
    case FXYOLDINITIALCANFAILED:
    {
        emit signalReceiveErrorStatus("初始化CAN协议失败，请重试！");
        break;
    }
    case FXYOLDREADBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在打开设备前读取信号");
        break;
    }
    case FXYOLDREADBEFORECONN:
    {
        emit signalReceiveErrorStatus("无法在启动通道前读取信号");
        break;
    }
    case FXYOLDREADBEFORESTART:
    {
        emit signalReceiveErrorStatus("无法在连接通信前读取信号");
        break;
    }
    case FXYOLDREADCANERROR:
    {
        emit signalReceiveErrorStatus("读取信息失败，请重试！");
        break;
    }
    case FXYOLDCRCERROR:
    {
        emit signalReceiveErrorStatus("CRC校验未通过，请重试！");
        break;
    }
    case FXYOLDREADCANSUCCESS:
    {
        break;
    }
    default:
        break;
    }
}

void CanBusControl::slotCANStatus(CANFXYNEWRETURNCODE status)
{
    switch (status)
    {
    case FXYNEWOPENCANSUCCESS:
    {
        emit signalReceiveStatus1("打开设备成功！");
        break;
    }
    case FXYNEWOPENCANFAILED:
    {
        emit signalReceiveErrorStatus("打开设备失败，请重试！");
        break;
    }
    case FXYNEWCLOSECANSUCCESS:
    {
        emit signalReceiveStatus1("关闭设备成功！");
        break;
    }
    case FXYNEWCLOSECANFAILED:
    {
        emit signalReceiveErrorStatus("关闭设备失败，请重试！");
        break;
    }
    case FXYNEWCLOSEBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在打开设备前关闭设备");
        break;
    }
    case FXYNEWSTARTCANSUCCESS:
    {
        emit signalReceiveStatus1("连接CAN通道成功！");
        break;
    }
    case FXYNEWSTARTCANFAILED:
    {
        emit signalReceiveErrorStatus("连接CAN通道失败，请重试！");
        break;
    }
    case FXYNEWSTARTBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在打开设备前连接CAN通道");
        break;
    }
    case FXYNEWRESETCANSUCCESS:
    {
        emit signalReceiveStatus1("重置CAN通道成功！");
        break;
    }
    case FXYNEWRESETCANFAILED:
    {
        emit signalReceiveErrorStatus("重置CAN通道失败，请重试！");
        break;
    }
    case FXYNEWRESETBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在连接通道前重置CAN通道");
        break;
    }
    case FXYNEWRESETBEFORECONNECT:
    {
        emit signalReceiveErrorStatus("无法在启动通信前重置CAN通道");
        break;
    }
    case FXYNEWCONNECTCANFILED:
    {
        emit signalReceiveErrorStatus("启动CAN通信失败，请重试！");
        break;
    }
    case FXYNEWCONNECTCANSUCCESS:
    {
        emit signalReceiveStatus1("启动CAN通信成功！");
        break;
    }
    case FXYNEWCANERRORDATAFORM:
    {
        emit signalReceiveStatus1("数据格式错误！");
        break;
    }
    case FXYNEWCANERRORSHIEDCODEFORM:
    {
        emit signalReceiveStatus1("掩码错误！");
        break;
    }
    case FXYNEWCANERRORTIMER0FORM:
    {
        emit signalReceiveErrorStatus("波特率错误，请重试！");
        break;
    }
    case FXYNEWCANERRORTIMER1FORM:
    {
        emit signalReceiveErrorStatus("波特率错误，请重试！");
        break;
    }
    case FXYNEWINITIALCANFAILED:
    {
        emit signalReceiveErrorStatus("初始化CAN协议失败，请重试！");
        break;
    }
    case FXYNEWREADBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在打开设备前读取信号");
        break;
    }
    case FXYNEWREADBEFORECONN:
    {
        emit signalReceiveErrorStatus("无法在启动通道前读取信号");
        break;
    }
    case FXYNEWREADBEFORESTART:
    {
        emit signalReceiveErrorStatus("无法在连接通信前读取信号");
        break;
    }
    case FXYNEWREADCANERROR:
    {
        emit signalReceiveErrorStatus("读取信息失败，请重试！");
        break;
    }
    case FXYNEWCRCERROR:
    {
        emit signalReceiveErrorStatus("CRC校验未通过，请重试！");
        break;
    }
    case FXYNEWREADCANSUCCESS:
    {
        break;
    }
    default:
        break;
    }
}

void CanBusControl::slotCANStatus(CANZLGRETURNCODE status)
{
    switch (status)
    {
    case ZLGOPENCANSUCCESS:
    {
        emit signalReceiveStatus1("打开设备成功！");
        break;
    }
    case ZLGOPENCANFAILED:
    {
        emit signalReceiveErrorStatus("打开设备失败，请重试！");
        break;
    }
    case ZLGCLOSECANSUCCESS:
    {
        emit signalReceiveStatus1("关闭设备成功！");
        break;
    }
    case ZLGCLOSECANFAILED:
    {
        emit signalReceiveErrorStatus("关闭设备失败，请重试！");
        break;
    }
    case ZLGCLOSEBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在打开设备前关闭设备");
        break;
    }
    case ZLGSTARTCANSUCCESS:
    {
        emit signalReceiveStatus1("连接CAN通道成功！");
        break;
    }
    case ZLGSTARTCANFAILED:
    {
        emit signalReceiveErrorStatus("连接CAN通道失败，请重试！");
        break;
    }
    case ZLGSTARTBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在打开设备前连接CAN通道");
        break;
    }
    case ZLGRESETCANSUCCESS:
    {
        emit signalReceiveStatus1("重置CAN通道成功！");
        break;
    }
    case ZLGRESETCANFAILED:
    {
        emit signalReceiveErrorStatus("重置CAN通道失败，请重试！");
        break;
    }
    case ZLGRESETBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在连接通道前重置CAN通道");
        break;
    }
    case ZLGRESETBEFORECONNECT:
    {
        emit signalReceiveErrorStatus("无法在启动通信前重置CAN通道");
        break;
    }
    case ZLGCONNECTCANFILED:
    {
        emit signalReceiveErrorStatus("启动CAN通信失败，请重试！");
        break;
    }
    case ZLGCONNECTCANSUCCESS:
    {
        emit signalReceiveStatus1("启动CAN通信成功！");
        break;
    }
    case ZLGCANERRORDATAFORM:
    {
        emit signalReceiveStatus1("数据格式错误！");
        break;
    }
    case ZLGCANERRORSHIEDCODEFORM:
    {
        emit signalReceiveStatus1("掩码错误！");
        break;
    }
    case ZLGCANERRORTIMER0FORM:
    {
        emit signalReceiveErrorStatus("波特率错误，请重试！");
        break;
    }
    case ZLGCANERRORTIMER1FORM:
    {
        emit signalReceiveErrorStatus("波特率错误，请重试！");
        break;
    }
    case ZLGINITIALCANFAILED:
    {
        emit signalReceiveErrorStatus("初始化CAN协议失败，请重试！");
        break;
    }
    case ZLGREADBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在打开设备前读取信号");
        break;
    }
    case ZLGREADBEFORECONN:
    {
        emit signalReceiveErrorStatus("无法在启动通道前读取信号");
        break;
    }
    case ZLGREADBEFORESTART:
    {
        emit signalReceiveErrorStatus("无法在连接通信前读取信号");
        break;
    }
    case ZLGREADCANERROR:
    {
        emit signalReceiveErrorStatus("读取信息失败，请重试！");
        break;
    }
    case ZLGCRCERROR:
    {
        emit signalReceiveErrorStatus("CRC校验未通过，请重试！");
        break;
    }
    case ZLGREADCANSUCCESS:
    {
        break;
    }
    default:
        break;
    }
}

void CanBusControl::slotCANStatus(CANGCRETURNCODE status)
{
    switch (status)
    {
    case GCOPENCANSUCCESS:
    {
        emit signalReceiveStatus1("打开设备成功！");
        break;
    }
    case GCOPENCANFAILED:
    {
        emit signalReceiveErrorStatus("打开设备失败，请重试！");
        break;
    }
    case GCCLOSECANSUCCESS:
    {
        emit signalReceiveStatus1("关闭设备成功！");
        break;
    }
    case GCCLOSECANFAILED:
    {
        emit signalReceiveErrorStatus("关闭设备失败，请重试！");
        break;
    }
    case GCCLOSEBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在打开设备前关闭设备");
        break;
    }
    case GCSTARTCANSUCCESS:
    {
        emit signalReceiveStatus1("连接CAN通道成功！");
        break;
    }
    case GCSTARTCANFAILED:
    {
        emit signalReceiveErrorStatus("连接CAN通道失败，请重试！");
        break;
    }
    case GCSTARTBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在打开设备前连接CAN通道");
        break;
    }
    case GCRESETCANSUCCESS:
    {
        emit signalReceiveStatus1("重置CAN通道成功！");
        break;
    }
    case GCRESETCANFAILED:
    {
        emit signalReceiveErrorStatus("重置CAN通道失败，请重试！");
        break;
    }
    case GCRESETBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在连接通道前重置CAN通道");
        break;
    }
    case GCRESETBEFORECONNECT:
    {
        emit signalReceiveErrorStatus("无法在启动通信前重置CAN通道");
        break;
    }
    case GCCONNECTCANFILED:
    {
        emit signalReceiveErrorStatus("启动CAN通信失败，请重试！");
        break;
    }
    case GCCONNECTCANSUCCESS:
    {
        emit signalReceiveStatus1("启动CAN通信成功！");
        break;
    }
    case GCCANERRORDATAFORM:
    {
        emit signalReceiveStatus1("数据格式错误！");
        break;
    }
    case GCCANERRORSHIEDCODEFORM:
    {
        emit signalReceiveStatus1("掩码错误！");
        break;
    }
    case GCCANERRORTIMER0FORM:
    {
        emit signalReceiveErrorStatus("波特率错误，请重试！");
        break;
    }
    case GCCANERRORTIMER1FORM:
    {
        emit signalReceiveErrorStatus("波特率错误，请重试！");
        break;
    }
    case GCINITIALCANFAILED:
    {
        emit signalReceiveErrorStatus("初始化CAN协议失败，请重试！");
        break;
    }
    case GCREADBEFOREOPEN:
    {
        emit signalReceiveErrorStatus("无法在打开设备前读取信号");
        break;
    }
    case GCREADBEFORECONN:
    {
        emit signalReceiveErrorStatus("无法在启动通道前读取信号");
        break;
    }
    case GCREADBEFORESTART:
    {
        emit signalReceiveErrorStatus("无法在连接通信前读取信号");
        break;
    }
    case GCREADCANERROR:
    {
        emit signalReceiveErrorStatus("读取信息失败，请重试！");
        break;
    }
    case GCCRCERROR:
    {
        emit signalReceiveErrorStatus("CRC校验未通过，请重试！");
        break;
    }
    case GCREADCANSUCCESS:
    {
        break;
    }
    default:
        break;
    }
}

void CanBusControl::slotReceiveData(VCI_CAN_OBJ pReceive)
{
    currentData.clear();
    for (int i = 0; i < 8; i++)
    {
        currentData.append(pReceive.Data[i]);
    }
    currentData = currentData.toHex();
    std::string tempString;
    unsigned_to_hex(pReceive.ID, tempString);
    int tempInt = pReceive.ID >> 8;
    if(tempInt == 0x18A1F4)
    {
        //qDebug() << "receive " << currentData << " from " << QString::fromStdString(tempString);
    }
    switch(pReceive.ID)
    {
    case 0x18A1F401:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A101F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting1)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x185701F4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial1 = "";
                    receiveArray1.clear();
                    receiveArray1.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray1.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray1.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial1 = QString::fromUtf8(receiveArray1);
                    emit signalReceiveSerial1(m_serial1);
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F402:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A102F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting2)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x185702F4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial2 = "";
                    receiveArray2.clear();
                    receiveArray2.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray2.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray2.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial2 = QString::fromUtf8(receiveArray2);
                    emit signalReceiveSerial2(m_serial2);
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F403:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A103F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting3)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x185703F4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial3 = "";
                    receiveArray3.clear();
                    receiveArray3.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray3.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray3.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial3 = QString::fromUtf8(receiveArray3);
                    emit signalReceiveSerial3(m_serial3);
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F404:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A104F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting4)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x185704F4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial4 = "";
                    receiveArray4.clear();
                    receiveArray4.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray4.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray4.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial4 = QString::fromUtf8(receiveArray4) ;
                    emit signalReceiveSerial4(m_serial4) ;
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F405:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A105F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting5)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x185705F4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial5 = "";
                    receiveArray5.clear();
                    receiveArray5.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray5.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray5.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial5 = QString::fromUtf8(receiveArray5) ;
                    emit signalReceiveSerial5(m_serial5) ;
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F406:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A106F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting6)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x185706F4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial6 = "";
                    receiveArray6.clear();
                    receiveArray6.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray6.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray6.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial6 = QString::fromUtf8(receiveArray6) ;
                    emit signalReceiveSerial6(m_serial6) ;
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F407:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A107F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting7)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x185707F4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial7 = "";
                    receiveArray7.clear();
                    receiveArray7.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray7.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray7.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial7 = QString::fromUtf8(receiveArray7) ;
                    emit signalReceiveSerial7(m_serial7) ;
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F408:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A108F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting8)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x185708F4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial8 = "";
                    receiveArray8.clear();
                    receiveArray8.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray8.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray8.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial8 = QString::fromUtf8(receiveArray8) ;
                    emit signalReceiveSerial8(m_serial8) ;
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F409:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A109F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting9)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x185709F4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial9 = "";
                    receiveArray9.clear();
                    receiveArray9.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray9.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray9.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial9 = QString::fromUtf8(receiveArray9) ;
                    emit signalReceiveSerial9(m_serial9) ;
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F40A:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A10AF4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting10)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x18570AF4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial10 = "";
                    receiveArray10.clear();
                    receiveArray10.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray10.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray10.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial10 = QString::fromUtf8(receiveArray10) ;
                    emit signalReceiveSerial10(m_serial10) ;
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F40B:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A10BF4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting11)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x18570BF4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial11 = "";
                    receiveArray11.clear();
                    receiveArray11.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray11.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray11.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial11 = QString::fromUtf8(receiveArray11) ;
                    emit signalReceiveSerial11(m_serial11) ;
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F40C:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A10CF4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting12)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x18570CF4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial12 = "";
                    receiveArray12.clear();
                    receiveArray12.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray12.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray12.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial12 = QString::fromUtf8(receiveArray12) ;
                    emit signalReceiveSerial12(m_serial12) ;
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F40D:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A10DF4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting13)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x18570DF4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial13 = "";
                    receiveArray13.clear();
                    receiveArray13.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray13.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray13.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial13 = QString::fromUtf8(receiveArray13) ;
                    emit signalReceiveSerial13(m_serial13) ;
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F40E:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A10EF4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting14)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x18570EF4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial14 = "";
                    receiveArray14.clear();
                    receiveArray14.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray14.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray14.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial14 = QString::fromUtf8(receiveArray14) ;
                    emit signalReceiveSerial14(m_serial14) ;
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F40F:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A10FF4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting15)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x18570FF4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial15 = "";
                    receiveArray15.clear();
                    receiveArray15.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray15.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray15.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial15 = QString::fromUtf8(receiveArray15) ;
                    emit signalReceiveSerial15(m_serial15) ;
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x18A1F410:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A110F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            if(!ifTesting16)
            {
                unsigned char data[8];
                data[0]      = 3;
                data[1]      = 0;
                data[2]      = 0;
                data[3]      = 0;
                data[4]      = 0;
                data[5]      = 0;
                data[6]      = 0;
                data[7]      = 0;
                TransmitCAN(0x185710F4, data);
                switch(pReceive.Data[1])
                {
                case 1:
                {
                    m_serial16 = "";
                    receiveArray16.clear();
                    receiveArray16.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 2:
                case 3:
                case 4:
                {
                    receiveArray16.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    break;
                }
                case 5:
                {
                    receiveArray16.append(pReceive.Data[2])
                        .append(pReceive.Data[3])
                        .append(pReceive.Data[4])
                        .append(pReceive.Data[5])
                        .append(pReceive.Data[6])
                        .append(pReceive.Data[7]);
                    m_serial16 = QString::fromUtf8(receiveArray16) ;
                    emit signalReceiveSerial16(m_serial16) ;
                    break;
                }
                default:
                    break;
                }
            }
        }
        break;
    }
    case 0x1855F460:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 1);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 1);
            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 1);
            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F461:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 2);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 2);
            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 2);
            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F462:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 3);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 3);

            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 3);

            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F463:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 4);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 4);

            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 4);

            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F464:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 5);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 5);

            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 5);

            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F465:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 6);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 6);

            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 6);

            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F466:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 7);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 7);

            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 7);

            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F467:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 8);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 8);

            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 8);

            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F468:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 9);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 9);

            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 9);

            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F469:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 10);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 10);

            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 10);

            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F46A:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 11);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 11);

            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 11);

            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F46B:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 12);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 12);

            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 12);

            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F46C:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 13);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 13);

            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 13);

            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F46D:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 14);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 14);

            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 14);

            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F46E:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 15);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 15);

            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 15);

            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
    case 0x1855F46F:
    {
        if (0x0E == static_cast<int>(pReceive.Data[0]))
        {
            float temp = 0.0;
            if((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5]) > 32767)
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
            }
            else
            {
                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
                                static_cast<int>(pReceive.Data[5]))) / 10.0;
            }
            emit signalGetInfo(m_info, temp, 16);
            emit signalReceiveData(pReceive);
        }
        else if(pReceive.Data[0] == 0x0C)
        {
            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
                    static_cast<int>(pReceive.Data[5])) /
                   10.0;
            if (m_info == 9)
            {
                emit signalGetInfo(99, m_gzgd * 10, 16);

            }
            else if(m_info != 0)
            {
                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
                                   static_cast<int>(pReceive.Data[5])) /
                                  10.0, 16);

            }
            else
            {
                emit signalReceiveData(pReceive);
            }
        }
        else
        {
            emit signalReceiveData(pReceive);
        }
        break;
    }
//    case 0x1855F470:
//    {
//        if (0x0E == static_cast<int>(pReceive.Data[0]))
//        {
//            float temp = 0.0;
//            if((static_cast<int>(pReceive.Data[4]) << 8) +
//                    static_cast<int>(pReceive.Data[5]) > 32767)
//            {
//                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
//                                static_cast<int>(pReceive.Data[5])) - 65535) / 10.0;
//            }
//            else
//            {
//                temp = (((static_cast<int>(pReceive.Data[4]) << 8) +
//                                static_cast<int>(pReceive.Data[5]))) / 10.0;
//            }
//            emit signalGetInfo(m_info, temp);
//            emit signalReceiveData(pReceive);
//        }
//        else if(pReceive.Data[0] == 0x0C)
//        {
//            float m_gzgd = ((static_cast<int>(pReceive.Data[4]) << 8) +
//                    static_cast<int>(pReceive.Data[5])) /
//                   10.0;
//            if (m_info == 9)
//            {
//                emit signalGetInfo(99, m_gzgd * 10);
//
//            }
//            else if(m_info != 0)
//            {
//                emit signalGetInfo(m_info, ((static_cast<int>(pReceive.Data[4]) << 8) +
//                                   static_cast<int>(pReceive.Data[5])) /
//                                  10.0);
//
//            }
//            else
//            {
//                emit signalReceiveData(pReceive);
//            }
//        }
//        else
//        {
//            emit signalReceiveData(pReceive);
//        }
//        break;
//    }
    default:
    {
        emit signalReceiveData(pReceive);
        break;
    }
    }
}

void CanBusControl::slotReceiveData(CAN_OBJ pReceive)
{
    currentData.clear();
    for (int i = 0; i < 8; i++)
    {
        currentData.append(pReceive.Data[i]);
    }
    currentData = currentData.toHex();
    std::string tempString;
    unsigned_to_hex(pReceive.ID, tempString);
    if(pReceive.ID == (0x18A1F4 << 8) | (m_locate & 0xFF))
    {
        qDebug() << "receive " << currentData << " from "
                 << QString::fromStdString(tempString);
    }
    switch(pReceive.ID)
    {
    case 0x18A1F401:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A101F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial1 = "";
                receiveArray1.clear();
                receiveArray1.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray1.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray1.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial1 = QString::fromUtf8(receiveArray1);
                emit signalReceiveSerial1(m_serial1);
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F402:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A102F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial2 = "";
                receiveArray2.clear();
                receiveArray2.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray2.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray2.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial2 = QString::fromUtf8(receiveArray2);
                emit signalReceiveSerial2(m_serial2);
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F403:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A103F4, data);
            }
        }
        else if(pReceive.Data[0] == 3)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial3 = "";
                receiveArray3.clear();
                receiveArray3.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray3.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray3.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial3 = QString::fromUtf8(receiveArray3);
                emit signalReceiveSerial3(m_serial3);
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F404:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A104F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial4 = "";
                receiveArray4.clear();
                receiveArray4.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray4.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray4.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial4 = QString::fromUtf8(receiveArray4) ;
                emit signalReceiveSerial4(m_serial4) ;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F405:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A105F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial5 = "";
                receiveArray5.clear();
                receiveArray5.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray5.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray5.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial5 = QString::fromUtf8(receiveArray5) ;
                emit signalReceiveSerial5(m_serial5) ;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F406:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A106F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial6 = "";
                receiveArray6.clear();
                receiveArray6.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray6.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray6.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial6 = QString::fromUtf8(receiveArray6) ;
                emit signalReceiveSerial6(m_serial6) ;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F407:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A107F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial7 = "";
                receiveArray7.clear();
                receiveArray7.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray7.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray7.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial7 = QString::fromUtf8(receiveArray7) ;
                emit signalReceiveSerial7(m_serial7) ;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F408:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A108F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial8 = "";
                receiveArray8.clear();
                receiveArray8.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray8.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray8.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial8 = QString::fromUtf8(receiveArray8) ;
                emit signalReceiveSerial8(m_serial8) ;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F409:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A109F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial9 = "";
                receiveArray9.clear();
                receiveArray9.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray9.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray9.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial9 = QString::fromUtf8(receiveArray9) ;
                emit signalReceiveSerial9(m_serial9) ;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F40A:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A10AF4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial10 = "";
                receiveArray10.clear();
                receiveArray10.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray10.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray10.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial10 = QString::fromUtf8(receiveArray10) ;
                emit signalReceiveSerial10(m_serial10) ;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F40B:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A10BF4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial11 = "";
                receiveArray11.clear();
                receiveArray11.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray11.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray11.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial11 = QString::fromUtf8(receiveArray11) ;
                emit signalReceiveSerial11(m_serial11) ;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F40C:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A10CF4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial12 = "";
                receiveArray12.clear();
                receiveArray12.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray12.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray12.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial12 = QString::fromUtf8(receiveArray12) ;
                emit signalReceiveSerial12(m_serial12) ;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F40D:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A10DF4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial13 = "";
                receiveArray13.clear();
                receiveArray13.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray13.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray13.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial13 = QString::fromUtf8(receiveArray13) ;
                emit signalReceiveSerial13(m_serial13) ;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F40E:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A10EF4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial14 = "";
                receiveArray14.clear();
                receiveArray14.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray14.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray14.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial14 = QString::fromUtf8(receiveArray14) ;
                emit signalReceiveSerial14(m_serial14) ;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F40F:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A10FF4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial15 = "";
                receiveArray15.clear();
                receiveArray15.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray15.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray15.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial15 = QString::fromUtf8(receiveArray15) ;
                emit signalReceiveSerial15(m_serial15) ;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case 0x18A1F410:
    {
        if(pReceive.Data[0] == 1)
        {
            if(pReceive.Data[2] == 1)
            {
                unsigned char data[8];
                data[0]      = 2;
                data[1]      = 0xFF;
                data[2]      = 0xFF;
                data[3]      = 0xFF;
                data[4]      = 0xFF;
                data[5]      = 0xFF;
                data[6]      = 0xFF;
                data[7]      = 0xFF;
                TransmitCAN(0x18A110F4, data);
            }
        }
        else if(pReceive.Data[0] == 2)
        {
            switch(pReceive.Data[1])
            {
            case 1:
            {
                m_serial16 = "";
                receiveArray16.clear();
                receiveArray16.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                receiveArray16.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                break;
            }
            case 5:
            {
                receiveArray16.append(pReceive.Data[2])
                    .append(pReceive.Data[3])
                    .append(pReceive.Data[4])
                    .append(pReceive.Data[5])
                    .append(pReceive.Data[6])
                    .append(pReceive.Data[7]);
                m_serial16 = QString::fromUtf8(receiveArray16) ;
                emit signalReceiveSerial16(m_serial16) ;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    default:
    {
        emit signalReceiveData(pReceive);
        break;
    }
    }
}

void CanBusControl::slotReceiveData2(VCI_CAN_OBJ pReceive)
{
    emit signalReceiveData2(pReceive);
}

void CanBusControl::slotReceiveData2(CAN_OBJ pReceive)
{
    emit signalReceiveData2(pReceive);
}

void CanBusControl::slotChooseZLGType(int type)
{
    deviceType = type;
    qDebug() << "open zlg as " << type;
    emit signalOpenCAN(type, 0);
}

void CanBusControl::slotChooseGCType(int type)
{
    deviceType = type;
    qDebug() << "open gc as " << type;
    emit signalOpenCAN(type, 0);
}
