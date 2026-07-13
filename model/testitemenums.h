#ifndef TESTITEMENUMS_H
#define TESTITEMENUMS_H

enum TestItem
{
    StartTest                     = 1,
    TestSetBatteryVoltage         = 2,
    TestSetHALLCurrent            = 3,
    TestSetRTC                    = 4,
    TestSetMAC                    = 5,
    TestSetSerial                 = 6,
    TestZDResistance              = 7,
    TestBatteryVoltage            = 8,
    TestHALLCurrent               = 9,
    TestSupplyVoltage             = 10,
    TestPower                     = 11,
    TestHALLSignal                = 12,
    TestTemperature               = 13,
    TestEncodeInterface           = 14,
    TestDO                        = 15,
    TestDIAndSLP                  = 16,
    TestSerialCommunication       = 17,
    TestCANCommunication          = 18,
    TestEEPROM                    = 19,
    TestFlash                     = 20,
    TestRTC                       = 21,
    TestSystemTemperature         = 22,
    TestReadMAC                   = 23,
    TestReadSerial                = 24,
    TestInternet                  = 25,
    TestInsulationResistance      = 26,
    TestSoftWareVersion           = 27,
    TestHardWareVersion           = 28,
    TestSPI                       = 29,
    TestKB                        = 30,
    FinishTest                    = 31,
    ExitTest                      = 32,
    UnknownType                   = 99
};

#endif // TESTITEMENUMS_H
