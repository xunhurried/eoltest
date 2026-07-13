#include "jsonmerge.h"
#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QString>

QString getDeviceName() {
    return QString::fromLocal8Bit(qgetenv("COMPUTERNAME"));
}

// 获取所有网络接口
QList<QNetworkInterface> getAllInterfaces() {
    return QNetworkInterface::allInterfaces();
}

// 获取特定网络接口的IP地址
QHostAddress getIPAddress(QNetworkInterface interface) {
    foreach (QNetworkAddressEntry entry, interface.addressEntries()) {
        if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
            return entry.ip();
        }
    }
    return QHostAddress();
}


JsonMerge::JsonMerge()
{
    initJson();
}

void JsonMerge::initJson()
{
    QString deviceName = getDeviceName();
    errorString.clear();
    jsonData = QJsonObject();
    jsonData.insert("LotSN", "untested");
    jsonData.insert("FactoryLoadNumber", "1");
    jsonData.insert("ResourceName", deviceName);
    jsonData.insert("FrockNumber", "untested");
    QList<QNetworkInterface> interfaces = getAllInterfaces();
    foreach (QNetworkInterface interface, interfaces) {
        QHostAddress ip = getIPAddress(interface);
        if (!ip.isNull() && ip.toString().contains("10.")) {
            jsonData.insert("IP", ip.toString());;
        }
    }
    jsonData.insert("TerminatingResistor", "untested");
    jsonData.insert("TestResult", "untested");
    jsonData.insert("TestType", "untested");
    jsonData.insert("DeviceType", "untested");
    jsonData.insert("Date", "untested");
    jsonData.insert("TestTime", "untested");
    jsonData.insert("MACNumber", "untested");
    jsonData.insert("Tester", "untested");
    jsonData.insert("HardwareVersion", "untested");
    jsonData.insert("SoftwareVersion", "untested");

    QJsonObject BatteryVoltage;
    BatteryVoltage.insert("standard", "untested");
    BatteryVoltage.insert("test", "untested");
    BatteryVoltage.insert("difference", "untested");
    BatteryVoltage.insert("allowable", "untested");
    BatteryVoltage.insert("result", "untested");
    jsonData.insert("BatteryVoltage", BatteryVoltage);

    QJsonObject PrechargeVoltage;
    PrechargeVoltage.insert("standard", "untested");
    PrechargeVoltage.insert("test", "untested");
    PrechargeVoltage.insert("difference", "untested");
    PrechargeVoltage.insert("allowable", "untested");
    PrechargeVoltage.insert("result", "untested");
    jsonData.insert("PrechargeVoltage", PrechargeVoltage);

    QJsonObject SerialCommunication;
    SerialCommunication.insert("standard", "untested");
    SerialCommunication.insert("test", "untested");
    SerialCommunication.insert("difference", "untested");
    SerialCommunication.insert("allowable", "untested");
    SerialCommunication.insert("result", "untested");
    jsonData.insert("SerialCommunication", SerialCommunication);

    QJsonObject CPCC2;
    CPCC2.insert("CPstandard", "untested");
    CPCC2.insert("CPtest", "untested");
    CPCC2.insert("CC2standard", "untested");
    CPCC2.insert("CC2test", "untested");
    CPCC2.insert("result", "untested");
    jsonData.insert("CPCC2", CPCC2);

    QJsonObject KBStatus;
    KBStatus.insert("preChargeK", "untested");
    KBStatus.insert("preChargeB", "untested");
    KBStatus.insert("endK", "untested");
    KBStatus.insert("endB", "untested");
    KBStatus.insert("result", "untested");
    jsonData.insert("KBStatus", KBStatus);

    QJsonObject LowPower;
    LowPower.insert("standard", "untested");
    LowPower.insert("test", "untested");
    LowPower.insert("difference", "untested");
    LowPower.insert("allowable", "untested");
    LowPower.insert("result", "untested");
    jsonData.insert("LowPower", LowPower);

    QJsonObject WatchDog;
    WatchDog.insert("before", "untested");
    WatchDog.insert("after", "untested");
    WatchDog.insert("difference", "untested");
    WatchDog.insert("allowable", "untested");
    WatchDog.insert("result", "untested");
    jsonData.insert("WatchDog", WatchDog);

    QJsonObject Storage;
    Storage.insert("before", "untested");
    Storage.insert("after", "untested");
    Storage.insert("difference", "untested");
    Storage.insert("allowable", "untested");
    Storage.insert("result", "untested");
    jsonData.insert("Storage", Storage);

    QJsonObject HALLCurrent;
    QJsonObject OneZheng;
    OneZheng.insert("standard", "untested");
    OneZheng.insert("test", "untested");
    OneZheng.insert("difference", "untested");
    OneZheng.insert("allowable", "untested");
    OneZheng.insert("result", "untested");
    HALLCurrent.insert("OneZheng", OneZheng);
    QJsonObject OneFu;
    OneFu.insert("standard", "untested");
    OneFu.insert("test", "untested");
    OneFu.insert("difference", "untested");
    OneFu.insert("allowable", "untested");
    OneFu.insert("result", "untested");
    HALLCurrent.insert("OneFu", OneFu);
    QJsonObject TwoZheng;
    TwoZheng.insert("standard", "untested");
    TwoZheng.insert("test", "untested");
    TwoZheng.insert("difference", "untested");
    TwoZheng.insert("allowable", "untested");
    TwoZheng.insert("result", "untested");
    HALLCurrent.insert("TwoZheng", TwoZheng);
    QJsonObject TwoFu;
    TwoFu.insert("standard", "untested");
    TwoFu.insert("test", "untested");
    TwoFu.insert("difference", "untested");
    TwoFu.insert("allowable", "untested");
    TwoFu.insert("result", "untested");
    HALLCurrent.insert("TwoFu", TwoFu);
    QJsonObject ThreeZheng;
    ThreeZheng.insert("standard", "untested");
    ThreeZheng.insert("test", "untested");
    ThreeZheng.insert("difference", "untested");
    ThreeZheng.insert("allowable", "untested");
    ThreeZheng.insert("result", "untested");
    HALLCurrent.insert("ThreeZheng", ThreeZheng);
    QJsonObject ThreeFu;
    ThreeFu.insert("standard", "untested");
    ThreeFu.insert("test", "untested");
    ThreeFu.insert("difference", "untested");
    ThreeFu.insert("allowable", "untested");
    ThreeFu.insert("result", "untested");
    HALLCurrent.insert("ThreeFu", ThreeFu);
    jsonData.insert("HALLCurrent", HALLCurrent);

    QJsonObject SupplyVoltage;
    SupplyVoltage.insert("standard", "untested");
    SupplyVoltage.insert("test", "untested");
    SupplyVoltage.insert("difference", "untested");
    SupplyVoltage.insert("allowable", "untested");
    SupplyVoltage.insert("result", "untested");
    jsonData.insert("SupplyVoltage", SupplyVoltage);

    QJsonObject Power;
    Power.insert("standard", "untested");
    Power.insert("test", "untested");
    Power.insert("difference", "untested");
    Power.insert("allowable", "untested");
    Power.insert("result", "untested");
    jsonData.insert("Power", Power);

    QJsonObject HALLSignal;
    QJsonObject Five;
    Five.insert("standard", "untested");
    Five.insert("test", "untested");
    Five.insert("difference", "untested");
    Five.insert("allowable", "untested");
    Five.insert("result", "untested");
    HALLSignal.insert("Five", Five);
    QJsonObject Twelve;
    Twelve.insert("standard", "untested");
    Twelve.insert("test", "untested");
    Twelve.insert("difference", "untested");
    Twelve.insert("allowable", "untested");
    Twelve.insert("result", "untested");
    HALLSignal.insert("Twelve", Twelve);
    jsonData.insert("HALLSignal", HALLSignal);

    QJsonObject Temperature;
    QJsonObject One;
    One.insert("standard", "untested");
    One.insert("test", "untested");
    One.insert("difference", "untested");
    One.insert("allowable", "untested");
    One.insert("result", "untested");
    Temperature.insert("One", One);
    QJsonObject Two;
    Two.insert("standard", "untested");
    Two.insert("test", "untested");
    Two.insert("difference", "untested");
    Two.insert("allowable", "untested");
    Two.insert("result", "untested");
    Temperature.insert("Two", Two);
    QJsonObject Three;
    Three.insert("standard", "untested");
    Three.insert("test", "untested");
    Three.insert("difference", "untested");
    Three.insert("allowable", "untested");
    Three.insert("result", "untested");
    Temperature.insert("Three", Three);
    QJsonObject Four;
    Four.insert("standard", "untested");
    Four.insert("test", "untested");
    Four.insert("difference", "untested");
    Four.insert("allowable", "untested");
    Four.insert("result", "untested");
    Temperature.insert("Four", Four);
    jsonData.insert("Temperature", Temperature);

    QJsonObject EncodeInterface;
    EncodeInterface.insert("standard", "untested");
    EncodeInterface.insert("test", "untested");
    EncodeInterface.insert("difference", "untested");
    EncodeInterface.insert("allowable", "untested");
    EncodeInterface.insert("result", "untested");
    jsonData.insert("EncodeInterface", EncodeInterface);

    QJsonObject DOTest;
    DOTest.insert("DO10", "untested");
    DOTest.insert("DO11", "untested");
    DOTest.insert("DO20", "untested");
    DOTest.insert("DO21", "untested");
    DOTest.insert("DO30", "untested");
    DOTest.insert("DO31", "untested");
    DOTest.insert("DO40", "untested");
    DOTest.insert("DO41", "untested");
    DOTest.insert("DO50", "untested");
    DOTest.insert("DO51", "untested");
    DOTest.insert("DO60", "untested");
    DOTest.insert("DO61", "untested");
    DOTest.insert("DO70", "untested");
    DOTest.insert("DO71", "untested");
    DOTest.insert("DO80", "untested");
    DOTest.insert("DO81", "untested");
    jsonData.insert("DOTest", DOTest);


    QJsonObject DITest;
    DITest.insert("DI10", "untested");
    DITest.insert("DI11", "untested");
    DITest.insert("DI20", "untested");
    DITest.insert("DI21", "untested");
    DITest.insert("DI30", "untested");
    DITest.insert("DI31", "untested");
    DITest.insert("DI40", "untested");
    DITest.insert("DI41", "untested");
    DITest.insert("DI50", "untested");
    DITest.insert("DI51", "untested");
    DITest.insert("DI60", "untested");
    DITest.insert("DI61", "untested");
    DITest.insert("DI70", "untested");
    DITest.insert("DI71", "untested");
    DITest.insert("DI80", "untested");
    DITest.insert("DI81", "untested");
    jsonData.insert("DITest", DITest);

    QJsonObject CANCommunication;
    CANCommunication.insert("CAN0", "untested");
    CANCommunication.insert("CAN1", "untested");
    CANCommunication.insert("CAN2", "untested");
    jsonData.insert("CANCommunication", CANCommunication);

    QJsonObject EEPROM;
    EEPROM.insert("standard", "untested");
    EEPROM.insert("test", "untested");
    EEPROM.insert("difference", "untested");
    EEPROM.insert("allowable", "untested");
    EEPROM.insert("result", "untested");
    jsonData.insert("EEPROM", EEPROM);

    QJsonObject Flash1;
    Flash1.insert("standard", "untested");
    Flash1.insert("test", "untested");
    Flash1.insert("difference", "untested");
    Flash1.insert("allowable", "untested");
    Flash1.insert("result", "untested");
    jsonData.insert("Flash1", Flash1);

    QJsonObject Flash2;
    Flash2.insert("standard", "untested");
    Flash2.insert("test", "untested");
    Flash2.insert("difference", "untested");
    Flash2.insert("allowable", "untested");
    Flash2.insert("result", "untested");
    jsonData.insert("Flash2", Flash2);

    QJsonObject RTC;
    RTC.insert("standard", "untested");
    RTC.insert("test", "untested");
    RTC.insert("difference", "untested");
    RTC.insert("allowable", "untested");
    RTC.insert("result", "untested");
    jsonData.insert("RTC", RTC);

    QJsonObject SystemTemperature;
    SystemTemperature.insert("standard", "untested");
    SystemTemperature.insert("test", "untested");
    SystemTemperature.insert("difference", "untested");
    SystemTemperature.insert("allowable", "untested");
    SystemTemperature.insert("result", "untested");
    jsonData.insert("SystemTemperature", SystemTemperature);

    QJsonObject Internet;
    QJsonObject InternetOne;
    InternetOne.insert("IP", "untested");
    InternetOne.insert("result", "untested");
    QJsonObject InternetTwo;
    InternetTwo.insert("IP", "untested");
    InternetTwo.insert("result", "untested");
    Internet.insert("InternetOne", InternetOne);
    Internet.insert("InternetTwo", InternetTwo);
    jsonData.insert("Internet", Internet);

    QJsonObject InsulationResistance;
    InsulationResistance.insert("standard", "untested");
    InsulationResistance.insert("test", "untested");
    InsulationResistance.insert("difference", "untested");
    InsulationResistance.insert("allowable", "untested");
    InsulationResistance.insert("result", "untested");
    jsonData.insert("InsulationResistance", InsulationResistance);

    QJsonObject SoftwareVersionTest;
    SoftwareVersionTest.insert("standard", "untested");
    SoftwareVersionTest.insert("test", "untested");
    SoftwareVersionTest.insert("difference", "untested");
    SoftwareVersionTest.insert("allowable", "untested");
    SoftwareVersionTest.insert("result", "untested");
    jsonData.insert("SoftwareVersionTest", SoftwareVersionTest);

    QJsonObject HardwareVersionTest;
    HardwareVersionTest.insert("standard", "untested");
    HardwareVersionTest.insert("test", "untested");
    HardwareVersionTest.insert("difference", "untested");
    HardwareVersionTest.insert("allowable", "untested");
    HardwareVersionTest.insert("result", "untested");
    jsonData.insert("HardwareVersionTest", HardwareVersionTest);

    QJsonObject SPI;
    SPI.insert("standard", "untested");
    SPI.insert("test", "untested");
    SPI.insert("difference", "untested");
    SPI.insert("allowable", "untested");
    SPI.insert("result", "untested");
    jsonData.insert("SPI", SPI);

    QJsonObject SetMAC;
    SetMAC.insert("standard", "untested");
    SetMAC.insert("test", "untested");
    SetMAC.insert("difference", "untested");
    SetMAC.insert("allowable", "untested");
    SetMAC.insert("result", "untested");
    jsonData.insert("SetMAC", SetMAC);

    QJsonObject MAC;
    MAC.insert("standard", "untested");
    MAC.insert("test", "untested");
    MAC.insert("difference", "untested");
    MAC.insert("allowable", "untested");
    MAC.insert("result", "untested");
    jsonData.insert("MAC", MAC);

    QJsonObject SetSerial;
    SetSerial.insert("standard", "untested");
    SetSerial.insert("test", "untested");
    SetSerial.insert("difference", "untested");
    SetSerial.insert("allowable", "untested");
    SetSerial.insert("result", "untested");
    jsonData.insert("SetSerial", SetSerial);

    QJsonObject Serial;
    Serial.insert("standard", "untested");
    Serial.insert("test", "untested");
    Serial.insert("difference", "untested");
    Serial.insert("allowable", "untested");
    Serial.insert("result", "untested");
    jsonData.insert("Serial", Serial);
    //qDebug() << jsonData;
}

void JsonMerge::slotReceiveResult(QJsonObject results)
{
    QStringList list = results.keys();
    for(auto result : list)
    {
        jsonData.remove(result);
        jsonData.insert(result, results.value(result));
    }
}

void JsonMerge::slotReceiveTestResult(QJsonObject results)
{
    QStringList list = results.keys();
    for(auto result : list)
    {
        jsonData.remove(result);
        jsonData.insert(result, results.value(result));
    }
    if(jsonData.value("TestResult") != "untested")
    {
        RequestData data;
        data.json = jsonData;
        emit singalSendData(data);
        //qDebug() << "result is " << jsonData;
        initJson();
    }
    else
    {
        qDebug() << "result is " << jsonData;
    }
//    RequestData data;
//    data.json = jsonData;
//    emit singalSendData(data);
}
