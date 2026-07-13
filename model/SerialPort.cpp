#include "SerialPort.hpp"
#include <QDebug>
#include <QByteArray>
#include <QTimer>
#include <QThread>
#include <QDateTime>

namespace
{
QString serialHexPreview(const QByteArray &data, int maxBytes = 32)
{
    if (data.isEmpty())
    {
        return QStringLiteral("<empty>");
    }

    const QByteArray preview = data.left(maxBytes).toHex();
    if (data.size() <= maxBytes)
    {
        return QString::fromLatin1(preview);
    }
    return QString::fromLatin1(preview) + QStringLiteral("...");
}
}

SerialPort::SerialPort(const char *portName, int port)
    : QObject(nullptr), readTimer(nullptr), connected(false), handler(INVALID_HANDLE_VALUE)
{
    // 串口打开逻辑不变
    handler = CreateFileA(static_cast<LPCSTR>(portName),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (handler != INVALID_HANDLE_VALUE)
    {
        DCB dcbSerialParameters = {0};
        if (GetCommState(handler, &dcbSerialParameters))
        {
            dcbSerialParameters.BaudRate = port;
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.StopBits = ONESTOPBIT;
            dcbSerialParameters.Parity = NOPARITY;
            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;
            SetCommState(handler, &dcbSerialParameters);
            connected = true;
            PurgeComm(handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
            Sleep(ARDUINO_WAIT_TIME);
        }
    }

    // 绑定当前SerialPort给内部发送轮询线程
    m_rs485PollThread.SetCtrl(this);
    // 串口打开成功，启动轮询线程持续读取全局Tx缓冲
    m_rs485PollThread.StartPoll();
}

void Rs485SendPollThread::run()
{
    auto pollLoop = [this]()
    {
        if (!m_ctrl) return;
        Rs485RingBuf& txBuf = Rs485BufferMgr::GetInstance()->GetTxBuf();
        Rs485Frame frame;

        while (m_running)
        {
            // 取出一条待发485指令
            if (txBuf.popNoBlock(frame))
            {
                frame.sendTick = QDateTime::currentMSecsSinceEpoch();
                // 下发给SerialPort，由SerialPort内部timer处理200ms等待应答
                emit m_ctrl->slotSendData(frame.sendData);
            }
            // 极小休眠，降低CPU空转，无应答等待延时
            QThread::usleep(50);
        }
        qDebug() << "485发送轮询线程正常退出";
    };
    pollLoop();
}

SerialPort::~SerialPort()
{
    // 先停止发送轮询线程
    m_rs485PollThread.StopPoll();

    if (readTimer)
    {
        readTimer->stop();
        delete readTimer;
        readTimer = nullptr;
    }
    if (connected && handler != INVALID_HANDLE_VALUE)
    {
        connected = false;
        CloseHandle(handler);
        handler = INVALID_HANDLE_VALUE;
    }
}

int SerialPort::readSerialPort(const char *buffer, unsigned int buf_size)
{
    if (!this->connected || this->handler == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    DWORD bytesRead{};
    unsigned int toRead = 0;

    ClearCommError(this->handler, &this->errors, &this->status);

    if (this->status.cbInQue > 0)
    {
        if (this->status.cbInQue > buf_size)
        {
            toRead = buf_size;
        }
        else
        {
            toRead = this->status.cbInQue;
        }
    }

    memset((void*) buffer, 0, buf_size);

    if (ReadFile(this->handler, (void*) buffer, toRead, &bytesRead, NULL))
    {
        if (bytesRead > 0)
        {
            const QByteArray readView(buffer, static_cast<int>(bytesRead));
        }
        return bytesRead;
    }
    return 0;
}

// Sending provided buffer to serial port;
// returns true if succeed, false if not
bool SerialPort::writeSerialPort(const char *buffer, unsigned int buf_size)
{
    if (!buffer || buf_size == 0 || !this->connected || this->handler == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    /* 1. 组包（你已有的逻辑） */
    QByteArray tempBuffer;
    if (m_mod == 0)
    {
        tempBuffer = QByteArray(buffer, buf_size);
        uint16_t crc = Modbus_CRC16(reinterpret_cast<uint8_t*>(tempBuffer.data()), buf_size);
        tempBuffer.append(static_cast<char>(crc >> 8))
                  .append(static_cast<char>(crc & 0xFF));
    }
    else
    {
        if (buf_size < 2)
        {
            return false;
        }
        tempBuffer.reserve(6 + buf_size + 3);
        static const char prefix[] = {char(0xEB),char(0x90),char(0xEB),char(0x90),char(0xEB),char(0x90)};
        tempBuffer.append(prefix, sizeof(prefix));
        tempBuffer.append(buffer, buf_size);
        uint16_t crc = CRC16(reinterpret_cast<uint8_t*>(tempBuffer.data() + sizeof(prefix)),
                             static_cast<UINT16>(buf_size));
        tempBuffer.append(static_cast<char>(crc >> 8))
                  .append(static_cast<char>(crc & 0xFF))
                  .append('\x7E');
    }

    /* 2. 强制深拷贝，断共享链，保证私有块 */

    QByteArray tmp = QByteArray(tempBuffer.constData(), tempBuffer.size());

    /* 3. 发给串口 */
    DWORD bytesSend;
    if (!WriteFile(handler, tmp.data(), tmp.size(), &bytesSend, nullptr))
    {
        ClearCommError(handler, &errors, &status);
        return false;
    }
    return true;
}
// Checking if serial port is connected
bool SerialPort::isConnected()
{
    if (!ClearCommError(this->handler, &this->errors, &this->status))
    {
        this->connected = false;
    }
    return this->connected;
}

void SerialPort::closeSerial()
{
    CloseHandle(this->handler);
}

uint16_t SerialPort::Modbus_CRC16(uint8_t *dat_buf, uint16_t dat_len)
{
    uint8_t j,CH,CL;
    uint16_t i;
    CH = 0xFF;
    CL = 0xFF;
    for(i = 0; i < dat_len; i++)
    {
        j = dat_buf[i];
        j = CH ^ j;
        CH = CL ^ AUCHCRCHI[j];
        CL = AUCHCRCLO[j];
    }
    return(CH * 0x100 + CL);
}

UINT16 SerialPort::CRC16(UINT8 *dat_buf, UINT16 dat_len)
{
    UINT8 j, r1, r2, r3, r4;
    UINT16 i;

    r1 = dat_buf[0];
    r2 = dat_buf[1];

    for (i = 2; i < dat_len + 2; i++)
    {
        if (i < dat_len)
        {
            r3 = dat_buf[i];
        }
        else
        {
            r3 = 0;
        }

        for (j = 0; j < 8; j++)
        {
            r4 = r1;
            r1 = (r1 << 1);
            if ((r2 & 0x80) != 0) r1++;
            r2 = r2 << 1;
            if ((r3 & 0x80) != 0) r2++;
            r3 = (r3 << 1);
            if ((r4 & 0x80) != 0)
            {
                r1 = r1 ^ 0x10;
                r2 = r2 ^ 0x21;
            }
        }
    }
    return (r1 * 0x100 + r2);
}

void SerialPort::startRead()
{
    if (!this->connected || this->handler == INVALID_HANDLE_VALUE)
    {
        //qDebug() << "[SERIAL] skip startRead because port inactive";
        return;
    }

    receiveData.clear();
    receiveData.resize(4096);
    if(!readTimer)
    {
        readTimer = new QTimer(this);
        readTimer->setSingleShot(false);
        connect(readTimer, SIGNAL(timeout()), this, SLOT(slotTryRead()));
    }
    else
    {
        if(readTimer->isActive())
        {
            readTimer->stop();
        }
    }
    readTimer->setSingleShot(true);
    readTimer->start(200);
}


void SerialPort::slotSendData(QByteArray data)
{
    if (!this->connected || this->handler == INVALID_HANDLE_VALUE)
    {
        return;
    }
    QByteArray tmp = QByteArray(data.constData(), data.size());
    resendData = QByteArray(data.constData(), data.size());
    if(writeSerialPort(tmp.data(), tmp.size()))
    {
        startRead();
    }
    else
    {
    }
}

void SerialPort::slotTryRead()
{
    int size = readSerialPort(receiveData.data(), 4096);
    if(size != 0)
    {
        receiveData.resize(size);
        if(receiveData.size() >= 3)
        {
            QByteArray tempArray;
            tempArray.append(receiveData.at(2));
            int lenth = tempArray.toHex().toInt(nullptr, 16);
            if(receiveData.size() > lenth)
            {
                if(receiveData.at(0) == 3 || receiveData.at(0) == 4 || receiveData.at(0) == 7
                         || receiveData.at(0) == 8 || receiveData.at(0) == 11 || receiveData.at(0) == 12
                         || receiveData.at(0) == 15 || receiveData.at(0) == 16)
                {
                    QByteArray tmp = QByteArray(receiveData.constData(), receiveData.size());
                    Rs485Frame recvFrame;
                    recvFrame.sendData = tmp;
                    recvFrame.sendTick = QDateTime::currentMSecsSinceEpoch();
                    Rs485BufferMgr::GetInstance()->GetRxBuf().push(recvFrame);
                }
            }
            else
            {
                if(receiveData.at(0) == 3 || receiveData.at(0) == 4 || receiveData.at(0) == 7
                         || receiveData.at(0) == 8 || receiveData.at(0) == 11 || receiveData.at(0) == 12
                         || receiveData.at(0) == 15 || receiveData.at(0) == 16)
                {
                    startRead();
                }
            }
        }
        else
        {
            if(receiveData.size() > 0 && (receiveData.at(0) == 3 || receiveData.at(0) == 4 || receiveData.at(0) == 7
                     || receiveData.at(0) == 8 || receiveData.at(0) == 11 || receiveData.at(0) == 12
                     || receiveData.at(0) == 15 || receiveData.at(0) == 16))
            {
                startRead();
            }
        }
    }
    else
    {
        if(!resendData.isEmpty() && writeSerialPort(resendData.data(), resendData.size()))
        {
            startRead();
        }
        else
        {
        }
    }
}

void SerialPort::slotSetMod(int mod)
{
    m_mod = mod;
}

void SerialPort::slotStop()
{
    if (readTimer && readTimer->isActive())
    {
        readTimer->stop();
    }
    resendData.clear();
    receiveData.clear();

    if (this->connected && this->handler != INVALID_HANDLE_VALUE)
    {
        this->connected = false;
        CloseHandle(this->handler);
        this->handler = INVALID_HANDLE_VALUE;
    }
}
