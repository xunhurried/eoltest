#ifndef RS485PARSETHREAD_H
#define RS485PARSETHREAD_H

#include <QThread>
#include <atomic>
#include <QByteArray>
#include "Rs485RingBuf.h"

class Rs485ParseThread : public QThread
{
    Q_OBJECT // 必须加，否则信号编译报错
public:
    std::atomic<bool> m_run{false};

    void Start()
    {
        m_run = true;
        start();
    }
    void Stop()
    {
        m_run = false;
        Rs485BufferMgr::GetInstance()->WakeAll();
        if (!wait(500))
        {
            terminate();
            wait();
        }
    }

signals:
    // 全部路由信号声明，统一传QByteArray
    void signalSend485Data1(const QByteArray& data);
    void signalSend485Data2(const QByteArray& data);
    void signalSend485Data3(const QByteArray& data);
    void signalSend485Data4(const QByteArray& data);

    void signalSend485Data5(const QByteArray& data);
    void signalSend485Data6(const QByteArray& data);
    void signalSend485Data7(const QByteArray& data);
    void signalSend485Data8(const QByteArray& data);

    void signalSend485Data9(const QByteArray& data);
    void signalSend485Data10(const QByteArray& data);
    void signalSend485Data11(const QByteArray& data);
    void signalSend485Data12(const QByteArray& data);

    void signalSend485Data13(const QByteArray& data);
    void signalSend485Data14(const QByteArray& data);
    void signalSend485Data15(const QByteArray& data);
    void signalSend485Data16(const QByteArray& data);

protected:
    void run() override;
private:
    void HandleRs485Response(const QByteArray& data, quint64 recvTick);
};

#endif // RS485PARSETHREAD_H
