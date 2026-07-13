#include "rs485parsethread.h"
#include <QDebug>

void Rs485ParseThread::run()
{
    Rs485RingBuf& rxBuf = Rs485BufferMgr::GetInstance()->GetRxBuf();
    Rs485Frame frame;
    while (m_run)
    {
        if (rxBuf.popBlock(frame))
        {
            HandleRs485Response(frame.sendData, frame.sendTick);
        }
    }
    qDebug() << "485解析线程退出";
}

void Rs485ParseThread::HandleRs485Response(const QByteArray& data, quint64 recvTick)
{
    Q_UNUSED(recvTick);
    if (data.isEmpty())
    {
        qDebug() << "[485-DISPATCH] receive empty 485 data"
                 << "thread" << QThread::currentThread()
                 << "threadId" << QThread::currentThreadId();
        return;
    }
    QByteArray tmp = QByteArray(data.constData(), data.size());
    const int address = static_cast<int>(static_cast<unsigned char>(data.at(0)));
    qDebug() << "[485-DISPATCH] receive"
             << "addr" << address
             << "bytes" << tmp.size()
             << "preview" << tmp.left(24).toHex()
             << "thread" << QThread::currentThread()
             << "threadId" << QThread::currentThreadId();

    if(address >= 1 && address <=4)
    {
        qDebug() << "[485-DISPATCH] route group" << "1-4" << "addr" << address;
        emit signalSend485Data1(tmp);
        emit signalSend485Data2(tmp);
        emit signalSend485Data3(tmp);
        emit signalSend485Data4(tmp);
    }
    else if(address >=5 && address <=8)
    {
        qDebug() << "[485-DISPATCH] route group" << "5-8" << "addr" << address;
        emit signalSend485Data5(tmp);
        emit signalSend485Data6(tmp);
        emit signalSend485Data7(tmp);
        emit signalSend485Data8(tmp);
    }
    else if(address >=9 && address <=12)
    {
        qDebug() << "[485-DISPATCH] route group" << "9-12" << "addr" << address;
        emit signalSend485Data9(tmp);
        emit signalSend485Data10(tmp);
        emit signalSend485Data11(tmp);
        emit signalSend485Data12(tmp);
    }
    else if(address >=13 && address <=16)
    {
        qDebug() << "[485-DISPATCH] route group" << "13-16" << "addr" << address;
        emit signalSend485Data13(tmp);
        emit signalSend485Data14(tmp);
        emit signalSend485Data15(tmp);
        emit signalSend485Data16(tmp);
    }
    else
    {
        qDebug() << "[485-DISPATCH] unexpected addr" << address
                 << "bytes" << tmp.size()
                 << "preview" << tmp.left(24).toHex();
    }
}
