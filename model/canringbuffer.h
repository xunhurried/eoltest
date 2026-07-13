#pragma once
#include "include/ControlCAN.h"
#include <mutex>
#include <condition_variable>
#include <cstring>

#define CAN_BUF_CAPACITY 512

class CanRingBuffer
{
public:
    void wakeAll()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_cv.notify_all();
    }

    bool push(const PVCI_CAN_OBJ& frame)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        if (isFull())
            return false;
        m_buf[m_writeIdx] = frame;
        m_writeIdx = (m_writeIdx + 1) % CAN_BUF_CAPACITY;
        m_cv.notify_one();
        return true;
    }

    bool popNoBlock(PVCI_CAN_OBJ& outFrame)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        if (isEmpty())
            return false;
        outFrame = m_buf[m_readIdx];
        m_readIdx = (m_readIdx + 1) % CAN_BUF_CAPACITY;
        return true;
    }

    bool popBlock(PVCI_CAN_OBJ& outFrame)
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_cv.wait(lock, [this](){ return !isEmpty(); });
        outFrame = m_buf[m_readIdx];
        m_readIdx = (m_readIdx + 1) % CAN_BUF_CAPACITY;
        return true;
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_readIdx = m_writeIdx = 0;
    }

    uint32_t size()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        return (m_writeIdx - m_readIdx + CAN_BUF_CAPACITY) % CAN_BUF_CAPACITY;
    }

private:
    bool isEmpty() const { return m_readIdx == m_writeIdx; }
    bool isFull() const { return (m_writeIdx + 1) % CAN_BUF_CAPACITY == m_readIdx; }

    PVCI_CAN_OBJ m_buf[CAN_BUF_CAPACITY];
    uint32_t m_readIdx = 0;
    uint32_t m_writeIdx = 0;
    std::mutex m_mtx;
    std::condition_variable m_cv;
};

// 全局共享缓冲，所有类/线程直接访问，不需要 LCANBusFXYNEW 实例
class CanBufferMgr
{
public:
    static CanBufferMgr* GetInstance()
    {
        static CanBufferMgr inst;
        return &inst;
    }
    CanRingBuffer& GetTxBuf() { return m_txBuf; }
    CanRingBuffer& GetRxBuf() { return m_rxBuf; }
    void ClearAll() { m_txBuf.clear(); m_rxBuf.clear(); }

private:
    CanBufferMgr() = default;
    ~CanBufferMgr() = default;
    CanBufferMgr(const CanBufferMgr&) = delete;
    CanBufferMgr& operator=(const CanBufferMgr&) = delete;
    CanRingBuffer m_txBuf;
    CanRingBuffer m_rxBuf;
};
