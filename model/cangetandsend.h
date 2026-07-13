#ifndef CANGETANDSEND_H
#define CANGETANDSEND_H

#include <QObject>
#include <QJsonObject>
#include "testitemenums.h"
#include "timer.h"
#include "controller/excelcontrol.h"
#include "ping.h"
#include "model/canbusgc.h"
#include "model/canbuszlg.h"
#include "model/canbusfxynew.h"
#include "model/canbusfxyold.h"
#include "model/testitemenums.h"
#include "model/hallcurrentvector.h"
#include "model/rs485parsethread.h"

class CANGetAndSend: public QObject
{
    Q_OBJECT
public:
    CANGetAndSend(int type, int locate);

private:
    void TransmitCAN(int ID, unsigned char data[8]);
    void unsigned_to_hex(unsigned int value, std::string& hex_string);
    void openDO7();
    void closeDO7();
    void cancelPendingAsyncCallbacks();

private:
    int m_type = 0;
    TestItem lastTestType = UnknownType;
    QByteArray currentData;
    QString IpAddress;
    int step = 0;
    Ping *ping;
    int temperature = 0;
    std::vector<ExcelItem> items;
    QStringList m_macList;
    QString m_serial;
    QString serial_receive;
    QString m_software;
    QString m_hardware;
    QTimer *testTimer = NULL;
    bool ifCanPass = true;
    bool ifVPass = true;
    bool ifRPass = true;
    bool ifCPass = true;
    QByteArray receiveArray;
    QByteArray hardwareArray;
    QByteArray nameArray;
    int hallTime = 0;
    int maxReadTime = 0;
    float m_vg = 0.0;
    float m_vc = 0.0;
    float m_gzvz1 = 0.0;
    float m_gzvz2 = 0.0;
    float m_gzvz3 = 0.0;
    float m_gzvy1 = 0.0;
    float m_gzvy2 = 0.0;
    float m_gzvy3 = 0.0;
    float m_dcvz1 = 0.0;
    float m_dcvz2 = 0.0;
    float m_dcvz3 = 0.0;
    float m_dcvy1 = 0.0;
    float m_dcvy2 = 0.0;
    float m_dcvy3 = 0.0;
    float m_dcgd = 0.0;
    float m_gzgd = 0.0;
    float m_dcjyz = 0.0;
    float m_dcjyf = 0.0;
    float m_temp1 = 0.0;
    float m_temp2 = 0.0;
    float m_temp3 = 0.0;
    float m_temp4 = 0.0;
    float m_dctemp = 0.0;
    float m_gztemp = 0.0;

    float m_gzaz11 = 0.0;
    float m_gzaz21 = 0.0;
    float m_gzaz31 = 0.0;
    float m_gzaf11 = 0.0;
    float m_gzaf21 = 0.0;
    float m_gzaf31 = 0.0;
    float m_dcaz11 = 0.0;
    float m_dcaz21 = 0.0;
    float m_dcaz31 = 0.0;
    float m_dcaf11 = 0.0;
    float m_dcaf21 = 0.0;
    float m_dcaf31 = 0.0;

    float m_gzaz12 = 0.0;
    float m_gzaz22 = 0.0;
    float m_gzaz32 = 0.0;
    float m_gzaf12 = 0.0;
    float m_gzaf22 = 0.0;
    float m_gzaf32 = 0.0;
    float m_dcaz12 = 0.0;
    float m_dcaz22 = 0.0;
    float m_dcaz32 = 0.0;
    float m_dcaf12 = 0.0;
    float m_dcaf22 = 0.0;
    float m_dcaf32 = 0.0;

    float m_gzaz13 = 0.0;
    float m_gzaz23 = 0.0;
    float m_gzaz33 = 0.0;
    float m_gzaf13 = 0.0;
    float m_gzaf23 = 0.0;
    float m_gzaf33 = 0.0;
    float m_dcaz13 = 0.0;
    float m_dcaz23 = 0.0;
    float m_dcaz33 = 0.0;
    float m_dcaf13 = 0.0;
    float m_dcaf23 = 0.0;
    float m_dcaf33 = 0.0;
    float a = 0.0;
    int m_info = 0;
    int m_hallReadTime = 0;
    QTimer *failTimer = NULL;
    int canTime = 0;
    bool if_mes = false;
    QJsonObject testResults;
    QJsonObject HALLSignal;
    QJsonObject HALLCurrent;
    QJsonObject SystemTemperature;
    QJsonObject DOTest;
    QJsonObject DITest;
    QJsonObject CANCommunication;
    QJsonObject Temperature;
    QJsonObject WatchDog;
    QJsonObject Storage;
    QJsonObject CPCC2;
    QString m_tempSoft;
    QJsonObject KBStatus;
    QJsonObject ZDResistance;
    int m_resz = 0;
    int m_Storage = 0;
    bool ifHallPass = true;
    QTimer *retryTimer;
    bool cp = true;
    int watchDog = 0;
    int retryNumber = 0;
    bool ifSerialPass = true;
    int serialTime = 0;
    bool ifSetSuccess = true;
    float m_current1 = 0.0;
    float m_current2 = 0.0;
    float m_current3 = 0.0;
    float m_current4 = 0.0;
    float m_current5 = 0.0;
    float m_current6 = 0.0;
    float m_current7 = 0.0;
    float m_current8 = 0.0;
    float m_current9 = 0.0;
    float m_current10 = 0.0;
    float m_current11 = 0.0;
    float m_current12 = 0.0;
    float m_current13 = 0.0;
    float m_current14 = 0.0;
    float m_current15 = 0.0;
    float m_current16 = 0.0;
    float m_current = 0.0;
    int m_locate = 0;
    QTimer *setFailTimer;
    QTimer timeoutTimer;
    bool if_zdPass = true;

