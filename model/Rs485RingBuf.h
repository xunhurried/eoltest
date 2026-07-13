#ifndef RS485BUFFER_H
#define RS485BUFFER_H

#include <QByteArray>
#include <mutex>
#include <condition_variable>
#include <atomic>

#define RS485_BUF_CAP 256

// 单帧485请求包
struct Rs485Frame
{
    QByteArray sendData;
    quint64 sendTick = 0; // 发送时间戳，业务匹配应答用
};

// 线程安全环形缓冲
class Rs485RingBuf
{
public:
    // 写入帧
    bool push(const Rs485Frame& frame)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        if (isFull()) return false;
        m_buf[m_w] = frame;
        m_w = (m_w + 1) % RS485_BUF_CAP;
        m_cv.notify_one();
        return true;
    }

    // 非阻塞读取（发送轮询线程用）
    bool popNoBlock(Rs485Frame& out)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        if (isEmpty()) return false;
        out = m_buf[m_r];
        m_r = (m_r + 1) % RS485_BUF_CAP;
        return true;
    }

    // 阻塞读取（业务解析线程用）
    bool popBlock(Rs485Frame& out)
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_cv.wait(lock, [this](){ return !isEmpty() || m_exit; });
        if (m_exit) return false;
        out = m_buf[m_r];
        m_r = (m_r + 1) % RS485_BUF_CAP;
        return true;
    }

    // 停止时唤醒所有阻塞线程
    void wakeAll()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_exit = true;
        m_cv.notify_all();
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_r = m_w = 0;
    }

private:
    bool isEmpty() const { return m_r == m_w; }
    bool isFull() const { return (m_w + 1) % RS485_BUF_CAP == m_r; }

    Rs485Frame m_buf[RS485_BUF_CAP];
    uint32_t m_r = 0, m_w = 0;
    std::mutex m_mtx;
    std::condition_variable m_cv;
    bool m_exit = false;
};

// 全局单例缓冲管理器：Tx发送缓冲 + Rx应答缓冲
class Rs485BufferMgr
{
public:
    static Rs485BufferMgr* GetInstance()
    {
        static Rs485BufferMgr inst;
        return &inst;
    }
    Rs485RingBuf& GetTxBuf() { return m_txBuf; }
    Rs485RingBuf& GetRxBuf() { return m_rxBuf; }
    void WakeAll()
    {
        m_txBuf.wakeAll();
        m_rxBuf.wakeAll();
    }
    void ClearAll()
    {
        m_txBuf.clear();
        m_rxBuf.clear();
    }
private:
    Rs485BufferMgr() = default;
    ~Rs485BufferMgr() = default;
    Rs485BufferMgr(const Rs485BufferMgr&) = delete;
    Rs485BufferMgr& operator=(const Rs485BufferMgr&) = delete;
    Rs485RingBuf m_txBuf;
    Rs485RingBuf m_rxBuf;
};

#endif // RS485BUFFER_H