    int voltageTime = 10000;
    int currentTime = 5000;

public slots:
    void slotReceiveData(VCI_CAN_OBJ pReceive);
    void slotReceiveData(CAN_OBJ pReceive);
    void slotReceiveData2(VCI_CAN_OBJ pReceive);
    void slotReceiveData2(CAN_OBJ pReceive);
    void slotRetrySend();
    void slotPingSuccess();
    void slotPingFail(int time);
    void slotCanTimeout();
    void updateTestItem();
    void slotSetCurrentTestStep(int type);
    //void slotReadKB(int type);
    void slotSetKB(int type, float k, float b);
    void slotReadInfo(int type);
    void slotTimeout();
    void slotCurrentTimeout();
    void slotSetSoft(QString id);
    void slotSetHard(QString id);
    void slotVolOut();
    void slotMACTimeout();
    void slotSetMod(bool mod);
    void slotSleepTimeout();
    void slotStorageTimeout();
    void slotCPTimeout();
    void slotWatchDogTimeout();
    void slotSendSuccess();
    void slotSendFail();
    void slotSetTestMod();
    void slotSetFail();
    void slotSetMod();
    void slotErrorFinish();
    void slotFinish();
    void slotReadVolt();
    void slotSetGYY();
    void slotReceive485Data(QByteArray data);
    void slotTestE2();
    void slotChange(int type);
    void slotReceiveSerial(QString serial);
    void slotTestInternet();
    void startTimerSlot(int time);
    void stopTimerSlot();
    void startFailTimerSlot(int time);
    void stopFailTimerSlot();
    void slotResendMod();
    void slotIfCanUpdate(bool status);
    void slotCheckIfUpdate();
    void slotTimeOut();

signals:
    void signalTransmitCAN(PVCI_CAN_OBJ pSend, ULONG Len);
    void signalTransmitCAN(CAN_OBJ pSend, ULONG Len);
    void signalReceiveStatus(const QString &);
    void signalReceiveErrorStatus(const QString &);
    void singalReceiveMAC(const QString &);
    void signalFinish();
    void signalErrorFinish();
    void signalSetExcelItem(std::vector<ExcelItem> items);
    void signalReceiveSerial(const QString &);
    void signalGetKB(float, float);
    void signalGetInfo(int, float);
    void signalSendTestResult(QJsonObject);
    void signalEEPROMFinish();
    void signalEEPROMErrorFinish();
    void signalFlashFinish();
    void signalFlashErrorFinish();
    void signalRTCFinish();
    void signalRTCErrorFinish();
    void signalIOErrorFinish();
    void signalIOFinish();
    void signalDIErrorFinish();
    void signalDOErrorFinish();
    void signalDIFinish();
    void signalDOFinish();
    void signalCANFinish();
    void signalCANErrorFinish();
    void signalSerialFinish();
    void signalSerialErrorFinish();
    void signalHALLCurrentFinish();
    void signalHALLCurrentErrorFinish();
    void signalBatteryVoltageFinish();
    void signalBatteryVoltageErrorFinish();
    void signalSupplyVoltageFinish();
    void signalSupplyVoltageErrorFinish();
    void signalSystemTemperatureFinish();
    void signalSystemTemperatureErrorFinish();
    void signalInsulationResistanceFinish();
    void signalInsulationResistanceErrorFinish();
    void signalPowerFinish();
    void signalPowerErrorFinish();
    void signalHALLSignalFinish();
    void signalHALLSignalErrorFinish();
    void signalTemperatureFinish();
    void signalTemperatureErrorFinish();
    void signalFinishSet();
    void signalFinishSet2();
    void signalSetModFail();
    void signalSetModFail2();
    void signalChangePort(int);
    void signalSerialError();
    void signalStartTest();
    void signalSetSerial(QString);
    void signalSetMAC(QString);
    void startTimerSignal(int);
    void stopTimerSignal();
    void startFailTimerSignal(int);
    void stopFailTimerSignal();
    void signalCheckIfCanUpdate();
    void signalSetUpdateStatus(bool);
    void signalTimeout();
};

#endif // CANGETANDSEND_H
