#include "maincontrol.h"
#include "canbuscontrol.h"
#include "view/mainwindow.h"
#include "view/choosedevicedialog.h"
#include "view/choosetypedialog.h"
#include <QDebug>
#include <QVector>
#include <QState>
#include <QApplication>
#include <QDateTime>
#include <QThread>
#include <QTimer>

MainControl::MainControl() : canbusControl(NULL), mainWindow(NULL),
    chooseDeviceWidget(NULL),
    ifContinue(true), typeDialog(NULL), canbusThread(NULL)
{
    qRegisterMetaType<PVCI_CAN_OBJ>("PVCI_CAN_OBJ");
    qRegisterMetaType<VCI_CAN_OBJ>("VCI_CAN_OBJ");
    qRegisterMetaType<CAN_OBJ>("CAN_OBJ");
    qRegisterMetaType<ULONG>("ULONG");
    qRegisterMetaType<CANFXYOLDRETURNCODE>("CANFXYOLDRETURNCODE");
    qRegisterMetaType<CANFXYNEWRETURNCODE>("CANFXYNEWRETURNCODE");
    qRegisterMetaType<CANZLGRETURNCODE>("CANZLGRETURNCODE");
    qRegisterMetaType<CANGCRETURNCODE>("CANGCRETURNCODE");
    qRegisterMetaType<const unsigned char *>("const unsigned char *");
    qRegisterMetaType<std::vector<ExcelItem>>("std::vector<ExcelItem>");
    qRegisterMetaType<std::vector<int>>("std::vector<int>");
    qRegisterMetaType<QJsonObject>("QJsonObject");
    layoutUI();
    connectFunc();
}

MainControl::~MainControl()
{
    if(canbusThread)
    {
        if(canbusControl)
        {
            delete canbusControl;
            canbusControl = NULL;
        }
        canbusThread->exit();
        delete canbusThread;
        canbusThread = NULL;
    }
    if(excelControl1)
    {
        delete excelControl1;
        excelControl1 = NULL;
    }
    if(mainWindow)
    {
        delete mainWindow;
        mainWindow = NULL;
    }
    if(chooseDeviceWidget)
    {
        delete chooseDeviceWidget;
        chooseDeviceWidget = NULL;
    }
}

void MainControl::layoutUI()
{
    m_stateMachine1 = new QStateMachine(this);
    m_stateMachine2 = new QStateMachine(this);
    m_stateMachine3 = new QStateMachine(this);
    m_stateMachine4 = new QStateMachine(this);
    m_stateMachine5 = new QStateMachine(this);
    m_stateMachine6 = new QStateMachine(this);
    m_stateMachine7 = new QStateMachine(this);
    m_stateMachine8 = new QStateMachine(this);
    m_stateMachine9 = new QStateMachine(this);
    m_stateMachine10 = new QStateMachine(this);
    m_stateMachine11 = new QStateMachine(this);
    m_stateMachine12 = new QStateMachine(this);
    m_stateMachine13 = new QStateMachine(this);
    m_stateMachine14 = new QStateMachine(this);
    m_stateMachine15 = new QStateMachine(this);
    m_stateMachine16 = new QStateMachine(this);
    mainWindow = new MainWindow();
}

void MainControl::connectFunc()
{
    connect(mainWindow, SIGNAL(signalOpenConnectDevicePage()), this, SLOT(slotOpenConnPage()));
    connect(mainWindow, SIGNAL(signalCloseCAN()), this, SLOT(slotCloseCANPort()));
    connect(mainWindow, SIGNAL(signalIfContuine(int)), this, SLOT(slotChangeContinue(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates1(int)), this, SLOT(slotTryUpadateStates1(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates2(int)), this, SLOT(slotTryUpadateStates2(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates3(int)), this, SLOT(slotTryUpadateStates3(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates4(int)), this, SLOT(slotTryUpadateStates4(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates5(int)), this, SLOT(slotTryUpadateStates5(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates6(int)), this, SLOT(slotTryUpadateStates6(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates7(int)), this, SLOT(slotTryUpadateStates7(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates8(int)), this, SLOT(slotTryUpadateStates8(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates9(int)), this, SLOT(slotTryUpadateStates9(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates10(int)), this, SLOT(slotTryUpadateStates10(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates11(int)), this, SLOT(slotTryUpadateStates11(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates12(int)), this, SLOT(slotTryUpadateStates12(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates13(int)), this, SLOT(slotTryUpadateStates13(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates14(int)), this, SLOT(slotTryUpadateStates14(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates15(int)), this, SLOT(slotTryUpadateStates15(int)));
    connect(mainWindow, SIGNAL(signalTryUpadateStates16(int)), this, SLOT(slotTryUpadateStates16(int)));
    connect(mainWindow, SIGNAL(signalStartTest1()), this, SLOT(slotStartTest1()));
    connect(mainWindow, SIGNAL(signalStartTest2()), this, SLOT(slotStartTest2()));
    connect(mainWindow, SIGNAL(signalStartTest3()), this, SLOT(slotStartTest3()));
    connect(mainWindow, SIGNAL(signalStartTest4()), this, SLOT(slotStartTest4()));
    connect(mainWindow, SIGNAL(signalStartTest5()), this, SLOT(slotStartTest5()));
    connect(mainWindow, SIGNAL(signalStartTest6()), this, SLOT(slotStartTest6()));
    connect(mainWindow, SIGNAL(signalStartTest7()), this, SLOT(slotStartTest7()));
    connect(mainWindow, SIGNAL(signalStartTest8()), this, SLOT(slotStartTest8()));
    connect(mainWindow, SIGNAL(signalStartTest9()), this, SLOT(slotStartTest9()));
    connect(mainWindow, SIGNAL(signalStartTest10()), this, SLOT(slotStartTest10()));
    connect(mainWindow, SIGNAL(signalStartTest11()), this, SLOT(slotStartTest11()));
    connect(mainWindow, SIGNAL(signalStartTest12()), this, SLOT(slotStartTest12()));
    connect(mainWindow, SIGNAL(signalStartTest13()), this, SLOT(slotStartTest13()));
    connect(mainWindow, SIGNAL(signalStartTest14()), this, SLOT(slotStartTest14()));
    connect(mainWindow, SIGNAL(signalStartTest15()), this, SLOT(slotStartTest15()));
    connect(mainWindow, SIGNAL(signalStartTest16()), this, SLOT(slotStartTest16()));
    connect(this, SIGNAL(signalChangeOpenDeviceStatus()), mainWindow, SLOT(slotChangeConnectStatus()));
    connect(this, SIGNAL(signalChangeItemBeforeFinish()), mainWindow, SLOT(slotChangeItemBeforeFinish()));
    connect(this, SIGNAL(signalStartTest()), mainWindow, SLOT(slotStartMesTest()));
    connect(this, SIGNAL(signalErrorSerial(QString)), mainWindow, SLOT(slotErrorSerial(QString)));
}


void MainControl::initializeStateMachine()
{
    QVector<QState*> tempVecState1;
    QVector<QState*> tempVecState2;
    QVector<QState*> tempVecState3;
    QVector<QState*> tempVecState4;
    QVector<QState*> tempVecState5;
    QVector<QState*> tempVecState6;
    QVector<QState*> tempVecState7;
    QVector<QState*> tempVecState8;
    QVector<QState*> tempVecState9;
    QVector<QState*> tempVecState10;
    QVector<QState*> tempVecState11;
    QVector<QState*> tempVecState12;
    QVector<QState*> tempVecState13;
    QVector<QState*> tempVecState14;
    QVector<QState*> tempVecState15;
    QVector<QState*> tempVecState16;
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    for(int i = 0; i < testState.keyCount(); i++)
    {
        QState* state1 = new QState(m_stateMachine1);
        QState* state2 = new QState(m_stateMachine2);
        QState* state3 = new QState(m_stateMachine3);
        QState* state4 = new QState(m_stateMachine4);
        QState* state5 = new QState(m_stateMachine5);
        QState* state6 = new QState(m_stateMachine6);
        QState* state7 = new QState(m_stateMachine7);
        QState* state8 = new QState(m_stateMachine8);
        QState* state9 = new QState(m_stateMachine9);
        QState* state10 = new QState(m_stateMachine10);
        QState* state11 = new QState(m_stateMachine11);
        QState* state12 = new QState(m_stateMachine12);
        QState* state13 = new QState(m_stateMachine13);
        QState* state14 = new QState(m_stateMachine14);
        QState* state15 = new QState(m_stateMachine15);
        QState* state16 = new QState(m_stateMachine16);
        tempVecState1.push_back(state1);
        tempVecState2.push_back(state2);
        tempVecState3.push_back(state3);
        tempVecState4.push_back(state4);
        tempVecState5.push_back(state5);
        tempVecState6.push_back(state6);
        tempVecState7.push_back(state7);
        tempVecState8.push_back(state8);
        tempVecState9.push_back(state9);
        tempVecState10.push_back(state10);
        tempVecState11.push_back(state11);
        tempVecState12.push_back(state12);
        tempVecState13.push_back(state13);
        tempVecState14.push_back(state14);
        tempVecState15.push_back(state15);
        tempVecState16.push_back(state16);
        state1->setObjectName(testState.valueToKey(i));
        state2->setObjectName(testState.valueToKey(i));
        state3->setObjectName(testState.valueToKey(i));
        state4->setObjectName(testState.valueToKey(i));
        state5->setObjectName(testState.valueToKey(i));
        state6->setObjectName(testState.valueToKey(i));
        state7->setObjectName(testState.valueToKey(i));
        state8->setObjectName(testState.valueToKey(i));
        state9->setObjectName(testState.valueToKey(i));
        state10->setObjectName(testState.valueToKey(i));
        state11->setObjectName(testState.valueToKey(i));
        state12->setObjectName(testState.valueToKey(i));
        state13->setObjectName(testState.valueToKey(i));
        state14->setObjectName(testState.valueToKey(i));
        state15->setObjectName(testState.valueToKey(i));
        state16->setObjectName(testState.valueToKey(i));
        QObject::connect(state1, &QState::entered, this, &MainControl::slotEnterTestState1);
        QObject::connect(state2, &QState::entered, this, &MainControl::slotEnterTestState2);
        QObject::connect(state3, &QState::entered, this, &MainControl::slotEnterTestState3);
        QObject::connect(state4, &QState::entered, this, &MainControl::slotEnterTestState4);
        QObject::connect(state5, &QState::entered, this, &MainControl::slotEnterTestState5);
        QObject::connect(state6, &QState::entered, this, &MainControl::slotEnterTestState6);
        QObject::connect(state7, &QState::entered, this, &MainControl::slotEnterTestState7);
        QObject::connect(state8, &QState::entered, this, &MainControl::slotEnterTestState8);
        QObject::connect(state9, &QState::entered, this, &MainControl::slotEnterTestState9);
        QObject::connect(state10, &QState::entered, this, &MainControl::slotEnterTestState10);
        QObject::connect(state11, &QState::entered, this, &MainControl::slotEnterTestState11);
        QObject::connect(state12, &QState::entered, this, &MainControl::slotEnterTestState12);
        QObject::connect(state13, &QState::entered, this, &MainControl::slotEnterTestState13);
        QObject::connect(state14, &QState::entered, this, &MainControl::slotEnterTestState14);
        QObject::connect(state15, &QState::entered, this, &MainControl::slotEnterTestState15);
        QObject::connect(state16, &QState::entered, this, &MainControl::slotEnterTestState16);
        QObject::connect(state1, &QState::exited, this, &MainControl::slotExitTestState1);
        QObject::connect(state2, &QState::exited, this, &MainControl::slotExitTestState2);
        QObject::connect(state3, &QState::exited, this, &MainControl::slotExitTestState3);
        QObject::connect(state4, &QState::exited, this, &MainControl::slotExitTestState4);
        QObject::connect(state5, &QState::exited, this, &MainControl::slotExitTestState5);
        QObject::connect(state6, &QState::exited, this, &MainControl::slotExitTestState6);
        QObject::connect(state7, &QState::exited, this, &MainControl::slotExitTestState7);
        QObject::connect(state8, &QState::exited, this, &MainControl::slotExitTestState8);
        QObject::connect(state9, &QState::exited, this, &MainControl::slotExitTestState9);
        QObject::connect(state10, &QState::exited, this, &MainControl::slotExitTestState10);
        QObject::connect(state11, &QState::exited, this, &MainControl::slotExitTestState11);
        QObject::connect(state12, &QState::exited, this, &MainControl::slotExitTestState12);
        QObject::connect(state13, &QState::exited, this, &MainControl::slotExitTestState13);
        QObject::connect(state14, &QState::exited, this, &MainControl::slotExitTestState14);
        QObject::connect(state15, &QState::exited, this, &MainControl::slotExitTestState15);
        QObject::connect(state16, &QState::exited, this, &MainControl::slotExitTestState16);
    }
    createTestStateLogic1(tempVecState1);
    createTestStateLogic2(tempVecState2);
    createTestStateLogic3(tempVecState3);
    createTestStateLogic4(tempVecState4);
    createTestStateLogic5(tempVecState5);
    createTestStateLogic6(tempVecState6);
    createTestStateLogic7(tempVecState7);
    createTestStateLogic8(tempVecState8);
    createTestStateLogic9(tempVecState9);
    createTestStateLogic10(tempVecState10);
    createTestStateLogic11(tempVecState11);
    createTestStateLogic12(tempVecState12);
    createTestStateLogic13(tempVecState13);
    createTestStateLogic14(tempVecState14);
    createTestStateLogic15(tempVecState15);
    createTestStateLogic16(tempVecState16);
    typeDialog = new ChooseTypeDialog();
    connect(typeDialog, SIGNAL(signalStartType(bool, int, int, int, int, int, int, int, int, int, int
                                               , int, int, int, int, int, int)), this, SLOT(slotStartMainWindow(
           bool, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int)));
    connect(typeDialog, SIGNAL(signalQuit()), this, SLOT(slotQuit()));
    typeDialog->exec();
}

bool MainControl::updateTestState1()
{
    if((currentTest1 == static_cast<int>(TestSetBatteryVoltage) || !if_test1))
    {
        if((currentTest2 == static_cast<int>(TestSetBatteryVoltage) || !if_test2) && (currentTest3 == static_cast<int>(TestSetBatteryVoltage) || !if_test3) && (currentTest4 == static_cast<int>(TestSetBatteryVoltage) || !if_test4))
        {
            qDebug() << "currentTest1 is " << currentTest1;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();
        }
        return false;
    }
    else if((currentTest1 == static_cast<int>(TestSetHALLCurrent) || !if_test1))
    {
        if((currentTest2 == static_cast<int>(TestSetHALLCurrent) || !if_test2) && (currentTest3 == static_cast<int>(TestSetHALLCurrent) || !if_test3) && (currentTest4 == static_cast<int>(TestSetHALLCurrent) || !if_test4))
        {
            qDebug() << "currentTest1 is " << currentTest1;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();

        }
        return false;
    }
    else if((currentTest1 == static_cast<int>(TestZDResistance) || !if_test1))
    {
        if((currentTest2 == static_cast<int>(TestZDResistance) || !if_test2) && (currentTest3 == static_cast<int>(TestZDResistance) || !if_test3) && (currentTest4 == static_cast<int>(TestZDResistance) || !if_test4))
        {
            qDebug() << "currentTest1 is " << currentTest1;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();
        }
        return false;
    }
    else if((currentTest1 == static_cast<int>(TestInsulationResistance) || !if_test1))
    {
        if((currentTest2 == static_cast<int>(TestInsulationResistance) || !if_test2) && (currentTest3 == static_cast<int>(TestInsulationResistance) || !if_test3) && (currentTest4 == static_cast<int>(TestInsulationResistance) || !if_test4))
        {
            qDebug() << "currentTest1 is " << currentTest1;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();
        }
        return false;
    }
    else if((currentTest1 == static_cast<int>(TestHALLCurrent) || !if_test1))
    {
        if((currentTest2 == static_cast<int>(TestHALLCurrent) || !if_test2) && (currentTest3 == static_cast<int>(TestHALLCurrent) || !if_test3) && (currentTest4 == static_cast<int>(TestHALLCurrent) || !if_test4))
        {
            qDebug() << "currentTest2 is " << currentTest2;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();

        }
        return false;
    }
    else if((currentTest1 == static_cast<int>(TestBatteryVoltage) || !if_test1))
    {
        if((currentTest2 == static_cast<int>(TestBatteryVoltage) || !if_test2) && (currentTest3 == static_cast<int>(TestBatteryVoltage) || !if_test3) && (currentTest4 == static_cast<int>(TestBatteryVoltage) || !if_test4))
        {
            qDebug() << "currentTest2 is " << currentTest2;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();

        }
        return false;
    }
    else
    {
        qDebug() << "currentTest1 is " << currentTest1;
        mainWindow->setCurrentItem1(currentTest1);
        emit signalSetCurrentTestStep1((int)currentTest1);
        return true;
    }
}

bool MainControl::updateTestState2()
{
    if((currentTest2 == static_cast<int>(TestSetBatteryVoltage) || !if_test2))
    {
        if((currentTest1 == static_cast<int>(TestSetBatteryVoltage) || !if_test1) && (currentTest3 == static_cast<int>(TestSetBatteryVoltage) || !if_test3) && (currentTest4 == static_cast<int>(TestSetBatteryVoltage) || !if_test4))
        {
            qDebug() << "currentTest2 is " << currentTest2;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();

        }
        return false;
    }
    else if((currentTest2 == static_cast<int>(TestHALLCurrent) || !if_test2))
    {
        if((currentTest1 == static_cast<int>(TestHALLCurrent) || !if_test1) && (currentTest3 == static_cast<int>(TestHALLCurrent) || !if_test3) && (currentTest4 == static_cast<int>(TestHALLCurrent) || !if_test4))
        {
            qDebug() << "currentTest2 is " << currentTest2;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();

        }
        return false;
    }
    else if((currentTest2 == static_cast<int>(TestBatteryVoltage) || !if_test2))
    {
        if((currentTest1 == static_cast<int>(TestBatteryVoltage) || !if_test1) && (currentTest3 == static_cast<int>(TestBatteryVoltage) || !if_test3) && (currentTest4 == static_cast<int>(TestBatteryVoltage) || !if_test4))
        {
            qDebug() << "currentTest2 is " << currentTest2;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();

        }
        return false;
    }
    else if((currentTest2 == static_cast<int>(TestSetHALLCurrent) || !if_test2))
    {
        if((currentTest1 == static_cast<int>(TestSetHALLCurrent) || !if_test1) && (currentTest3 == static_cast<int>(TestSetHALLCurrent) || !if_test3) && (currentTest4 == static_cast<int>(TestSetHALLCurrent) || !if_test4))
        {
            qDebug() << "currentTest2 is " << currentTest2;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();

        }
        return false;
    }
    else if((currentTest2 == static_cast<int>(TestZDResistance) || !if_test2))
    {
        if((currentTest1 == static_cast<int>(TestZDResistance) || !if_test1) && (currentTest3 == static_cast<int>(TestZDResistance) || !if_test3) && (currentTest4 == static_cast<int>(TestZDResistance) || !if_test4))
        {
            qDebug() << "currentTest2 is " << currentTest2;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();
        }
        return false;
    }
    else if((currentTest2 == static_cast<int>(TestInsulationResistance) || !if_test2))
    {
        if((currentTest1 == static_cast<int>(TestInsulationResistance) || !if_test1) && (currentTest3 == static_cast<int>(TestInsulationResistance) || !if_test3) && (currentTest4 == static_cast<int>(TestInsulationResistance) || !if_test4))
        {
            qDebug() << "currentTest2 is " << currentTest2;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();
        }
        return false;
    }
    else
    {
        qDebug() << "currentTest2 is " << currentTest2;
        mainWindow->setCurrentItem2(currentTest2);
        emit signalSetCurrentTestStep2((int)currentTest2);
        return true;
    }
}

bool MainControl::updateTestState3()
{
    if((currentTest3 == static_cast<int>(TestSetBatteryVoltage) || !if_test3))
    {
        if((currentTest2 == static_cast<int>(TestSetBatteryVoltage) || !if_test2) && (currentTest1 == static_cast<int>(TestSetBatteryVoltage) || !if_test1) && (currentTest4 == static_cast<int>(TestSetBatteryVoltage) || !if_test4))
        {
            qDebug() << "currentTest3 is " << currentTest3;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();

        }
        return false;
    }
    else if((currentTest3 == static_cast<int>(TestSetHALLCurrent) || !if_test3))
    {
        if((currentTest2 == static_cast<int>(TestSetHALLCurrent) || !if_test2) && (currentTest1 == static_cast<int>(TestSetHALLCurrent) || !if_test1) && (currentTest4 == static_cast<int>(TestSetHALLCurrent) || !if_test4))
        {
            qDebug() << "currentTest3 is " << currentTest3;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();

        }
        return false;
    }
    else if((currentTest3 == static_cast<int>(TestZDResistance) || !if_test3))
    {
        if((currentTest2 == static_cast<int>(TestZDResistance) || !if_test2) && (currentTest1 == static_cast<int>(TestZDResistance) || !if_test1) && (currentTest4 == static_cast<int>(TestZDResistance) || !if_test4))
        {
            qDebug() << "currentTest3 is " << currentTest3;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();
        }
        return false;
    }
    else if((currentTest3 == static_cast<int>(TestInsulationResistance) || !if_test3))
    {
        if((currentTest2 == static_cast<int>(TestInsulationResistance) || !if_test2) && (currentTest1 == static_cast<int>(TestInsulationResistance) || !if_test1) && (currentTest4 == static_cast<int>(TestInsulationResistance) || !if_test4))
        {
            qDebug() << "currentTest3 is " << currentTest3;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();
        }
        return false;
    }
    else if((currentTest3 == static_cast<int>(TestHALLCurrent) || !if_test3))
    {
        if((currentTest1 == static_cast<int>(TestHALLCurrent) || !if_test1) && (currentTest2 == static_cast<int>(TestHALLCurrent) || !if_test2) && (currentTest4 == static_cast<int>(TestHALLCurrent) || !if_test4))
        {
            qDebug() << "currentTest2 is " << currentTest2;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();

        }
        return false;
    }
    else if((currentTest3 == static_cast<int>(TestBatteryVoltage) || !if_test3))
    {
        if((currentTest1 == static_cast<int>(TestBatteryVoltage) || !if_test1) && (currentTest2 == static_cast<int>(TestBatteryVoltage) || !if_test2) && (currentTest4 == static_cast<int>(TestBatteryVoltage) || !if_test4))
        {
            qDebug() << "currentTest2 is " << currentTest2;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();

        }
        return false;
    }
    else
    {
        qDebug() << "currentTest3 is " << currentTest3;
        mainWindow->setCurrentItem3(currentTest3);
        emit signalSetCurrentTestStep3((int)currentTest3);
        return true;
    }
}

bool MainControl::updateTestState4()
{
    if((currentTest4 == static_cast<int>(TestSetBatteryVoltage) || !if_test4))
    {
        if((currentTest2 == static_cast<int>(TestSetBatteryVoltage) || !if_test2) && (currentTest3 == static_cast<int>(TestSetBatteryVoltage) || !if_test3) && (currentTest1 == static_cast<int>(TestSetBatteryVoltage) || !if_test1))
        {
            qDebug() << "currentTest4 is " << currentTest4;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();

        }
        return false;
    }
    else if((currentTest4 == static_cast<int>(TestSetHALLCurrent) || !if_test4))
    {
        if((currentTest2 == static_cast<int>(TestSetHALLCurrent) || !if_test2) && (currentTest3 == static_cast<int>(TestSetHALLCurrent) || !if_test3) && (currentTest1 == static_cast<int>(TestSetHALLCurrent) || !if_test1))
        {
            qDebug() << "currentTest4 is " << currentTest4;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();

        }
        return false;
    }
    else if((currentTest4 == static_cast<int>(TestZDResistance) || !if_test4))
    {
        if((currentTest2 == static_cast<int>(TestZDResistance) || !if_test2) && (currentTest3 == static_cast<int>(TestZDResistance) || !if_test3) && (currentTest1 == static_cast<int>(TestZDResistance) || !if_test1))
        {
            qDebug() << "currentTest4 is " << currentTest4;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();
        }
        return false;
    }
    else if((currentTest4 == static_cast<int>(TestInsulationResistance) || !if_test4))
    {
        if((currentTest2 == static_cast<int>(TestInsulationResistance) || !if_test2) && (currentTest3 == static_cast<int>(TestInsulationResistance) || !if_test3) && (currentTest1 == static_cast<int>(TestInsulationResistance) || !if_test1))
        {
            qDebug() << "currentTest4 is " << currentTest4;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();
        }
        return false;
    }
    else if((currentTest4 == static_cast<int>(TestHALLCurrent) || !if_test4))
    {
        if((currentTest1 == static_cast<int>(TestHALLCurrent) || !if_test1) && (currentTest3 == static_cast<int>(TestHALLCurrent) || !if_test3) && (currentTest2 == static_cast<int>(TestHALLCurrent) || !if_test2))
        {
            qDebug() << "currentTest2 is " << currentTest2;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();
        }
        return false;
    }
    else if((currentTest4 == static_cast<int>(TestBatteryVoltage) || !if_test4))
    {
        if((currentTest1 == static_cast<int>(TestBatteryVoltage) || !if_test1) && (currentTest3 == static_cast<int>(TestBatteryVoltage) || !if_test3) && (currentTest2 == static_cast<int>(TestBatteryVoltage) || !if_test2))
        {
            qDebug() << "currentTest2 is " << currentTest2;
            mainWindow->setCurrentItem1(currentTest1);
            emit signalSetCurrentTestStep1((int)currentTest1);
            mainWindow->setCurrentItem2(currentTest2);
            emit signalSetCurrentTestStep2((int)currentTest2);
            mainWindow->setCurrentItem3(currentTest3);
            emit signalSetCurrentTestStep3((int)currentTest3);
            mainWindow->setCurrentItem4(currentTest4);
            emit signalSetCurrentTestStep4((int)currentTest4);
            emitSignals1();
            emitSignals2();
            emitSignals3();
            emitSignals4();

        }
        return false;
    }
    else
    {
        qDebug() << "currentTest4 is " << currentTest4;
        mainWindow->setCurrentItem4(currentTest4);
        emit signalSetCurrentTestStep4((int)currentTest4);
        return true;
    }
}

bool MainControl::updateTestState5()
{
    if((currentTest5 == static_cast<int>(TestSetBatteryVoltage) || !if_test5))
    {
        if((currentTest6 == static_cast<int>(TestSetBatteryVoltage) || !if_test6) && (currentTest7 == static_cast<int>(TestSetBatteryVoltage) || !if_test7) && (currentTest8 == static_cast<int>(TestSetBatteryVoltage) || !if_test8))
        {
            qDebug() << "currentTest5 is " << currentTest5;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else if((currentTest5 == static_cast<int>(TestSetHALLCurrent) || !if_test5))
    {
        if((currentTest6 == static_cast<int>(TestSetHALLCurrent) || !if_test6) && (currentTest7 == static_cast<int>(TestSetHALLCurrent) || !if_test7) && (currentTest8 == static_cast<int>(TestSetHALLCurrent) || !if_test8))
        {
            qDebug() << "currentTest5 is " << currentTest5;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else if((currentTest5 == static_cast<int>(TestZDResistance) || !if_test5))
    {
        if((currentTest6 == static_cast<int>(TestZDResistance) || !if_test6) && (currentTest7 == static_cast<int>(TestZDResistance) || !if_test7) && (currentTest8 == static_cast<int>(TestZDResistance) || !if_test8))
        {
            qDebug() << "currentTest5 is " << currentTest5;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();
        }
        return false;
    }
    else if((currentTest5 == static_cast<int>(TestInsulationResistance) || !if_test5))
    {
        if((currentTest6 == static_cast<int>(TestInsulationResistance) || !if_test6) && (currentTest7 == static_cast<int>(TestInsulationResistance) || !if_test7) && (currentTest8 == static_cast<int>(TestInsulationResistance) || !if_test8))
        {
            qDebug() << "currentTest5 is " << currentTest5;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();
        }
        return false;
    }
    else if((currentTest5 == static_cast<int>(TestHALLCurrent) || !if_test5))
    {
        if((currentTest6 == static_cast<int>(TestHALLCurrent) || !if_test6) && (currentTest7 == static_cast<int>(TestHALLCurrent) || !if_test7) && (currentTest8 == static_cast<int>(TestHALLCurrent) || !if_test8))
        {
            qDebug() << "currentTest5 is " << currentTest5;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else if((currentTest5 == static_cast<int>(TestBatteryVoltage) || !if_test5))
    {
        if((currentTest6 == static_cast<int>(TestBatteryVoltage) || !if_test6) && (currentTest7 == static_cast<int>(TestBatteryVoltage) || !if_test7) && (currentTest8 == static_cast<int>(TestBatteryVoltage) || !if_test8))
        {
            qDebug() << "currentTest5 is " << currentTest5;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else
    {
        qDebug() << "currentTest5 is " << currentTest5;
        mainWindow->setCurrentItem5(currentTest5);
        emit signalSetCurrentTestStep5((int)currentTest5);
        return true;
    }
}

bool MainControl::updateTestState6()
{
    if((currentTest6 == static_cast<int>(TestSetBatteryVoltage) || !if_test6))
    {
        if((currentTest5 == static_cast<int>(TestSetBatteryVoltage) || !if_test5) && (currentTest7 == static_cast<int>(TestSetBatteryVoltage) || !if_test7) && (currentTest8 == static_cast<int>(TestSetBatteryVoltage) || !if_test8))
        {
            qDebug() << "currentTest6 is " << currentTest6;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else if((currentTest6 == static_cast<int>(TestSetHALLCurrent) || !if_test6))
    {
        if((currentTest5 == static_cast<int>(TestSetHALLCurrent) || !if_test5) && (currentTest7 == static_cast<int>(TestSetHALLCurrent) || !if_test7) && (currentTest8 == static_cast<int>(TestSetHALLCurrent) || !if_test8))
        {
            qDebug() << "currentTest6 is " << currentTest6;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else if((currentTest6 == static_cast<int>(TestZDResistance) || !if_test6))
    {
        if((currentTest5 == static_cast<int>(TestZDResistance) || !if_test5) && (currentTest7 == static_cast<int>(TestZDResistance) || !if_test7) && (currentTest8 == static_cast<int>(TestZDResistance) || !if_test8))
        {
            qDebug() << "currentTest6 is " << currentTest6;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();
        }
        return false;
    }
    else if((currentTest6 == static_cast<int>(TestInsulationResistance) || !if_test6))
    {
        if((currentTest5 == static_cast<int>(TestInsulationResistance) || !if_test5) && (currentTest7 == static_cast<int>(TestInsulationResistance) || !if_test7) && (currentTest8 == static_cast<int>(TestInsulationResistance) || !if_test8))
        {
            qDebug() << "currentTest6 is " << currentTest6;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();
        }
        return false;
    }
    else if((currentTest6 == static_cast<int>(TestHALLCurrent) || !if_test6))
    {
        if((currentTest5 == static_cast<int>(TestHALLCurrent) || !if_test5) && (currentTest7 == static_cast<int>(TestHALLCurrent) || !if_test7) && (currentTest8 == static_cast<int>(TestHALLCurrent) || !if_test8))
        {
            qDebug() << "currentTest5 is " << currentTest5;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else if((currentTest6 == static_cast<int>(TestBatteryVoltage) || !if_test6))
    {
        if((currentTest5 == static_cast<int>(TestBatteryVoltage) || !if_test5) && (currentTest7 == static_cast<int>(TestBatteryVoltage) || !if_test7) && (currentTest8 == static_cast<int>(TestBatteryVoltage) || !if_test8))
        {
            qDebug() << "currentTest5 is " << currentTest5;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else
    {
        qDebug() << "currentTest6 is " << currentTest6;
        mainWindow->setCurrentItem6(currentTest6);
        emit signalSetCurrentTestStep6((int)currentTest6);
        return true;
    }
}

bool MainControl::updateTestState7()
{
    if((currentTest7 == static_cast<int>(TestSetBatteryVoltage) || !if_test7))
    {
        if((currentTest6 == static_cast<int>(TestSetBatteryVoltage) || !if_test6) && (currentTest5 == static_cast<int>(TestSetBatteryVoltage) || !if_test5) && (currentTest8 == static_cast<int>(TestSetBatteryVoltage) || !if_test8))
        {
            qDebug() << "currentTest7 is " << currentTest7;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else if((currentTest7 == static_cast<int>(TestSetHALLCurrent) || !if_test7))
    {
        if((currentTest6 == static_cast<int>(TestSetHALLCurrent) || !if_test6) && (currentTest5 == static_cast<int>(TestSetHALLCurrent) || !if_test5) && (currentTest8 == static_cast<int>(TestSetHALLCurrent) || !if_test8))
        {
            qDebug() << "currentTest7 is " << currentTest7;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else if((currentTest7 == static_cast<int>(TestZDResistance) || !if_test7))
    {
        if((currentTest6 == static_cast<int>(TestZDResistance) || !if_test6) && (currentTest5 == static_cast<int>(TestZDResistance) || !if_test5) && (currentTest8 == static_cast<int>(TestZDResistance) || !if_test8))
        {
            qDebug() << "currentTest7 is " << currentTest7;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();
        }
        return false;
    }
    else if((currentTest7 == static_cast<int>(TestInsulationResistance) || !if_test7))
    {
        if((currentTest6 == static_cast<int>(TestInsulationResistance) || !if_test6) && (currentTest5 == static_cast<int>(TestInsulationResistance) || !if_test5) && (currentTest8 == static_cast<int>(TestInsulationResistance) || !if_test8))
        {
            qDebug() << "currentTest7 is " << currentTest7;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();
        }
        return false;
    }
    else if((currentTest7 == static_cast<int>(TestHALLCurrent) || !if_test7))
    {
        if((currentTest6 == static_cast<int>(TestHALLCurrent) || !if_test6) && (currentTest5 == static_cast<int>(TestHALLCurrent) || !if_test5) && (currentTest8 == static_cast<int>(TestHALLCurrent) || !if_test8))
        {
            qDebug() << "currentTest5 is " << currentTest5;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else if((currentTest7 == static_cast<int>(TestBatteryVoltage) || !if_test7))
    {
        if((currentTest6 == static_cast<int>(TestBatteryVoltage) || !if_test6) && (currentTest5 == static_cast<int>(TestBatteryVoltage) || !if_test5) && (currentTest8 == static_cast<int>(TestBatteryVoltage) || !if_test8))
        {
            qDebug() << "currentTest5 is " << currentTest5;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else
    {
        qDebug() << "currentTest7 is " << currentTest7;
        mainWindow->setCurrentItem7(currentTest7);
        emit signalSetCurrentTestStep7((int)currentTest7);
        return true;
    }
}

bool MainControl::updateTestState8()
{
    if((currentTest8 == static_cast<int>(TestSetBatteryVoltage) || !if_test8))
    {
        if((currentTest6 == static_cast<int>(TestSetBatteryVoltage) || !if_test6) && (currentTest7 == static_cast<int>(TestSetBatteryVoltage) || !if_test7) && (currentTest5 == static_cast<int>(TestSetBatteryVoltage) || !if_test5))
        {
            qDebug() << "currentTest8 is " << currentTest8;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else if((currentTest8 == static_cast<int>(TestSetHALLCurrent) || !if_test8))
    {
        if((currentTest6 == static_cast<int>(TestSetHALLCurrent) || !if_test6) && (currentTest7 == static_cast<int>(TestSetHALLCurrent) || !if_test7) && (currentTest5 == static_cast<int>(TestSetHALLCurrent) || !if_test5))
        {
            qDebug() << "currentTest8 is " << currentTest8;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else if((currentTest8 == static_cast<int>(TestZDResistance) || !if_test8))
    {
        if((currentTest6 == static_cast<int>(TestZDResistance) || !if_test6) && (currentTest7 == static_cast<int>(TestZDResistance) || !if_test7) && (currentTest5 == static_cast<int>(TestZDResistance) || !if_test5))
        {
            qDebug() << "currentTest8 is " << currentTest8;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();
        }
        return false;
    }
    else if((currentTest8 == static_cast<int>(TestInsulationResistance) || !if_test8))
    {
        if((currentTest6 == static_cast<int>(TestInsulationResistance) || !if_test6) && (currentTest7 == static_cast<int>(TestInsulationResistance) || !if_test7) && (currentTest5 == static_cast<int>(TestInsulationResistance) || !if_test5))
        {
            qDebug() << "currentTest8 is " << currentTest8;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();
        }
        return true;
    }
    else if((currentTest8 == static_cast<int>(TestHALLCurrent) || !if_test8))
    {
        if((currentTest6 == static_cast<int>(TestHALLCurrent) || !if_test6) && (currentTest7 == static_cast<int>(TestHALLCurrent) || !if_test7) && (currentTest5 == static_cast<int>(TestHALLCurrent) || !if_test5))
        {
            qDebug() << "currentTest5 is " << currentTest5;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else if((currentTest8 == static_cast<int>(TestBatteryVoltage) || !if_test8))
    {
        if((currentTest6 == static_cast<int>(TestBatteryVoltage) || !if_test6) && (currentTest7 == static_cast<int>(TestBatteryVoltage) || !if_test7) && (currentTest5 == static_cast<int>(TestBatteryVoltage) || !if_test5))
        {
            qDebug() << "currentTest5 is " << currentTest5;
            mainWindow->setCurrentItem5(currentTest5);
            emit signalSetCurrentTestStep5((int)currentTest5);
            mainWindow->setCurrentItem6(currentTest6);
            emit signalSetCurrentTestStep6((int)currentTest6);
            mainWindow->setCurrentItem7(currentTest7);
            emit signalSetCurrentTestStep7((int)currentTest7);
            mainWindow->setCurrentItem8(currentTest8);
            emit signalSetCurrentTestStep8((int)currentTest8);
            emitSignals5();
            emitSignals6();
            emitSignals7();
            emitSignals8();

        }
        return false;
    }
    else
    {
        qDebug() << "currentTest8 is " << currentTest8;
        mainWindow->setCurrentItem8(currentTest8);
        emit signalSetCurrentTestStep8((int)currentTest8);
        return true;
    }
}

bool MainControl::updateTestState9()
{
    if((currentTest9 == static_cast<int>(TestSetBatteryVoltage) || !if_test9))
    {
        if((currentTest10 == static_cast<int>(TestSetBatteryVoltage) || !if_test10) && (currentTest11 == static_cast<int>(TestSetBatteryVoltage) || !if_test11) && (currentTest12 == static_cast<int>(TestSetBatteryVoltage) || !if_test12))
        {
            qDebug() << "currentTest9 is " << currentTest9;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest9 == static_cast<int>(TestSetHALLCurrent) || !if_test9))
    {
        if((currentTest10 == static_cast<int>(TestSetHALLCurrent) || !if_test10) && (currentTest11 == static_cast<int>(TestSetHALLCurrent) || !if_test11) && (currentTest12 == static_cast<int>(TestSetHALLCurrent) || !if_test12))
        {
            qDebug() << "currentTest9 is " << currentTest9;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest9 == static_cast<int>(TestZDResistance) || !if_test9))
    {
        if((currentTest10 == static_cast<int>(TestZDResistance) || !if_test10) && (currentTest11 == static_cast<int>(TestZDResistance) || !if_test11) && (currentTest12 == static_cast<int>(TestZDResistance) || !if_test12))
        {
            qDebug() << "currentTest9 is " << currentTest9;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest9 == static_cast<int>(TestInsulationResistance) || !if_test9))
    {
        if((currentTest10 == static_cast<int>(TestInsulationResistance) || !if_test10) && (currentTest11 == static_cast<int>(TestInsulationResistance) || !if_test11) && (currentTest12 == static_cast<int>(TestInsulationResistance) || !if_test12))
        {
            qDebug() << "currentTest9 is " << currentTest9;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest9 == static_cast<int>(TestHALLCurrent) || !if_test9))
    {
        if((currentTest10 == static_cast<int>(TestHALLCurrent) || !if_test10) && (currentTest11 == static_cast<int>(TestHALLCurrent) || !if_test11) && (currentTest12 == static_cast<int>(TestHALLCurrent) || !if_test12))
        {
            qDebug() << "currentTest9 is " << currentTest9;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest9 == static_cast<int>(TestBatteryVoltage) || !if_test9))
    {
        if((currentTest10 == static_cast<int>(TestBatteryVoltage) || !if_test10) && (currentTest11 == static_cast<int>(TestBatteryVoltage) || !if_test11) && (currentTest12 == static_cast<int>(TestBatteryVoltage) || !if_test12))
        {
            qDebug() << "currentTest9 is " << currentTest9;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else
    {
        qDebug() << "currentTest9 is " << currentTest9;
        mainWindow->setCurrentItem9(currentTest9);
        emit signalSetCurrentTestStep9((int)currentTest9);
        return true;
    }
}

bool MainControl::updateTestState10()
{
    if((currentTest10 == static_cast<int>(TestSetBatteryVoltage) || !if_test10))
    {
        if((currentTest9 == static_cast<int>(TestSetBatteryVoltage) || !if_test9) && (currentTest11 == static_cast<int>(TestSetBatteryVoltage) || !if_test11) && (currentTest12 == static_cast<int>(TestSetBatteryVoltage) || !if_test12))
        {
            qDebug() << "currentTest9 is " << currentTest9;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest10 == static_cast<int>(TestSetHALLCurrent) || !if_test10))
    {
        if((currentTest9 == static_cast<int>(TestSetHALLCurrent) || !if_test9) && (currentTest11 == static_cast<int>(TestSetHALLCurrent) || !if_test11) && (currentTest12 == static_cast<int>(TestSetHALLCurrent) || !if_test12))
        {
            qDebug() << "currentTest10 is " << currentTest10;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);

            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();
        }
        return false;
    }
    else if((currentTest10 == static_cast<int>(TestZDResistance) || !if_test10))
    {
        if((currentTest9 == static_cast<int>(TestZDResistance) || !if_test9) && (currentTest11 == static_cast<int>(TestZDResistance) || !if_test11) && (currentTest12 == static_cast<int>(TestZDResistance) || !if_test12))
        {
            qDebug() << "currentTest10 is " << currentTest10;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest10 == static_cast<int>(TestInsulationResistance) || !if_test10))
    {
        if((currentTest9 == static_cast<int>(TestInsulationResistance) || !if_test9) && (currentTest11 == static_cast<int>(TestInsulationResistance) || !if_test11) && (currentTest12 == static_cast<int>(TestInsulationResistance) || !if_test12))
        {
            qDebug() << "currentTest10 is " << currentTest10;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest10 == static_cast<int>(TestHALLCurrent) || !if_test10))
    {
        if((currentTest9 == static_cast<int>(TestHALLCurrent) || !if_test9) && (currentTest11 == static_cast<int>(TestHALLCurrent) || !if_test11) && (currentTest12 == static_cast<int>(TestHALLCurrent) || !if_test12))
        {
            qDebug() << "currentTest9 is " << currentTest9;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest10 == static_cast<int>(TestBatteryVoltage) || !if_test10))
    {
        if((currentTest9 == static_cast<int>(TestBatteryVoltage) || !if_test9) && (currentTest11 == static_cast<int>(TestBatteryVoltage) || !if_test11) && (currentTest12 == static_cast<int>(TestBatteryVoltage) || !if_test12))
        {
            qDebug() << "currentTest9 is " << currentTest9;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else
    {
        qDebug() << "currentTest10 is " << currentTest10;
        mainWindow->setCurrentItem10(currentTest10);
        emit signalSetCurrentTestStep10((int)currentTest10);
        return true;
    }
}

bool MainControl::updateTestState11()
{
    if((currentTest11 == static_cast<int>(TestSetBatteryVoltage) || !if_test11))
    {
        if((currentTest10 == static_cast<int>(TestSetBatteryVoltage) || !if_test10) && (currentTest9 == static_cast<int>(TestSetBatteryVoltage) || !if_test9) && (currentTest12 == static_cast<int>(TestSetBatteryVoltage) || !if_test12))
        {
            qDebug() << "currentTest11 is " << currentTest11;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest11 == static_cast<int>(TestSetHALLCurrent) || !if_test11))
    {
        if((currentTest10 == static_cast<int>(TestSetHALLCurrent) || !if_test10) && (currentTest9 == static_cast<int>(TestSetHALLCurrent) || !if_test9) && (currentTest12 == static_cast<int>(TestSetHALLCurrent) || !if_test12))
        {
            qDebug() << "currentTest11 is " << currentTest11;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest11 == static_cast<int>(TestZDResistance) || !if_test11))
    {
        if((currentTest10 == static_cast<int>(TestZDResistance) || !if_test10) && (currentTest9 == static_cast<int>(TestZDResistance) || !if_test9) && (currentTest12 == static_cast<int>(TestZDResistance) || !if_test12))
        {
            qDebug() << "currentTest11 is " << currentTest11;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest11 == static_cast<int>(TestInsulationResistance) || !if_test11))
    {
        if((currentTest10 == static_cast<int>(TestInsulationResistance) || !if_test10) && (currentTest9 == static_cast<int>(TestInsulationResistance) || !if_test9) && (currentTest12 == static_cast<int>(TestInsulationResistance) || !if_test12))
        {
            qDebug() << "currentTest11 is " << currentTest11;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest11 == static_cast<int>(TestHALLCurrent) || !if_test11))
    {
        if((currentTest10 == static_cast<int>(TestHALLCurrent) || !if_test10) && (currentTest9 == static_cast<int>(TestHALLCurrent) || !if_test9) && (currentTest12 == static_cast<int>(TestHALLCurrent) || !if_test12))
        {
            qDebug() << "currentTest9 is " << currentTest9;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest11 == static_cast<int>(TestBatteryVoltage) || !if_test11))
    {
        if((currentTest10 == static_cast<int>(TestBatteryVoltage) || !if_test10) && (currentTest9 == static_cast<int>(TestBatteryVoltage) || !if_test9) && (currentTest12 == static_cast<int>(TestBatteryVoltage) || !if_test12))
        {
            qDebug() << "currentTest9 is " << currentTest9;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else
    {
        qDebug() << "currentTest11 is " << currentTest11;
        mainWindow->setCurrentItem11(currentTest11);
        emit signalSetCurrentTestStep11((int)currentTest11);
        return true;
    }
}

bool MainControl::updateTestState12()
{
    if((currentTest12 == static_cast<int>(TestSetBatteryVoltage) || !if_test12))
    {
        if((currentTest10 == static_cast<int>(TestSetBatteryVoltage) || !if_test10) && (currentTest11 == static_cast<int>(TestSetBatteryVoltage) || !if_test11) && (currentTest9 == static_cast<int>(TestSetBatteryVoltage) || !if_test9))
        {
            qDebug() << "currentTest12 is " << currentTest12;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();
        }
        return false;
    }
    else if((currentTest12 == static_cast<int>(TestSetHALLCurrent) || !if_test12))
    {
        if((currentTest10 == static_cast<int>(TestSetHALLCurrent) || !if_test10) && (currentTest11 == static_cast<int>(TestSetHALLCurrent) || !if_test11) && (currentTest9 == static_cast<int>(TestSetHALLCurrent) || !if_test9))
        {
            qDebug() << "currentTest12 is " << currentTest12;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest12 == static_cast<int>(TestZDResistance) || !if_test12))
    {
        if((currentTest10 == static_cast<int>(TestZDResistance) || !if_test10) && (currentTest11 == static_cast<int>(TestZDResistance) || !if_test11) && (currentTest9 == static_cast<int>(TestZDResistance) || !if_test9))
        {
            qDebug() << "currentTest12 is " << currentTest12;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);

            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();
        }
        return false;
    }
    else if((currentTest12 == static_cast<int>(TestInsulationResistance) || !if_test12))
    {
        if((currentTest10 == static_cast<int>(TestInsulationResistance) || !if_test10) && (currentTest11 == static_cast<int>(TestInsulationResistance) || !if_test11) && (currentTest9 == static_cast<int>(TestInsulationResistance) || !if_test9))
        {
            qDebug() << "currentTest12 is " << currentTest12;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);

            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();
        }
        return false;
    }
    else if((currentTest12 == static_cast<int>(TestHALLCurrent) || !if_test12))
    {
        if((currentTest10 == static_cast<int>(TestHALLCurrent) || !if_test10) && (currentTest11 == static_cast<int>(TestHALLCurrent) || !if_test11) && (currentTest9 == static_cast<int>(TestHALLCurrent) || !if_test9))
        {
            qDebug() << "currentTest9 is " << currentTest9;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else if((currentTest12 == static_cast<int>(TestBatteryVoltage) || !if_test12))
    {
        if((currentTest10 == static_cast<int>(TestBatteryVoltage) || !if_test10) && (currentTest11 == static_cast<int>(TestBatteryVoltage) || !if_test11) && (currentTest9 == static_cast<int>(TestBatteryVoltage) || !if_test9))
        {
            qDebug() << "currentTest9 is " << currentTest9;
            mainWindow->setCurrentItem9(currentTest9);
            emit signalSetCurrentTestStep9((int)currentTest9);
            mainWindow->setCurrentItem10(currentTest10);
            emit signalSetCurrentTestStep10((int)currentTest10);
            mainWindow->setCurrentItem11(currentTest11);
            emit signalSetCurrentTestStep11((int)currentTest11);
            mainWindow->setCurrentItem12(currentTest12);
            emit signalSetCurrentTestStep12((int)currentTest12);
            emitSignals9();
            emitSignals10();
            emitSignals11();
            emitSignals12();

        }
        return false;
    }
    else
    {
        qDebug() << "currentTest12 is " << currentTest12;
        mainWindow->setCurrentItem12(currentTest12);
        emit signalSetCurrentTestStep12((int)currentTest12);
        return true;
    }
}

bool MainControl::updateTestState13()
{
    if((currentTest13 == static_cast<int>(TestSetBatteryVoltage) || !if_test13))
    {
        if((currentTest14 == static_cast<int>(TestSetBatteryVoltage) || !if_test14) && (currentTest15 == static_cast<int>(TestSetBatteryVoltage) || !if_test15) && (currentTest16 == static_cast<int>(TestSetBatteryVoltage) || !if_test16))
        {
            qDebug() << "currentTest13 is " << currentTest13;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest13 == static_cast<int>(TestSetHALLCurrent) || !if_test13))
    {
        if((currentTest14 == static_cast<int>(TestSetHALLCurrent) || !if_test14) && (currentTest15 == static_cast<int>(TestSetHALLCurrent) || !if_test15) && (currentTest16 == static_cast<int>(TestSetHALLCurrent) || !if_test16))
        {
            qDebug() << "currentTest13 is " << currentTest13;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest13 == static_cast<int>(TestZDResistance) || !if_test13))
    {
        if((currentTest14 == static_cast<int>(TestZDResistance) || !if_test14) && (currentTest15 == static_cast<int>(TestZDResistance) || !if_test15) && (currentTest16 == static_cast<int>(TestZDResistance) || !if_test16))
        {
            qDebug() << "currentTest13 is " << currentTest13;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest13 == static_cast<int>(TestInsulationResistance) || !if_test13))
    {
        if((currentTest14 == static_cast<int>(TestInsulationResistance) || !if_test14) && (currentTest15 == static_cast<int>(TestInsulationResistance) || !if_test15) && (currentTest16 == static_cast<int>(TestInsulationResistance) || !if_test16))
        {
            qDebug() << "currentTest13 is " << currentTest13;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest13 == static_cast<int>(TestHALLCurrent) || !if_test13))
    {
        if((currentTest14 == static_cast<int>(TestHALLCurrent) || !if_test14) && (currentTest15 == static_cast<int>(TestHALLCurrent) || !if_test15) && (currentTest16 == static_cast<int>(TestHALLCurrent) || !if_test16))
        {
            qDebug() << "currentTest13 is " << currentTest13;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest13 == static_cast<int>(TestBatteryVoltage) || !if_test13))
    {
        if((currentTest14 == static_cast<int>(TestBatteryVoltage) || !if_test14) && (currentTest15 == static_cast<int>(TestBatteryVoltage) || !if_test15) && (currentTest16 == static_cast<int>(TestBatteryVoltage) || !if_test16))
        {
            qDebug() << "currentTest13 is " << currentTest13;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else
    {
        qDebug() << "currentTest13 is " << currentTest13;
        mainWindow->setCurrentItem13(currentTest13);
        emit signalSetCurrentTestStep13((int)currentTest13);
        return true;
    }
}

bool MainControl::updateTestState14()
{
    if((currentTest14 == static_cast<int>(TestSetBatteryVoltage) || !if_test14))
    {
        if((currentTest13 == static_cast<int>(TestSetBatteryVoltage) || !if_test13) && (currentTest15 == static_cast<int>(TestSetBatteryVoltage) || !if_test15) && (currentTest16 == static_cast<int>(TestSetBatteryVoltage) || !if_test16))
        {
            qDebug() << "currentTest14 is " << currentTest14;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest14 == static_cast<int>(TestSetHALLCurrent) || !if_test14))
    {
        if((currentTest13 == static_cast<int>(TestSetHALLCurrent) || !if_test13) && (currentTest15 == static_cast<int>(TestSetHALLCurrent) || !if_test15) && (currentTest16 == static_cast<int>(TestSetHALLCurrent) || !if_test16))
        {
            qDebug() << "currentTest14 is " << currentTest14;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest14 == static_cast<int>(TestZDResistance) || !if_test14))
    {
        if((currentTest13 == static_cast<int>(TestZDResistance) || !if_test13) && (currentTest15 == static_cast<int>(TestZDResistance) || !if_test15) && (currentTest16 == static_cast<int>(TestZDResistance) || !if_test16))
        {
            qDebug() << "currentTest14 is " << currentTest14;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest14 == static_cast<int>(TestInsulationResistance) || !if_test14))
    {
        if((currentTest13 == static_cast<int>(TestInsulationResistance) || !if_test13) && (currentTest15 == static_cast<int>(TestInsulationResistance) || !if_test15) && (currentTest16 == static_cast<int>(TestInsulationResistance) || !if_test16))
        {
            qDebug() << "currentTest14 is " << currentTest14;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest14 == static_cast<int>(TestHALLCurrent) || !if_test14))
    {
        if((currentTest13 == static_cast<int>(TestHALLCurrent) || !if_test13) && (currentTest15 == static_cast<int>(TestHALLCurrent) || !if_test15) && (currentTest16 == static_cast<int>(TestHALLCurrent) || !if_test16))
        {
            qDebug() << "currentTest13 is " << currentTest13;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest14 == static_cast<int>(TestBatteryVoltage) || !if_test14))
    {
        if((currentTest13 == static_cast<int>(TestBatteryVoltage) || !if_test13) && (currentTest15 == static_cast<int>(TestBatteryVoltage) || !if_test15) && (currentTest16 == static_cast<int>(TestBatteryVoltage) || !if_test16))
        {
            qDebug() << "currentTest13 is " << currentTest13;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else
    {
        qDebug() << "currentTest14 is " << currentTest14;
        mainWindow->setCurrentItem14(currentTest14);
        emit signalSetCurrentTestStep14((int)currentTest14);
        return true;
    }
}

bool MainControl::updateTestState15()
{
    if((currentTest15 == static_cast<int>(TestSetBatteryVoltage) || !if_test15))
    {
        if((currentTest14 == static_cast<int>(TestSetBatteryVoltage) || !if_test14) && (currentTest13 == static_cast<int>(TestSetBatteryVoltage) || !if_test13) && (currentTest16 == static_cast<int>(TestSetBatteryVoltage) || !if_test16))
        {
            qDebug() << "currentTest15 is " << currentTest15;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest15 == static_cast<int>(TestSetHALLCurrent) || !if_test15))
    {
        if((currentTest14 == static_cast<int>(TestSetHALLCurrent) || !if_test14) && (currentTest13 == static_cast<int>(TestSetHALLCurrent) || !if_test13) && (currentTest16 == static_cast<int>(TestSetHALLCurrent) || !if_test16))
        {
            qDebug() << "currentTest15 is " << currentTest15;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest15 == static_cast<int>(TestZDResistance) || !if_test15))
    {
        if((currentTest14 == static_cast<int>(TestZDResistance) || !if_test14) && (currentTest13 == static_cast<int>(TestZDResistance) || !if_test13) && (currentTest16 == static_cast<int>(TestZDResistance) || !if_test16))
        {
            qDebug() << "currentTest15 is " << currentTest15;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest15 == static_cast<int>(TestInsulationResistance) || !if_test15))
    {
        if((currentTest14 == static_cast<int>(TestInsulationResistance) || !if_test14) && (currentTest13 == static_cast<int>(TestInsulationResistance) || !if_test13) && (currentTest16 == static_cast<int>(TestInsulationResistance) || !if_test16))
        {
            qDebug() << "currentTest15 is " << currentTest15;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest15 == static_cast<int>(TestHALLCurrent) || !if_test15))
    {
        if((currentTest14 == static_cast<int>(TestHALLCurrent) || !if_test14) && (currentTest13 == static_cast<int>(TestHALLCurrent) || !if_test13) && (currentTest16 == static_cast<int>(TestHALLCurrent) || !if_test16))
        {
            qDebug() << "currentTest13 is " << currentTest13;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest15 == static_cast<int>(TestBatteryVoltage) || !if_test15))
    {
        if((currentTest14 == static_cast<int>(TestBatteryVoltage) || !if_test14) && (currentTest13 == static_cast<int>(TestBatteryVoltage) || !if_test13) && (currentTest16 == static_cast<int>(TestBatteryVoltage) || !if_test16))
        {
            qDebug() << "currentTest13 is " << currentTest13;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else
    {
        qDebug() << "currentTest15 is " << currentTest15;
        mainWindow->setCurrentItem15(currentTest15);
        emit signalSetCurrentTestStep15((int)currentTest15);
        return true;
    }
}

bool MainControl::updateTestState16()
{
    if((currentTest16 == static_cast<int>(TestSetBatteryVoltage) || !if_test16))
    {
        if((currentTest14 == static_cast<int>(TestSetBatteryVoltage) || !if_test14) && (currentTest15 == static_cast<int>(TestSetBatteryVoltage) || !if_test15) && (currentTest13 == static_cast<int>(TestSetBatteryVoltage) || !if_test13))
        {
            qDebug() << "currentTest16 is " << currentTest16;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest16 == static_cast<int>(TestSetHALLCurrent) || !if_test16))
    {
        if((currentTest14 == static_cast<int>(TestSetHALLCurrent) || !if_test14) && (currentTest15 == static_cast<int>(TestSetHALLCurrent) || !if_test15) && (currentTest13 == static_cast<int>(TestSetHALLCurrent) || !if_test13))
        {
            qDebug() << "currentTest16 is " << currentTest16;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest16 == static_cast<int>(TestZDResistance) || !if_test16))
    {
        if((currentTest14 == static_cast<int>(TestZDResistance) || !if_test14) && (currentTest15 == static_cast<int>(TestZDResistance) || !if_test15) && (currentTest13 == static_cast<int>(TestZDResistance) || !if_test13))
        {
            qDebug() << "currentTest16 is " << currentTest16;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest16 == static_cast<int>(TestInsulationResistance) || !if_test16))
    {
        if((currentTest14 == static_cast<int>(TestInsulationResistance) || !if_test14) && (currentTest15 == static_cast<int>(TestInsulationResistance) || !if_test15) && (currentTest13 == static_cast<int>(TestInsulationResistance) || !if_test13))
        {
            qDebug() << "currentTest16 is " << currentTest16;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest16 == static_cast<int>(TestHALLCurrent) || !if_test16))
    {
        if((currentTest14 == static_cast<int>(TestHALLCurrent) || !if_test14) && (currentTest15 == static_cast<int>(TestHALLCurrent) || !if_test15) && (currentTest13 == static_cast<int>(TestHALLCurrent) || !if_test13))
        {
            qDebug() << "currentTest13 is " << currentTest13;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else if((currentTest16 == static_cast<int>(TestBatteryVoltage) || !if_test16))
    {
        if((currentTest14 == static_cast<int>(TestBatteryVoltage) || !if_test14) && (currentTest15 == static_cast<int>(TestBatteryVoltage) || !if_test15) && (currentTest13 == static_cast<int>(TestBatteryVoltage) || !if_test13))
        {
            qDebug() << "currentTest13 is " << currentTest13;
            mainWindow->setCurrentItem13(currentTest13);
            emit signalSetCurrentTestStep13((int)currentTest13);
            mainWindow->setCurrentItem14(currentTest14);
            emit signalSetCurrentTestStep14((int)currentTest14);
            mainWindow->setCurrentItem15(currentTest15);
            emit signalSetCurrentTestStep15((int)currentTest15);
            mainWindow->setCurrentItem16(currentTest16);
            emit signalSetCurrentTestStep16((int)currentTest16);
            emitSignals13();
            emitSignals14();
            emitSignals15();
            emitSignals16();
        }
        return false;
    }
    else
    {
        qDebug() << "currentTest16 is " << currentTest16;
        mainWindow->setCurrentItem16(currentTest16);
        emit signalSetCurrentTestStep16((int)currentTest16);
        return true;
    }
}

QString MainControl::findRenderSignal1(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "1()";
    }
    return "";
}

QString MainControl::findRenderSignal1(const TestItemIdentify &type)
{
    return findRenderSignal1(findState1(type));
}

QState *MainControl::findState1(const QString &name) const
{
    return m_stateMachine1->findChild<QState*>(name);
}

QState *MainControl::findState1(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState1(testState.valueToKey(type));
}
QString MainControl::findRenderSignal2(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "2()";
    }
    return "";
}

QString MainControl::findRenderSignal2(const TestItemIdentify &type)
{
    return findRenderSignal2(findState2(type));
}

QState *MainControl::findState2(const QString &name) const
{
    return m_stateMachine2->findChild<QState*>(name);
}

QState *MainControl::findState2(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState2(testState.valueToKey(type));
}
QString MainControl::findRenderSignal3(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "3()";
    }
    return "";
}

QString MainControl::findRenderSignal3(const TestItemIdentify &type)
{
    return findRenderSignal3(findState3(type));
}

QState *MainControl::findState3(const QString &name) const
{
    return m_stateMachine3->findChild<QState*>(name);
}

QState *MainControl::findState3(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState3(testState.valueToKey(type));
}
QString MainControl::findRenderSignal4(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "4()";
    }
    return "";
}

QString MainControl::findRenderSignal4(const TestItemIdentify &type)
{
    return findRenderSignal4(findState4(type));
}

QState *MainControl::findState4(const QString &name) const
{
    return m_stateMachine4->findChild<QState*>(name);
}

QState *MainControl::findState4(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState4(testState.valueToKey(type));
}
QString MainControl::findRenderSignal5(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "5()";
    }
    return "";
}

QString MainControl::findRenderSignal5(const TestItemIdentify &type)
{
    return findRenderSignal5(findState5(type));
}

QState *MainControl::findState5(const QString &name) const
{
    return m_stateMachine5->findChild<QState*>(name);
}

QState *MainControl::findState5(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState5(testState.valueToKey(type));
}
QString MainControl::findRenderSignal6(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "6()";
    }
    return "";
}

QString MainControl::findRenderSignal6(const TestItemIdentify &type)
{
    return findRenderSignal6(findState6(type));
}

QState *MainControl::findState6(const QString &name) const
{
    return m_stateMachine6->findChild<QState*>(name);
}

QState *MainControl::findState6(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState6(testState.valueToKey(type));
}
QString MainControl::findRenderSignal7(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "7()";
    }
    return "";
}

QString MainControl::findRenderSignal7(const TestItemIdentify &type)
{
    return findRenderSignal7(findState7(type));
}

QState *MainControl::findState7(const QString &name) const
{
    return m_stateMachine7->findChild<QState*>(name);
}

QState *MainControl::findState7(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState7(testState.valueToKey(type));
}
QString MainControl::findRenderSignal8(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "8()";
    }
    return "";
}

QString MainControl::findRenderSignal8(const TestItemIdentify &type)
{
    return findRenderSignal8(findState8(type));
}

QState *MainControl::findState8(const QString &name) const
{
    return m_stateMachine8->findChild<QState*>(name);
}

QState *MainControl::findState8(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState8(testState.valueToKey(type));
}
QString MainControl::findRenderSignal9(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "9()";
    }
    return "";
}

QString MainControl::findRenderSignal9(const TestItemIdentify &type)
{
    return findRenderSignal9(findState9(type));
}

QState *MainControl::findState9(const QString &name) const
{
    return m_stateMachine9->findChild<QState*>(name);
}

QState *MainControl::findState9(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState9(testState.valueToKey(type));
}
QString MainControl::findRenderSignal10(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "10()";
    }
    return "";
}

QString MainControl::findRenderSignal10(const TestItemIdentify &type)
{
    return findRenderSignal10(findState10(type));
}

QState *MainControl::findState10(const QString &name) const
{
    return m_stateMachine10->findChild<QState*>(name);
}

QState *MainControl::findState10(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState10(testState.valueToKey(type));
}
QString MainControl::findRenderSignal11(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "11()";
    }
    return "";
}

QString MainControl::findRenderSignal11(const TestItemIdentify &type)
{
    return findRenderSignal11(findState11(type));
}

QState *MainControl::findState11(const QString &name) const
{
    return m_stateMachine11->findChild<QState*>(name);
}

QState *MainControl::findState11(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState11(testState.valueToKey(type));
}
QString MainControl::findRenderSignal12(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "12()";
    }
    return "";
}

QString MainControl::findRenderSignal12(const TestItemIdentify &type)
{
    return findRenderSignal12(findState12(type));
}

QState *MainControl::findState12(const QString &name) const
{
    return m_stateMachine12->findChild<QState*>(name);
}

QState *MainControl::findState12(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState12(testState.valueToKey(type));
}
QString MainControl::findRenderSignal13(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "13()";
    }
    return "";
}

QString MainControl::findRenderSignal13(const TestItemIdentify &type)
{
    return findRenderSignal13(findState13(type));
}

QState *MainControl::findState13(const QString &name) const
{
    return m_stateMachine13->findChild<QState*>(name);
}

QState *MainControl::findState13(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState13(testState.valueToKey(type));
}
QString MainControl::findRenderSignal14(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "14()";
    }
    return "";
}

QString MainControl::findRenderSignal14(const TestItemIdentify &type)
{
    return findRenderSignal14(findState14(type));
}

QState *MainControl::findState14(const QString &name) const
{
    return m_stateMachine14->findChild<QState*>(name);
}

QState *MainControl::findState14(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState14(testState.valueToKey(type));
}
QString MainControl::findRenderSignal15(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "15()";
    }
    return "";
}

QString MainControl::findRenderSignal15(const TestItemIdentify &type)
{
    return findRenderSignal15(findState15(type));
}

QState *MainControl::findState15(const QString &name) const
{
    return m_stateMachine15->findChild<QState*>(name);
}

QState *MainControl::findState15(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState15(testState.valueToKey(type));
}
QString MainControl::findRenderSignal16(QState *state)
{
    if(state)
    {
        return "signalEnter" + state->objectName() + "16()";
    }
    return "";
}

QString MainControl::findRenderSignal16(const TestItemIdentify &type)
{
    return findRenderSignal16(findState16(type));
}

QState *MainControl::findState16(const QString &name) const
{
    return m_stateMachine16->findChild<QState*>(name);
}

QState *MainControl::findState16(const TestItemIdentify &type) const
{
    QMetaEnum testState = QMetaEnum::fromType<TestItemIdentify>();
    return findState16(testState.valueToKey(type));
}

void MainControl::createTestStateLogic1(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal1(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic2(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal2(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic3(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal3(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic4(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal4(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic5(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal5(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic6(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal6(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic7(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal7(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic8(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal8(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic9(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal9(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic10(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal10(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic11(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal11(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic12(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal12(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic13(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal13(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic14(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal14(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic15(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal15(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::createTestStateLogic16(const QVector<QState *> &tempVecState)
{
    for(auto index = 1; index < 32; ++index)
    {
        if(tempVecState[index])
        {
            for(auto findindex = 1; findindex < 32; ++findindex)
            {
                if(tempVecState.at(findindex))
                {
                    tempVecState[findindex]->addTransition(
                        this, findRenderSignal16(tempVecState[index]).toLatin1(), tempVecState[index]);
                }
            }
        }
    }
}

void MainControl::emitSignals1()
{
    switch(currentTest1)
    {
    case StateStartTest:
    {
        emit signalEnterStateStartTest1();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test1 = true;
        testResult1 = true;
        emit signalEnterStateTestSetBatteryVoltage1();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage1();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance1();
        break;
    }
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage1();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent1();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC1();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal1();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent1();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface1();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO1();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP1();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication1();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication1();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM1();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash1();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC1();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature1();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature1();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet1();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower1();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion1();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion1();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC1();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC1();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial1();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial1();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI1();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB1();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance1();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest1();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest1();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType1();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals2()
{
    switch(currentTest2)
    {
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage2();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent2();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC2();
        break;
    }
    case StateStartTest:
    {
        emit signalEnterStateStartTest2();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test2 = true;
        testResult2 = true;
        emit signalEnterStateTestSetBatteryVoltage2();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage2();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance2();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal2();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent2();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface2();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO2();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP2();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication2();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication2();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM2();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash2();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC2();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature2();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature2();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet2();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower2();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion2();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion2();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC2();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC2();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial2();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial2();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI2();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB2();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance2();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest2();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest2();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType2();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals3()
{
    switch(currentTest3)
    {
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage3();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent3();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC3();
        break;
    }
    case StateStartTest:
    {
        emit signalEnterStateStartTest3();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test3 = true;
        testResult3 = true;
        emit signalEnterStateTestSetBatteryVoltage3();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage3();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance3();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal3();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent3();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface3();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO3();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP3();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication3();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication3();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM3();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash3();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC3();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature3();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature3();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet3();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower3();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion3();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion3();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC3();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC3();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial3();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial3();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI3();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB3();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance3();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest3();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest3();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType3();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals4()
{
    switch(currentTest4)
    {
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage4();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent4();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC4();
        break;
    }
    case StateStartTest:
    {
        emit signalEnterStateStartTest4();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test4 = true;
        testResult4 = true;
        emit signalEnterStateTestSetBatteryVoltage4();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage4();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance4();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal4();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent4();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface4();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO4();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP4();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication4();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication4();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM4();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash4();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC4();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature4();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature4();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet4();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower4();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion4();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion4();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC4();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC4();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial4();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial4();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI4();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB4();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance4();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest4();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest4();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType4();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals5()
{
    switch(currentTest5)
    {
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage5();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent5();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC5();
        break;
    }
    case StateStartTest:
    {
        emit signalEnterStateStartTest5();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test5 = true;
        testResult5 = true;
        emit signalEnterStateTestSetBatteryVoltage5();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage5();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance5();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal5();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent5();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface5();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO5();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP5();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication5();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication5();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM5();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash5();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC5();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature5();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature5();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet5();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower5();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion5();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion5();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC5();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC5();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial5();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial5();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI5();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB5();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance5();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest5();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest5();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType5();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals6()
{
    switch(currentTest6)
    {
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage6();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent6();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC6();
        break;
    }
    case StateStartTest:
    {
        emit signalEnterStateStartTest6();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test6 = true;
        testResult6 = true;
        emit signalEnterStateTestSetBatteryVoltage6();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage6();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance6();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal6();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent6();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface6();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO6();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP6();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication6();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication6();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM6();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash6();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC6();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature6();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature6();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet6();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower6();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion6();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion6();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC6();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC6();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial6();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial6();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI6();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB6();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance6();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest6();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest6();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType6();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals7()
{
    switch(currentTest7)
    {
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage7();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent7();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC7();
        break;
    }
    case StateStartTest:
    {
        emit signalEnterStateStartTest7();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test7 = true;
        testResult7 = true;
        emit signalEnterStateTestSetBatteryVoltage7();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage7();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance7();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal7();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent7();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface7();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO7();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP7();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication7();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication7();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM7();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash7();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC7();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature7();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature7();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet7();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower7();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion7();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion7();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC7();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC7();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial7();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial7();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI7();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB7();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance7();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest7();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest7();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType7();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals8()
{
    switch(currentTest8)
    {
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage8();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent8();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC8();
        break;
    }
    case StateStartTest:
    {
        emit signalEnterStateStartTest8();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test8 = true;
        testResult8 = true;
        emit signalEnterStateTestSetBatteryVoltage8();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage8();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance8();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal8();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent8();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface8();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO8();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP8();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication8();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication8();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM8();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash8();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC8();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature8();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature8();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet8();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower8();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion8();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion8();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC8();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC8();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial8();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial8();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI8();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB8();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance8();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest8();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest8();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType8();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals9()
{
    switch(currentTest9)
    {
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage9();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent9();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC9();
        break;
    }
    case StateStartTest:
    {
        emit signalEnterStateStartTest9();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test9 = true;
        testResult9 = true;
        emit signalEnterStateTestSetBatteryVoltage9();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage9();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance9();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal9();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent9();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface9();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO9();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP9();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication9();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication9();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM9();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash9();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC9();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature9();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature9();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet9();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower9();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion9();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion9();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC9();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC9();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial9();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial9();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI9();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB9();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance9();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest9();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest9();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType9();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals10()
{
    switch(currentTest10)
    {
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage10();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent10();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC10();
        break;
    }
    case StateStartTest:
    {
        emit signalEnterStateStartTest10();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test10 = true;
        testResult10 = true;
        emit signalEnterStateTestSetBatteryVoltage10();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage10();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance10();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal10();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent10();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface10();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO10();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP10();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication10();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication10();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM10();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash10();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC10();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature10();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature10();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet10();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower10();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion10();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion10();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC10();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC10();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial10();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial10();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI10();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB10();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance10();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest10();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest10();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType10();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals11()
{
    switch(currentTest11)
    {
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage11();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent11();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC11();
        break;
    }
    case StateStartTest:
    {
        emit signalEnterStateStartTest11();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test11 = true;
        testResult11 = true;
        emit signalEnterStateTestSetBatteryVoltage11();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage11();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance11();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal11();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent11();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface11();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO11();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP11();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication11();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication11();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM11();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash11();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC11();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature11();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature11();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet11();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower11();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion11();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion11();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC11();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC11();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial11();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial11();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI11();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB11();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance11();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest11();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest11();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType11();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals12()
{
    switch(currentTest12)
    {
    case StateStartTest:
    {
        emit signalEnterStateStartTest12();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test12 = true;
        testResult12 = true;
        emit signalEnterStateTestSetBatteryVoltage12();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage12();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance12();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal12();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent12();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface12();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO12();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP12();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication12();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication12();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM12();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash12();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC12();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature12();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature12();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet12();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower12();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion12();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion12();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC12();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC12();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial12();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial12();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI12();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB12();
        break;
    }
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage12();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent12();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC12();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance12();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest12();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest12();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType12();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals13()
{
    switch(currentTest13)
    {
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage13();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent13();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC13();
        break;
    }
    case StateStartTest:
    {
        emit signalEnterStateStartTest13();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test13 = true;
        testResult13 = true;
        emit signalEnterStateTestSetBatteryVoltage13();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage13();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance13();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal13();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent13();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface13();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO13();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP13();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication13();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication13();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM13();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash13();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC13();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature13();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature13();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet13();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower13();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion13();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion13();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC13();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC13();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial13();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial13();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI13();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB13();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance13();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest13();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest13();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType13();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals14()
{
    switch(currentTest14)
    {
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage14();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent14();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC14();
        break;
    }
    case StateStartTest:
    {
        emit signalEnterStateStartTest14();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test14 = true;
        testResult14 = true;
        emit signalEnterStateTestSetBatteryVoltage14();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage14();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance14();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal14();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent14();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface14();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO14();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP14();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication14();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication14();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM14();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash14();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC14();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature14();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature14();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet14();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower14();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion14();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion14();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC14();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC14();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial14();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial14();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI14();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB14();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance14();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest14();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest14();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType14();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals15()
{
    switch(currentTest15)
    {
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage15();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent15();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC15();
        break;
    }
    case StateStartTest:
    {
        emit signalEnterStateStartTest15();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test15 = true;
        testResult15 = true;
        emit signalEnterStateTestSetBatteryVoltage15();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage15();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance15();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal15();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent15();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface15();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO15();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP15();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication15();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication15();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM15();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash15();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC15();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature15();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature15();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet15();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower15();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion15();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion15();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC15();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC15();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial15();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial15();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI15();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB15();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance15();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest15();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest15();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType15();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::emitSignals16()
{
    switch(currentTest16)
    {
    case StateTestBatteryVoltage:
    {
        emit signalEnterStateTestBatteryVoltage16();
        break;
    }
    case StateTestHALLCurrent:
    {
        emit signalEnterStateTestHALLCurrent16();
        break;
    }
    case StateTestSetRTC:
    {
        emit signalEnterStateTestSetRTC16();
        break;
    }
    case StateStartTest:
    {
        emit signalEnterStateStartTest16();
        break;
    }
    case StateTestSetBatteryVoltage:
    {
        if_test16 = true;
        testResult16 = true;
        emit signalEnterStateTestSetBatteryVoltage16();
        break;
    }
    case StateTestSupplyVoltage:
    {
        emit signalEnterStateTestSupplyVoltage16();
        break;
    }
    case StateTestInsulationResistance:
    {
        emit signalEnterStateTestInsulationResistance16();
        break;
    }
    case StateTestHALLSignal:
    {
        emit signalEnterStateTestHALLSignal16();
        break;
    }
    case StateTestSetHALLCurrent:
    {
        emit signalEnterStateTestSetHALLCurrent16();
        break;
    }
    case StateTestEncodeInterface:
    {
        emit signalEnterStateTestEncodeInterface16();
        break;
    }
    case StateTestDO:
    {
        emit signalEnterStateTestDO16();
        break;
    }
    case StateTestDIAndSLP:
    {
        emit signalEnterStateTestDIAndSLP16();
        break;
    }
    case StateTestSerialCommunication:
    {
        emit signalEnterStateTestSerialCommunication16();
        break;
    }
    case StateTestCANCommunication:
    {
        emit signalEnterStateTestCANCommunication16();
        break;
    }
    case StateTestEEPROM:
    {
        emit signalEnterStateTestEEPROM16();
        break;
    }
    case StateTestFlash:
    {
        emit signalEnterStateTestFlash16();
        break;
    }
    case StateTestRTC:
    {
        emit signalEnterStateTestRTC16();
        break;
    }
    case StateTestTemperature:
    {
        emit signalEnterStateTestTemperature16();
        break;
    }
    case StateTestSystemTemperature:
    {
        emit signalEnterStateTestSystemTemperature16();
        break;
    }
    case StateTestInternet:
    {
        emit signalEnterStateTestInternet16();
        break;
    }
    case StateTestPower:
    {
        emit signalEnterStateTestPower16();
        break;
    }
    case StateTestSoftWareVersion:
    {
        emit signalEnterStateTestSoftWareVersion16();
        break;
    }
    case StateTestHardWareVersion:
    {
        emit signalEnterStateTestHardWareVersion16();
        break;
    }
    case StateTestSetMAC:
    {
        emit signalEnterStateTestSetMAC16();
        break;
    }
    case StateTestReadMAC:
    {
        emit signalEnterStateTestReadMAC16();
        break;
    }
    case StateTestSetSerial:
    {
        emit signalEnterStateTestSetSerial16();
        break;
    }
    case StateTestReadSerial:
    {
        emit signalEnterStateTestReadSerial16();
        break;
    }
    case StateTestSPI:
    {
        emit signalEnterStateTestSPI16();
        break;
    }
    case StateTestKB:
    {
        emit signalEnterStateTestKB16();
        break;
    }
    case StateTestZDResistance:
    {
        emit signalEnterStateTestZDResistance16();
        break;
    }
    case StateFinishTest:
    {
        emit signalEnterStateFinishTest16();
        break;
    }
    case StateExitTest:
    {
        emit signalEnterStateExitTest16();
        break;
    }
    case StateUnknownType:
    {
        emit signalEnterStateUnknownType16();
        break;
    }
    default:
    {
        qDebug() << "error test type, exit";
        break;
    }
    }
}

void MainControl::slotOpenConnPage()
{
    if(chooseDeviceWidget)
    {
        delete chooseDeviceWidget;
        chooseDeviceWidget = NULL;
    }
    chooseDeviceWidget = new ChooseDeviceDialog(mainWindow, 1);
    connect(chooseDeviceWidget, SIGNAL(signalChooseDevice(int, int, int, int)),
            this, SLOT(slotStartCanBus(int, int, int, int)));
    chooseDeviceWidget->exec();
}

void MainControl::slotStartCanBus(int type, int id, int pass, int port)
{
    if(canbusControl)
    {
        delete canbusControl;
        canbusControl = NULL;
    }
    if(!canbusThread)
    {
        canbusThread = new QThread();
    }
    canbusControl = new CanBusControl(type, id, pass, port);
    canbusControl->setType(m_type1, m_type2, m_type3, m_type4, m_type5, m_type6, m_type7, m_type8
                           , m_type9, m_type10, m_type11, m_type12, m_type13, m_type14, m_type15, m_type16);
    canbusControl->moveToThread(canbusThread);
    connect(canbusControl, SIGNAL(signalChooseZLGDeviceType()), this, SLOT(slotChooseZLG()));
    connect(canbusControl, SIGNAL(signalChooseGCDeviceType()), this, SLOT(slotChooseGC()));
    connect(canbusControl, SIGNAL(signalReceiveStatus1(const QString &)),
            mainWindow, SLOT(slotAppendStatus1(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus2(const QString &)),
            mainWindow, SLOT(slotAppendStatus2(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus3(const QString &)),
            mainWindow, SLOT(slotAppendStatus3(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus4(const QString &)),
            mainWindow, SLOT(slotAppendStatus4(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus5(const QString &)),
            mainWindow, SLOT(slotAppendStatus5(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus6(const QString &)),
            mainWindow, SLOT(slotAppendStatus6(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus7(const QString &)),
            mainWindow, SLOT(slotAppendStatus7(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus8(const QString &)),
            mainWindow, SLOT(slotAppendStatus8(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus9(const QString &)),
            mainWindow, SLOT(slotAppendStatus9(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus10(const QString &)),
            mainWindow, SLOT(slotAppendStatus10(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus11(const QString &)),
            mainWindow, SLOT(slotAppendStatus11(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus12(const QString &)),
            mainWindow, SLOT(slotAppendStatus12(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus13(const QString &)),
            mainWindow, SLOT(slotAppendStatus13(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus14(const QString &)),
            mainWindow, SLOT(slotAppendStatus14(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus15(const QString &)),
            mainWindow, SLOT(slotAppendStatus15(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveStatus16(const QString &)),
            mainWindow, SLOT(slotAppendStatus16(const QString &)));
    connect(canbusControl, SIGNAL(signalReceiveErrorStatus(const QString &)),
            mainWindow, SLOT(slotShowErrorStatus(const QString &)));
    connect(mainWindow, SIGNAL(signalSetMac(const QString &)), canbusControl, SLOT(slotMac(const QString &)));
    connect(canbusControl, SIGNAL(signalErrorFinish1()), mainWindow, SLOT(slotErrorFinish1()));
    connect(canbusControl, SIGNAL(signalErrorFinish2()), mainWindow, SLOT(slotErrorFinish2()));
    connect(canbusControl, SIGNAL(signalErrorFinish3()), mainWindow, SLOT(slotErrorFinish3()));
    connect(canbusControl, SIGNAL(signalErrorFinish4()), mainWindow, SLOT(slotErrorFinish4()));
    connect(canbusControl, SIGNAL(signalErrorFinish5()), mainWindow, SLOT(slotErrorFinish5()));
    connect(canbusControl, SIGNAL(signalErrorFinish6()), mainWindow, SLOT(slotErrorFinish6()));
    connect(canbusControl, SIGNAL(signalErrorFinish7()), mainWindow, SLOT(slotErrorFinish7()));
    connect(canbusControl, SIGNAL(signalErrorFinish8()), mainWindow, SLOT(slotErrorFinish8()));
    connect(canbusControl, SIGNAL(signalErrorFinish9()), mainWindow, SLOT(slotErrorFinish9()));
    connect(canbusControl, SIGNAL(signalErrorFinish10()), mainWindow, SLOT(slotErrorFinish10()));
    connect(canbusControl, SIGNAL(signalErrorFinish11()), mainWindow, SLOT(slotErrorFinish11()));
    connect(canbusControl, SIGNAL(signalErrorFinish12()), mainWindow, SLOT(slotErrorFinish12()));
    connect(canbusControl, SIGNAL(signalErrorFinish13()), mainWindow, SLOT(slotErrorFinish13()));
    connect(canbusControl, SIGNAL(signalErrorFinish14()), mainWindow, SLOT(slotErrorFinish14()));
    connect(canbusControl, SIGNAL(signalErrorFinish15()), mainWindow, SLOT(slotErrorFinish15()));
    connect(canbusControl, SIGNAL(signalErrorFinish16()), mainWindow, SLOT(slotErrorFinish16()));


    connect(mainWindow, SIGNAL(signalTestResult1(bool)), canbusControl, SLOT(slotTestResult1(bool)));
    connect(mainWindow, SIGNAL(signalTestResult2(bool)), canbusControl, SLOT(slotTestResult2(bool)));
    connect(mainWindow, SIGNAL(signalTestResult3(bool)), canbusControl, SLOT(slotTestResult3(bool)));
    connect(mainWindow, SIGNAL(signalTestResult4(bool)), canbusControl, SLOT(slotTestResult4(bool)));
    connect(mainWindow, SIGNAL(signalTestResult5(bool)), canbusControl, SLOT(slotTestResult5(bool)));
    connect(mainWindow, SIGNAL(signalTestResult6(bool)), canbusControl, SLOT(slotTestResult6(bool)));
    connect(mainWindow, SIGNAL(signalTestResult7(bool)), canbusControl, SLOT(slotTestResult7(bool)));
    connect(mainWindow, SIGNAL(signalTestResult8(bool)), canbusControl, SLOT(slotTestResult8(bool)));
    connect(mainWindow, SIGNAL(signalTestResult9(bool)), canbusControl, SLOT(slotTestResult9(bool)));
    connect(mainWindow, SIGNAL(signalTestResult10(bool)), canbusControl, SLOT(slotTestResult10(bool)));
    connect(mainWindow, SIGNAL(signalTestResult11(bool)), canbusControl, SLOT(slotTestResult11(bool)));
    connect(mainWindow, SIGNAL(signalTestResult12(bool)), canbusControl, SLOT(slotTestResult12(bool)));
    connect(mainWindow, SIGNAL(signalTestResult13(bool)), canbusControl, SLOT(slotTestResult13(bool)));
    connect(mainWindow, SIGNAL(signalTestResult14(bool)), canbusControl, SLOT(slotTestResult14(bool)));
    connect(mainWindow, SIGNAL(signalTestResult15(bool)), canbusControl, SLOT(slotTestResult15(bool)));
    connect(mainWindow, SIGNAL(signalTestResult16(bool)), canbusControl, SLOT(slotTestResult16(bool)));

    connect(canbusControl, SIGNAL(signalErrorFinish1()), this, SLOT(slotErrorFinish1()));
    connect(canbusControl, SIGNAL(signalErrorFinish2()), this, SLOT(slotErrorFinish2()));
    connect(canbusControl, SIGNAL(signalErrorFinish3()), this, SLOT(slotErrorFinish3()));
    connect(canbusControl, SIGNAL(signalErrorFinish4()), this, SLOT(slotErrorFinish4()));
    connect(canbusControl, SIGNAL(signalErrorFinish5()), this, SLOT(slotErrorFinish5()));
    connect(canbusControl, SIGNAL(signalErrorFinish6()), this, SLOT(slotErrorFinish6()));
    connect(canbusControl, SIGNAL(signalErrorFinish7()), this, SLOT(slotErrorFinish7()));
    connect(canbusControl, SIGNAL(signalErrorFinish8()), this, SLOT(slotErrorFinish8()));
    connect(canbusControl, SIGNAL(signalErrorFinish9()), this, SLOT(slotErrorFinish9()));
    connect(canbusControl, SIGNAL(signalErrorFinish10()), this, SLOT(slotErrorFinish10()));
    connect(canbusControl, SIGNAL(signalErrorFinish11()), this, SLOT(slotErrorFinish11()));
    connect(canbusControl, SIGNAL(signalErrorFinish12()), this, SLOT(slotErrorFinish12()));
    connect(canbusControl, SIGNAL(signalErrorFinish13()), this, SLOT(slotErrorFinish13()));
    connect(canbusControl, SIGNAL(signalErrorFinish14()), this, SLOT(slotErrorFinish14()));
    connect(canbusControl, SIGNAL(signalErrorFinish15()), this, SLOT(slotErrorFinish15()));
    connect(canbusControl, SIGNAL(signalErrorFinish16()), this, SLOT(slotErrorFinish16()));
    connect(canbusControl, SIGNAL(signalFinish1()), mainWindow, SLOT(slotFinish1()));
    connect(canbusControl, SIGNAL(signalFinish2()), mainWindow, SLOT(slotFinish2()));
    connect(canbusControl, SIGNAL(signalFinish3()), mainWindow, SLOT(slotFinish3()));
    connect(canbusControl, SIGNAL(signalFinish4()), mainWindow, SLOT(slotFinish4()));
    connect(canbusControl, SIGNAL(signalFinish5()), mainWindow, SLOT(slotFinish5()));
    connect(canbusControl, SIGNAL(signalFinish6()), mainWindow, SLOT(slotFinish6()));
    connect(canbusControl, SIGNAL(signalFinish7()), mainWindow, SLOT(slotFinish7()));
    connect(canbusControl, SIGNAL(signalFinish8()), mainWindow, SLOT(slotFinish8()));
    connect(canbusControl, SIGNAL(signalFinish9()), mainWindow, SLOT(slotFinish9()));
    connect(canbusControl, SIGNAL(signalFinish10()), mainWindow, SLOT(slotFinish10()));
    connect(canbusControl, SIGNAL(signalFinish11()), mainWindow, SLOT(slotFinish11()));
    connect(canbusControl, SIGNAL(signalFinish12()), mainWindow, SLOT(slotFinish12()));
    connect(canbusControl, SIGNAL(signalFinish13()), mainWindow, SLOT(slotFinish13()));
    connect(canbusControl, SIGNAL(signalFinish14()), mainWindow, SLOT(slotFinish14()));
    connect(canbusControl, SIGNAL(signalFinish15()), mainWindow, SLOT(slotFinish15()));
    connect(canbusControl, SIGNAL(signalFinish16()), mainWindow, SLOT(slotFinish16()));
    connect(canbusControl, SIGNAL(signalSetExcelItem1(std::vector<ExcelItem>)),
            excelControl1, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem2(std::vector<ExcelItem>)),
            excelControl2, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem3(std::vector<ExcelItem>)),
            excelControl3, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem4(std::vector<ExcelItem>)),
            excelControl4, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem5(std::vector<ExcelItem>)),
            excelControl5, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem6(std::vector<ExcelItem>)),
            excelControl6, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem7(std::vector<ExcelItem>)),
            excelControl7, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem8(std::vector<ExcelItem>)),
            excelControl8, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem9(std::vector<ExcelItem>)),
            excelControl9, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem10(std::vector<ExcelItem>)),
            excelControl10, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem11(std::vector<ExcelItem>)),
            excelControl11, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem12(std::vector<ExcelItem>)),
            excelControl12, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem13(std::vector<ExcelItem>)),
            excelControl13, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem14(std::vector<ExcelItem>)),
            excelControl14, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem15(std::vector<ExcelItem>)),
            excelControl15, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalSetExcelItem16(std::vector<ExcelItem>)),
               excelControl16, SLOT(slotSetExcel(std::vector<ExcelItem>)));
    connect(canbusControl, SIGNAL(signalReceiveSerial(const QString &)),
               this, SLOT(slotSetSerial(const QString &)));
    connect(this, SIGNAL(signalSetCurrentTestStep1(int)),
               canbusControl, SLOT(slotSetCurrentTestStep1(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep2(int)),
               canbusControl, SLOT(slotSetCurrentTestStep2(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep3(int)),
               canbusControl, SLOT(slotSetCurrentTestStep3(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep4(int)),
               canbusControl, SLOT(slotSetCurrentTestStep4(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep5(int)),
               canbusControl, SLOT(slotSetCurrentTestStep5(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep6(int)),
               canbusControl, SLOT(slotSetCurrentTestStep6(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep7(int)),
               canbusControl, SLOT(slotSetCurrentTestStep7(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep8(int)),
               canbusControl, SLOT(slotSetCurrentTestStep8(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep9(int)),
               canbusControl, SLOT(slotSetCurrentTestStep9(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep10(int)),
               canbusControl, SLOT(slotSetCurrentTestStep10(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep11(int)),
               canbusControl, SLOT(slotSetCurrentTestStep11(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep12(int)),
               canbusControl, SLOT(slotSetCurrentTestStep12(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep13(int)),
               canbusControl, SLOT(slotSetCurrentTestStep13(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep14(int)),
               canbusControl, SLOT(slotSetCurrentTestStep14(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep15(int)),
               canbusControl, SLOT(slotSetCurrentTestStep15(int)));
    connect(this, SIGNAL(signalSetCurrentTestStep16(int)),
               canbusControl, SLOT(slotSetCurrentTestStep16(int)));
    connect(this, SIGNAL(signalUpdateTestItem1()),
               canbusControl, SLOT(updateTestItem1()));
    connect(this, SIGNAL(signalUpdateTestItem2()),
               canbusControl, SLOT(updateTestItem2()));
    connect(this, SIGNAL(signalUpdateTestItem3()),
               canbusControl, SLOT(updateTestItem3()));
    connect(this, SIGNAL(signalUpdateTestItem4()),
               canbusControl, SLOT(updateTestItem4()));
    connect(this, SIGNAL(signalUpdateTestItem5()),
               canbusControl, SLOT(updateTestItem5()));
    connect(this, SIGNAL(signalUpdateTestItem6()),
               canbusControl, SLOT(updateTestItem6()));
    connect(this, SIGNAL(signalUpdateTestItem7()),
               canbusControl, SLOT(updateTestItem7()));
    connect(this, SIGNAL(signalUpdateTestItem8()),
               canbusControl, SLOT(updateTestItem8()));
    connect(this, SIGNAL(signalUpdateTestItem9()),
               canbusControl, SLOT(updateTestItem9()));
    connect(this, SIGNAL(signalUpdateTestItem10()),
               canbusControl, SLOT(updateTestItem10()));
    connect(this, SIGNAL(signalUpdateTestItem11()),
               canbusControl, SLOT(updateTestItem11()));
    connect(this, SIGNAL(signalUpdateTestItem12()),
               canbusControl, SLOT(updateTestItem12()));
    connect(this, SIGNAL(signalUpdateTestItem13()),
               canbusControl, SLOT(updateTestItem13()));
    connect(this, SIGNAL(signalUpdateTestItem14()),
               canbusControl, SLOT(updateTestItem14()));
    connect(this, SIGNAL(signalUpdateTestItem15()),
               canbusControl, SLOT(updateTestItem15()));
    connect(this, SIGNAL(signalUpdateTestItem16()),
               canbusControl, SLOT(updateTestItem16()));
    connect(mainWindow, SIGNAL(signalReadKB(int, int, int)), canbusControl, SLOT(slotReadKB(int, int, int)));
    connect(mainWindow, SIGNAL(signalSetKB(int, int, float, float, int)),
            canbusControl, SLOT(slotSetKB(int, int, float, float, int)));
    connect(canbusControl, SIGNAL(signalGetKB(float, float)), mainWindow, SLOT(slotGetKB(float, float)));
    connect(mainWindow, SIGNAL(signalReadInfo(int, int)), canbusControl, SLOT(slotReadInfo(int, int)));
    connect(canbusControl, SIGNAL(signalGetInfo(int, float, int)), mainWindow, SLOT(slotGetInfo(int, float, int)));
    connect(mainWindow, SIGNAL(signalOpenDO(int)), canbusControl, SLOT(slotOpenDO(int)));
    connect(mainWindow, SIGNAL(signalCloseDO(int)), canbusControl, SLOT(slotCloseDO(int)));
    connect(mainWindow, SIGNAL(signalSetSoft(const QString &)), canbusControl, SLOT(slotSetSoft(const QString &)));
    connect(mainWindow, SIGNAL(signalSetHard(const QString &)), canbusControl, SLOT(slotSetHard(const QString &)));
    connect(canbusControl, SIGNAL(signalEEPROMFinish()), mainWindow, SLOT(slotEEPROMFinish()));
    connect(canbusControl, SIGNAL(signalEEPROMErrorFinish()), mainWindow, SLOT(slotEEPROMErrorFinish()));
    connect(canbusControl, SIGNAL(signalFlashFinish()), mainWindow, SLOT(slotFlashFinish()));
    connect(canbusControl, SIGNAL(signalFlashErrorFinish()), mainWindow, SLOT(slotFlashErrorFinish()));
    connect(canbusControl, SIGNAL(signalRTCFinish()), mainWindow, SLOT(slotRTCFinish()));
    connect(canbusControl, SIGNAL(signalRTCErrorFinish()), mainWindow, SLOT(slotRTCErrorFinish()));
    connect(canbusControl, SIGNAL(signalIOErrorFinish()), mainWindow, SLOT(slotIOErrorFinish()));
    connect(canbusControl, SIGNAL(signalIOFinish()), mainWindow, SLOT(slotIOFinish()));
    connect(canbusControl, SIGNAL(signalDIErrorFinish()), mainWindow, SLOT(slotDIErrorFinish()));
    connect(canbusControl, SIGNAL(signalDOErrorFinish()), mainWindow, SLOT(slotDOErrorFinish()));
    connect(canbusControl, SIGNAL(signalDIFinish()), mainWindow, SLOT(slotDIFinish()));
    connect(canbusControl, SIGNAL(signalDOFinish()), mainWindow, SLOT(slotDOFinish()));
    connect(canbusControl, SIGNAL(signalCANFinish()), mainWindow, SLOT(slotCANFinish()));
    connect(canbusControl, SIGNAL(signalCANErrorFinish()), mainWindow, SLOT(slotCANErrorFinish()));
    connect(canbusControl, SIGNAL(signalSerialFinish()), mainWindow, SLOT(slotSerialFinish()));
    connect(canbusControl, SIGNAL(signalSerialErrorFinish()), mainWindow, SLOT(slotSerialErrorFinish()));
    connect(canbusControl, SIGNAL(signalHALLCurrentFinish()), mainWindow, SLOT(slotHALLCurrentFinish()));
    connect(canbusControl, SIGNAL(signalHALLCurrentErrorFinish()), mainWindow, SLOT(slotHALLCurrentErrorFinish()));
    connect(canbusControl, SIGNAL(signalBatteryVoltageFinish()), mainWindow, SLOT(slotBatteryVoltageFinish()));
    connect(canbusControl, SIGNAL(signalBatteryVoltageErrorFinish()), mainWindow, SLOT(slotBatteryVoltageErrorFinish()));
    connect(canbusControl, SIGNAL(signalSupplyVoltageFinish()), mainWindow, SLOT(slotSupplyVoltageFinish()));
    connect(canbusControl, SIGNAL(signalSupplyVoltageErrorFinish()), mainWindow, SLOT(slotSupplyVoltageErrorFinish()));
    connect(canbusControl, SIGNAL(signalSystemTemperatureFinish()), mainWindow, SLOT(slotSystemTemperatureFinish()));
    connect(canbusControl, SIGNAL(signalSystemTemperatureErrorFinish()), mainWindow, SLOT(slotSystemTemperatureErrorFinish()));
    connect(canbusControl, SIGNAL(signalInsulationResistanceFinish()), mainWindow, SLOT(slotInsulationResistanceFinish()));
    connect(canbusControl, SIGNAL(signalInsulationResistanceErrorFinish()), mainWindow, SLOT(slotInsulationResistanceErrorFinish()));
    connect(canbusControl, SIGNAL(signalPowerFinish()), mainWindow, SLOT(slotPowerFinish()));
    connect(canbusControl, SIGNAL(signalPowerErrorFinish()), mainWindow, SLOT(slotPowerErrorFinish()));
    connect(canbusControl, SIGNAL(signalHALLSignalFinish()), mainWindow, SLOT(slotHALLSignalFinish()));
    connect(canbusControl, SIGNAL(signalHALLSignalErrorFinish()), mainWindow, SLOT(slotHALLSignalErrorFinish()));
    connect(canbusControl, SIGNAL(signalTemperatureFinish()), mainWindow, SLOT(slotTemperatureFinish()));
    connect(canbusControl, SIGNAL(signalTemperatureErrorFinish()), mainWindow, SLOT(slotTemperatureErrorFinish()));
    connect(canbusControl, SIGNAL(signalFinishSet1()), this, SLOT(slotFinishSet1()));
    connect(canbusControl, SIGNAL(signalFinishSet2()), this, SLOT(slotFinishSet2()));
    connect(canbusControl, SIGNAL(signalFinishSet3()), this, SLOT(slotFinishSet3()));
    connect(canbusControl, SIGNAL(signalFinishSet4()), this, SLOT(slotFinishSet4()));
    connect(canbusControl, SIGNAL(signalFinishSet5()), this, SLOT(slotFinishSet5()));
    connect(canbusControl, SIGNAL(signalFinishSet6()), this, SLOT(slotFinishSet6()));
    connect(canbusControl, SIGNAL(signalFinishSet7()), this, SLOT(slotFinishSet7()));
    connect(canbusControl, SIGNAL(signalFinishSet8()), this, SLOT(slotFinishSet8()));
    connect(canbusControl, SIGNAL(signalFinishSet9()), this, SLOT(slotFinishSet9()));
    connect(canbusControl, SIGNAL(signalFinishSet10()), this, SLOT(slotFinishSet10()));
    connect(canbusControl, SIGNAL(signalFinishSet11()), this, SLOT(slotFinishSet11()));
    connect(canbusControl, SIGNAL(signalFinishSet12()), this, SLOT(slotFinishSet12()));
    connect(canbusControl, SIGNAL(signalFinishSet13()), this, SLOT(slotFinishSet13()));
    connect(canbusControl, SIGNAL(signalFinishSet14()), this, SLOT(slotFinishSet14()));
    connect(canbusControl, SIGNAL(signalFinishSet15()), this, SLOT(slotFinishSet15()));
    connect(canbusControl, SIGNAL(signalFinishSet16()), this, SLOT(slotFinishSet16()));

    connect(canbusControl, SIGNAL(signalFinishSet21()), this, SLOT(slotFinishSet21()));
    connect(canbusControl, SIGNAL(signalFinishSet22()), this, SLOT(slotFinishSet22()));
    connect(canbusControl, SIGNAL(signalFinishSet23()), this, SLOT(slotFinishSet23()));
    connect(canbusControl, SIGNAL(signalFinishSet24()), this, SLOT(slotFinishSet24()));
    connect(canbusControl, SIGNAL(signalFinishSet25()), this, SLOT(slotFinishSet25()));
    connect(canbusControl, SIGNAL(signalFinishSet26()), this, SLOT(slotFinishSet26()));
    connect(canbusControl, SIGNAL(signalFinishSet27()), this, SLOT(slotFinishSet27()));
    connect(canbusControl, SIGNAL(signalFinishSet28()), this, SLOT(slotFinishSet28()));
    connect(canbusControl, SIGNAL(signalFinishSet29()), this, SLOT(slotFinishSet29()));
    connect(canbusControl, SIGNAL(signalFinishSet210()), this, SLOT(slotFinishSet210()));
    connect(canbusControl, SIGNAL(signalFinishSet211()), this, SLOT(slotFinishSet211()));
    connect(canbusControl, SIGNAL(signalFinishSet212()), this, SLOT(slotFinishSet212()));
    connect(canbusControl, SIGNAL(signalFinishSet213()), this, SLOT(slotFinishSet213()));
    connect(canbusControl, SIGNAL(signalFinishSet214()), this, SLOT(slotFinishSet214()));
    connect(canbusControl, SIGNAL(signalFinishSet215()), this, SLOT(slotFinishSet215()));
    connect(canbusControl, SIGNAL(signalFinishSet216()), this, SLOT(slotFinishSet216()));


    connect(canbusControl, SIGNAL(signalSetSerial1(QString)), mainWindow, SLOT(slotReceiveSerial1(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial2(QString)), mainWindow, SLOT(slotReceiveSerial2(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial3(QString)), mainWindow, SLOT(slotReceiveSerial3(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial4(QString)), mainWindow, SLOT(slotReceiveSerial4(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial5(QString)), mainWindow, SLOT(slotReceiveSerial5(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial6(QString)), mainWindow, SLOT(slotReceiveSerial6(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial7(QString)), mainWindow, SLOT(slotReceiveSerial7(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial8(QString)), mainWindow, SLOT(slotReceiveSerial8(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial9(QString)), mainWindow, SLOT(slotReceiveSerial9(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial10(QString)), mainWindow, SLOT(slotReceiveSerial10(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial11(QString)), mainWindow, SLOT(slotReceiveSerial11(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial12(QString)), mainWindow, SLOT(slotReceiveSerial12(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial13(QString)), mainWindow, SLOT(slotReceiveSerial13(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial14(QString)), mainWindow, SLOT(slotReceiveSerial14(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial15(QString)), mainWindow, SLOT(slotReceiveSerial15(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial16(QString)), mainWindow, SLOT(slotReceiveSerial16(QString)));

    connect(canbusControl, SIGNAL(signalSetSerial1(QString)), this, SLOT(slotReceiveSerial1(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial2(QString)), this, SLOT(slotReceiveSerial2(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial3(QString)), this, SLOT(slotReceiveSerial3(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial4(QString)), this, SLOT(slotReceiveSerial4(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial5(QString)), this, SLOT(slotReceiveSerial5(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial6(QString)), this, SLOT(slotReceiveSerial6(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial7(QString)), this, SLOT(slotReceiveSerial7(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial8(QString)), this, SLOT(slotReceiveSerial8(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial9(QString)), this, SLOT(slotReceiveSerial9(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial10(QString)), this, SLOT(slotReceiveSerial10(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial11(QString)), this, SLOT(slotReceiveSerial11(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial12(QString)), this, SLOT(slotReceiveSerial12(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial13(QString)), this, SLOT(slotReceiveSerial13(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial14(QString)), this, SLOT(slotReceiveSerial14(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial15(QString)), this, SLOT(slotReceiveSerial15(QString)));
    connect(canbusControl, SIGNAL(signalSetSerial16(QString)), this, SLOT(slotReceiveSerial16(QString)));

    connect(canbusControl, SIGNAL(signalSetMAC1(QString)), mainWindow, SLOT(slotReceiveMAC1(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC2(QString)), mainWindow, SLOT(slotReceiveMAC2(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC3(QString)), mainWindow, SLOT(slotReceiveMAC3(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC4(QString)), mainWindow, SLOT(slotReceiveMAC4(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC5(QString)), mainWindow, SLOT(slotReceiveMAC5(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC6(QString)), mainWindow, SLOT(slotReceiveMAC6(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC7(QString)), mainWindow, SLOT(slotReceiveMAC7(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC8(QString)), mainWindow, SLOT(slotReceiveMAC8(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC9(QString)), mainWindow, SLOT(slotReceiveMAC9(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC10(QString)), mainWindow, SLOT(slotReceiveMAC10(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC11(QString)), mainWindow, SLOT(slotReceiveMAC11(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC12(QString)), mainWindow, SLOT(slotReceiveMAC12(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC13(QString)), mainWindow, SLOT(slotReceiveMAC13(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC14(QString)), mainWindow, SLOT(slotReceiveMAC14(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC15(QString)), mainWindow, SLOT(slotReceiveMAC15(QString)));
    connect(canbusControl, SIGNAL(signalSetMAC16(QString)), mainWindow, SLOT(slotReceiveMAC16(QString)));


    connect(canbusControl, SIGNAL(signalStartTest1()), mainWindow, SLOT(slotStartTest1()));
    connect(canbusControl, SIGNAL(signalStartTest2()), mainWindow, SLOT(slotStartTest2()));
    connect(canbusControl, SIGNAL(signalStartTest3()), mainWindow, SLOT(slotStartTest3()));
    connect(canbusControl, SIGNAL(signalStartTest4()), mainWindow, SLOT(slotStartTest4()));
    connect(canbusControl, SIGNAL(signalStartTest5()), mainWindow, SLOT(slotStartTest5()));
    connect(canbusControl, SIGNAL(signalStartTest6()), mainWindow, SLOT(slotStartTest6()));
    connect(canbusControl, SIGNAL(signalStartTest7()), mainWindow, SLOT(slotStartTest7()));
    connect(canbusControl, SIGNAL(signalStartTest8()), mainWindow, SLOT(slotStartTest8()));
    connect(canbusControl, SIGNAL(signalStartTest9()), mainWindow, SLOT(slotStartTest9()));
    connect(canbusControl, SIGNAL(signalStartTest10()), mainWindow, SLOT(slotStartTest10()));
    connect(canbusControl, SIGNAL(signalStartTest11()), mainWindow, SLOT(slotStartTest11()));
    connect(canbusControl, SIGNAL(signalStartTest12()), mainWindow, SLOT(slotStartTest12()));
    connect(canbusControl, SIGNAL(signalStartTest13()), mainWindow, SLOT(slotStartTest13()));
    connect(canbusControl, SIGNAL(signalStartTest14()), mainWindow, SLOT(slotStartTest14()));
    connect(canbusControl, SIGNAL(signalStartTest15()), mainWindow, SLOT(slotStartTest15()));
    connect(canbusControl, SIGNAL(signalStartTest16()), mainWindow, SLOT(slotStartTest16()));

    connect(canbusControl, SIGNAL(signalSetModFail1()), mainWindow, SLOT(slotSetModFail1()));
    connect(canbusControl, SIGNAL(signalSetModFail2()), mainWindow, SLOT(slotSetModFail2()));
    connect(canbusControl, SIGNAL(signalSetModFail3()), mainWindow, SLOT(slotSetModFail3()));
    connect(canbusControl, SIGNAL(signalSetModFail4()), mainWindow, SLOT(slotSetModFail4()));
    connect(canbusControl, SIGNAL(signalSetModFail5()), mainWindow, SLOT(slotSetModFail5()));
    connect(canbusControl, SIGNAL(signalSetModFail6()), mainWindow, SLOT(slotSetModFail6()));
    connect(canbusControl, SIGNAL(signalSetModFail7()), mainWindow, SLOT(slotSetModFail7()));
    connect(canbusControl, SIGNAL(signalSetModFail8()), mainWindow, SLOT(slotSetModFail8()));
    connect(canbusControl, SIGNAL(signalSetModFail9()), mainWindow, SLOT(slotSetModFail9()));
    connect(canbusControl, SIGNAL(signalSetModFail10()), mainWindow, SLOT(slotSetModFail10()));
    connect(canbusControl, SIGNAL(signalSetModFail11()), mainWindow, SLOT(slotSetModFail11()));
    connect(canbusControl, SIGNAL(signalSetModFail12()), mainWindow, SLOT(slotSetModFail12()));
    connect(canbusControl, SIGNAL(signalSetModFail13()), mainWindow, SLOT(slotSetModFail13()));
    connect(canbusControl, SIGNAL(signalSetModFail14()), mainWindow, SLOT(slotSetModFail14()));
    connect(canbusControl, SIGNAL(signalSetModFail15()), mainWindow, SLOT(slotSetModFail15()));
    connect(canbusControl, SIGNAL(signalSetModFail16()), mainWindow, SLOT(slotSetModFail16()));
    connect(canbusControl, SIGNAL(signalSetModFail21()), mainWindow, SLOT(slotSetModFail1()));
    connect(canbusControl, SIGNAL(signalSetModFail22()), mainWindow, SLOT(slotSetModFail2()));
    connect(canbusControl, SIGNAL(signalSetModFail23()), mainWindow, SLOT(slotSetModFail3()));
    connect(canbusControl, SIGNAL(signalSetModFail24()), mainWindow, SLOT(slotSetModFail4()));
    connect(canbusControl, SIGNAL(signalSetModFail25()), mainWindow, SLOT(slotSetModFail5()));
    connect(canbusControl, SIGNAL(signalSetModFail26()), mainWindow, SLOT(slotSetModFail6()));
    connect(canbusControl, SIGNAL(signalSetModFail27()), mainWindow, SLOT(slotSetModFail7()));
    connect(canbusControl, SIGNAL(signalSetModFail28()), mainWindow, SLOT(slotSetModFail8()));
    connect(canbusControl, SIGNAL(signalSetModFail29()), mainWindow, SLOT(slotSetModFail9()));
    connect(canbusControl, SIGNAL(signalSetModFail210()), mainWindow, SLOT(slotSetModFail10()));
    connect(canbusControl, SIGNAL(signalSetModFail211()), mainWindow, SLOT(slotSetModFail11()));
    connect(canbusControl, SIGNAL(signalSetModFail212()), mainWindow, SLOT(slotSetModFail12()));
    connect(canbusControl, SIGNAL(signalSetModFail213()), mainWindow, SLOT(slotSetModFail13()));
    connect(canbusControl, SIGNAL(signalSetModFail214()), mainWindow, SLOT(slotSetModFail14()));
    connect(canbusControl, SIGNAL(signalSetModFail215()), mainWindow, SLOT(slotSetModFail15()));
    connect(canbusControl, SIGNAL(signalSetModFail216()), mainWindow, SLOT(slotSetModFail16()));

    connect(canbusControl, SIGNAL(signalSetModFail1()), this, SLOT(slotSingleFail1()));
    connect(canbusControl, SIGNAL(signalSetModFail2()), this, SLOT(slotSingleFail2()));
    connect(canbusControl, SIGNAL(signalSetModFail3()), this, SLOT(slotSingleFail3()));
    connect(canbusControl, SIGNAL(signalSetModFail4()), this, SLOT(slotSingleFail4()));
    connect(canbusControl, SIGNAL(signalSetModFail5()), this, SLOT(slotSingleFail5()));
    connect(canbusControl, SIGNAL(signalSetModFail6()), this, SLOT(slotSingleFail6()));
    connect(canbusControl, SIGNAL(signalSetModFail7()), this, SLOT(slotSingleFail7()));
    connect(canbusControl, SIGNAL(signalSetModFail8()), this, SLOT(slotSingleFail8()));
    connect(canbusControl, SIGNAL(signalSetModFail9()), this, SLOT(slotSingleFail9()));
    connect(canbusControl, SIGNAL(signalSetModFail10()), this, SLOT(slotSingleFail10()));
    connect(canbusControl, SIGNAL(signalSetModFail11()), this, SLOT(slotSingleFail11()));
    connect(canbusControl, SIGNAL(signalSetModFail12()), this, SLOT(slotSingleFail12()));
    connect(canbusControl, SIGNAL(signalSetModFail13()), this, SLOT(slotSingleFail13()));
    connect(canbusControl, SIGNAL(signalSetModFail14()), this, SLOT(slotSingleFail14()));
    connect(canbusControl, SIGNAL(signalSetModFail15()), this, SLOT(slotSingleFail15()));
    connect(canbusControl, SIGNAL(signalSetModFail16()), this, SLOT(slotSingleFail16()));
    connect(canbusControl, SIGNAL(signalSetModFail21()), this, SLOT(slotSingleFail21()));
    connect(canbusControl, SIGNAL(signalSetModFail22()), this, SLOT(slotSingleFail22()));
    connect(canbusControl, SIGNAL(signalSetModFail23()), this, SLOT(slotSingleFail23()));
    connect(canbusControl, SIGNAL(signalSetModFail24()), this, SLOT(slotSingleFail24()));
    connect(canbusControl, SIGNAL(signalSetModFail25()), this, SLOT(slotSingleFail25()));
    connect(canbusControl, SIGNAL(signalSetModFail26()), this, SLOT(slotSingleFail26()));
    connect(canbusControl, SIGNAL(signalSetModFail27()), this, SLOT(slotSingleFail27()));
    connect(canbusControl, SIGNAL(signalSetModFail28()), this, SLOT(slotSingleFail28()));
    connect(canbusControl, SIGNAL(signalSetModFail29()), this, SLOT(slotSingleFail29()));
    connect(canbusControl, SIGNAL(signalSetModFail210()), this, SLOT(slotSingleFail210()));
    connect(canbusControl, SIGNAL(signalSetModFail211()), this, SLOT(slotSingleFail211()));
    connect(canbusControl, SIGNAL(signalSetModFail212()), this, SLOT(slotSingleFail212()));
    connect(canbusControl, SIGNAL(signalSetModFail213()), this, SLOT(slotSingleFail213()));
    connect(canbusControl, SIGNAL(signalSetModFail214()), this, SLOT(slotSingleFail214()));
    connect(canbusControl, SIGNAL(signalSetModFail215()), this, SLOT(slotSingleFail215()));
    connect(canbusControl, SIGNAL(signalSetModFail216()), this, SLOT(slotSingleFail216()));

    connect(canbusControl, SIGNAL(signalTimeout1()), this, SLOT(slotTimeout1()));
    connect(canbusControl, SIGNAL(signalTimeout2()), this, SLOT(slotTimeout2()));
    connect(canbusControl, SIGNAL(signalTimeout3()), this, SLOT(slotTimeout3()));
    connect(canbusControl, SIGNAL(signalTimeout4()), this, SLOT(slotTimeout4()));
    connect(canbusControl, SIGNAL(signalTimeout5()), this, SLOT(slotTimeout5()));
    connect(canbusControl, SIGNAL(signalTimeout6()), this, SLOT(slotTimeout6()));
    connect(canbusControl, SIGNAL(signalTimeout7()), this, SLOT(slotTimeout7()));
    connect(canbusControl, SIGNAL(signalTimeout8()), this, SLOT(slotTimeout8()));
    connect(canbusControl, SIGNAL(signalTimeout9()), this, SLOT(slotTimeout9()));
    connect(canbusControl, SIGNAL(signalTimeout10()), this, SLOT(slotTimeout10()));
    connect(canbusControl, SIGNAL(signalTimeout11()), this, SLOT(slotTimeout11()));
    connect(canbusControl, SIGNAL(signalTimeout12()), this, SLOT(slotTimeout12()));
    connect(canbusControl, SIGNAL(signalTimeout13()), this, SLOT(slotTimeout13()));
    connect(canbusControl, SIGNAL(signalTimeout14()), this, SLOT(slotTimeout14()));
    connect(canbusControl, SIGNAL(signalTimeout15()), this, SLOT(slotTimeout15()));
    connect(canbusControl, SIGNAL(signalTimeout16()), this, SLOT(slotTimeout16()));

    connect(canbusControl, SIGNAL(signalSerialError1()), mainWindow, SLOT(slotSerialError1()));
    connect(canbusControl, SIGNAL(signalSerialError2()), mainWindow, SLOT(slotSerialError2()));
    connect(canbusControl, SIGNAL(signalSerialError3()), mainWindow, SLOT(slotSerialError3()));
    connect(canbusControl, SIGNAL(signalSerialError4()), mainWindow, SLOT(slotSerialError4()));
    connect(canbusControl, SIGNAL(signalSerialError5()), mainWindow, SLOT(slotSerialError5()));
    connect(canbusControl, SIGNAL(signalSerialError6()), mainWindow, SLOT(slotSerialError6()));
    connect(canbusControl, SIGNAL(signalSerialError7()), mainWindow, SLOT(slotSerialError7()));
    connect(canbusControl, SIGNAL(signalSerialError8()), mainWindow, SLOT(slotSerialError8()));
    connect(canbusControl, SIGNAL(signalSerialError9()), mainWindow, SLOT(slotSerialError9()));
    connect(canbusControl, SIGNAL(signalSerialError10()), mainWindow, SLOT(slotSerialError10()));
    connect(canbusControl, SIGNAL(signalSerialError11()), mainWindow, SLOT(slotSerialError11()));
    connect(canbusControl, SIGNAL(signalSerialError12()), mainWindow, SLOT(slotSerialError12()));
    connect(canbusControl, SIGNAL(signalSerialError13()), mainWindow, SLOT(slotSerialError13()));
    connect(canbusControl, SIGNAL(signalSerialError14()), mainWindow, SLOT(slotSerialError14()));
    connect(canbusControl, SIGNAL(signalSerialError15()), mainWindow, SLOT(slotSerialError15()));
    connect(canbusControl, SIGNAL(signalSerialError16()), mainWindow, SLOT(slotSerialError16()));
    connect(mainWindow, SIGNAL(signalChange(int, int)), canbusControl, SLOT(slotChange(int, int)));

    if(m_mod)
    {
        connect(canbusControl, SIGNAL(signalSendTestResult1(QJsonObject)),
                   this, SLOT(slotAddJson1(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult2(QJsonObject)),
                   this, SLOT(slotAddJson2(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult3(QJsonObject)),
                   this, SLOT(slotAddJson3(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult4(QJsonObject)),
                   this, SLOT(slotAddJson4(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult5(QJsonObject)),
                   this, SLOT(slotAddJson5(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult6(QJsonObject)),
                   this, SLOT(slotAddJson6(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult7(QJsonObject)),
                   this, SLOT(slotAddJson7(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult8(QJsonObject)),
                   this, SLOT(slotAddJson8(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult9(QJsonObject)),
                   this, SLOT(slotAddJson9(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult10(QJsonObject)),
                   this, SLOT(slotAddJson10(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult11(QJsonObject)),
                   this, SLOT(slotAddJson11(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult12(QJsonObject)),
                   this, SLOT(slotAddJson12(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult13(QJsonObject)),
                   this, SLOT(slotAddJson13(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult14(QJsonObject)),
                   this, SLOT(slotAddJson14(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult15(QJsonObject)),
                   this, SLOT(slotAddJson15(QJsonObject)));
        connect(canbusControl, SIGNAL(signalSendTestResult16(QJsonObject)),
                   this, SLOT(slotAddJson16(QJsonObject)));
    }
    //canbusControl->testJson();
    canbusControl->slotSetMod(m_mod);
    canbusControl->testConn();
    canbusThread->start();
    canbusThread->setPriority(QThread::HighPriority);
    emit signalChangeOpenDeviceStatus();
}

void MainControl::slotCreateExcel1(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem1(items);
    if(m_serial1 == "")
    {
        m_serial1 = "NULL";
    }
    QString path;
    if(testResult1)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type1)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial1 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial1 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial1 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial1 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial1 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial1 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial1 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial1 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial1 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial1 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial1 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial1 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial1 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel2(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem2(items);
    if(m_serial2 == "")
    {
        m_serial2 = "NULL";
    }
    QString path;
    if(testResult2)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type2)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial2 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial2 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial2 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial2 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial2 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial2 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial2 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial2 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial2 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial2 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial2 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial2 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial2 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel3(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem3(items);
    if(m_serial3 == "")
    {
        m_serial3 = "NULL";
    }
    QString path;
    if(testResult3)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type3)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial3 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial3 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial3 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial3 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial3 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial3 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial3 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial3 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial3 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial3 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial3 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial3 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial3 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel4(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem4(items);
    if(m_serial4 == "")
    {
        m_serial4 = "NULL";
    }
    QString path;
    if(testResult4)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type4)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial4 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial4 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial4 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial4 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial4 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial4 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial4 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial4 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial4 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial4 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial4 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial4 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial4 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel5(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem5(items);
    if(m_serial5 == "")
    {
        m_serial5 = "NULL";
    }
    QString path;
    if(testResult5)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type5)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial5 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial5 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial5 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial5 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial5 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial5 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial5 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial5 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial5 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial5 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial5 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial5 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial5 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel6(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem6(items);
    if(m_serial6 == "")
    {
        m_serial6 = "NULL";
    }
    QString path;
    if(testResult6)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type6)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial6 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial6 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial6 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial6 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial6 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial6 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial6 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial6 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial6 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial6 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial6 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial6 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial6 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel7(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem7(items);
    if(m_serial7 == "")
    {
        m_serial7 = "NULL";
    }
    QString path;
    if(testResult7)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type7)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial7 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial7 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial7 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial7 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial7 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial7 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial7 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial7 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial7 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial7 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial7 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial7 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial7 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel8(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem8(items);
    if(m_serial8 == "")
    {
        m_serial8 = "NULL";
    }
    QString path;
    if(testResult8)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type8)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial8 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial8 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial8 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial8 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial8 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial8 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial8 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial8 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial8 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial8 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial8 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial8 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial8 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel9(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem9(items);
    if(m_serial9 == "")
    {
        m_serial9 = "NULL";
    }
    QString path;
    if(testResult9)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type9)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial9 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial9 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial9 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial9 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial9 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial9 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial9 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial9 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial9 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial9 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial9 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial9 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial9 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel10(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem10(items);
    if(m_serial10 == "")
    {
        m_serial10 = "NULL";
    }
    QString path;
    if(testResult10)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type10)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial10 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial10 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial10 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial10 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial10 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial10 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial10 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial10 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial10 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial10 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial10 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial10 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial10 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel11(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem11(items);
    if(m_serial11 == "")
    {
        m_serial11 = "NULL";
    }
    QString path;
    if(testResult11)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type11)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial11 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial11 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial11 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial11 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial11 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial11 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial11 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial11 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial11 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial11 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial11 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial11 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial11 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel12(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem12(items);
    if(m_serial12 == "")
    {
        m_serial12 = "NULL";
    }
    QString path;
    if(testResult12)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type12)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial12 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial12 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial12 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial12 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial12 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial12 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial12 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial12 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial12 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial12 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial12 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial12 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial12 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel13(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem13(items);
    if(m_serial13 == "")
    {
        m_serial13 = "NULL";
    }
    QString path;
    if(testResult13)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type13)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial13 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial13 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial13 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial13 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial13 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial13 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial13 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial13 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial13 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial13 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial13 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial13 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial13 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel14(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem14(items);
    if(m_serial14 == "")
    {
        m_serial14 = "NULL";
    }
    QString path;
    if(testResult14)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type14)    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial14 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial14 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial14 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial14 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial14 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial14 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial14 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial14 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial14 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial14 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial14 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial14 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial14 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel15(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem15(items);
    if(m_serial15 == "")
    {
        m_serial15 = "NULL";
    }
    QString path;
    if(testResult15)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type15)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial15 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial15 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial15 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial15 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial15 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial15 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial15 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial15 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial15 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial15 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial15 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial15 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial15 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotCreateExcel16(std::vector<ExcelItem> items)
{
    emit signalSetExcelItem16(items);
    if(m_serial16 == "")
    {
        m_serial16 = "NULL";
    }
    QString path;
    if(testResult16)
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/zheng_chang";
    }
    else
    {
        path = QCoreApplication::applicationDirPath() + "/test_results/yi_chang";
    }
    switch(m_type16)
    {
    case 0:
    {
        path = path + "/F133-L-HV5.0.1/Test-" + m_serial16 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 1:
    {
        path = path + "/F133-L-S-HV5.0.1/Test-" + m_serial16 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 2:
    {
        path = path + "/F133-L-ZC-HV5.0.2/Test-" + m_serial16 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 3:
    {
        path = path + "/F133-L-HV4.0.0/Test-" + m_serial16 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 4:
    {
        path = path + "/F133-L-S-HV6.0.0/Test-" + m_serial16 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 5:
    {
        path = path + "/A133-HV5.0.0/Test-" + m_serial16 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 6:
    {
        path = path + "/F133-F-S-HV6.1.0/Test-" + m_serial16 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 7:
    {
        path = path + "/H133-L-S-HV6.2.0/Test-" + m_serial16 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 8:
    {
        path = path + "/F133-L-S-XY1-HV5.0.1/Test-" + m_serial16 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 9:
    {
        path = path + "/F133-L-S-HV7.0.0/Test-" + m_serial16 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 10:
    {
        path = path + "/H133-L-ZC1-HV6.2.0/Test-" + m_serial16 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 11:
    {
        path = path + "/H133-L-S-SD1-HV6.2.0/Test-" + m_serial16 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    case 12:
    {
        path = path + "/F133-L-S-ZH1-HV5.3.0/Test-" + m_serial16 + ".xlsx";
        qDebug() << "save as " << path;
        emit signalCreateExcel1(path);
        break;
    }
    default:
        break;
    }
}

void MainControl::slotChooseZLG()
{
    qDebug() << "choose zlg";
    if(chooseDeviceWidget)
    {
        delete chooseDeviceWidget;
        chooseDeviceWidget = NULL;
    }
    chooseDeviceWidget = new ChooseDeviceDialog(mainWindow, 2);
    connect(chooseDeviceWidget, SIGNAL(signalChooseZLGType(int)), canbusControl, SLOT(slotChooseZLGType(int)));
    connect(chooseDeviceWidget, SIGNAL(signalChooseZLGType(int)), mainWindow, SLOT(slotChangeConnectStatus()));
    chooseDeviceWidget->exec();
}

void MainControl::slotChooseGC()
{
    qDebug() << "choose gc";
    if(chooseDeviceWidget)
    {
        delete chooseDeviceWidget;
        chooseDeviceWidget = NULL;
    }
    chooseDeviceWidget = new ChooseDeviceDialog(mainWindow, 3);
    connect(chooseDeviceWidget, SIGNAL(signalChooseGCType(int)), canbusControl, SLOT(slotChooseGCType(int)));
    connect(chooseDeviceWidget, SIGNAL(signalChooseGCType(int)), mainWindow, SLOT(slotChangeConnectStatus()));
    chooseDeviceWidget->exec();
}

void MainControl::slotCloseCANPort()
{
    if(canbusThread)
    {
        canbusThread->exit();
        delete canbusControl;
        canbusControl = NULL;
    }
    emit signalChangeOpenDeviceStatus();
}

void MainControl::slotEnterTestState1()
{
    qDebug() << "entered test state1" << currentTest1;
    if(mainWindow)
    {
        mainWindow->updateView1();
    }
    emit signalUpdateTestItem1();
}

void MainControl::slotExitTestState1()
{
    qDebug() << "EXIT test state" << lastTest1;
}

void MainControl::slotEnterTestState2()
{
    qDebug() << "entered test state2" << currentTest2;
    if(mainWindow)
    {
        mainWindow->updateView2();
    }
    emit signalUpdateTestItem2();
}

void MainControl::slotExitTestState2()
{
    qDebug() << "EXIT test state" << lastTest2;
}

void MainControl::slotEnterTestState3()
{
    qDebug() << "entered test state1" << currentTest3;
    if(mainWindow)
    {
        mainWindow->updateView3();
    }
    emit signalUpdateTestItem3();
}

void MainControl::slotExitTestState3()
{
    qDebug() << "EXIT test state" << lastTest3;
}

void MainControl::slotEnterTestState4()
{
    qDebug() << "entered test state4" << currentTest4;
    if(mainWindow)
    {
        mainWindow->updateView4();
    }
    emit signalUpdateTestItem4();
}

void MainControl::slotExitTestState4()
{
    qDebug() << "EXIT test state" << lastTest4;
}

void MainControl::slotEnterTestState5()
{
    qDebug() << "entered test state5" << currentTest5;
    if(mainWindow)
    {
        mainWindow->updateView5();
    }
    emit signalUpdateTestItem5();
}

void MainControl::slotExitTestState5()
{
    qDebug() << "EXIT test state" << lastTest5;
}

void MainControl::slotEnterTestState6()
{
    qDebug() << "entered test state6" << currentTest6;
    if(mainWindow)
    {
        mainWindow->updateView6();
    }
    emit signalUpdateTestItem6();
}

void MainControl::slotExitTestState6()
{
    qDebug() << "EXIT test state" << lastTest6;
}

void MainControl::slotEnterTestState7()
{
    qDebug() << "entered test state7" << currentTest7;
    if(mainWindow)
    {
        mainWindow->updateView7();
    }
    emit signalUpdateTestItem7();
}

void MainControl::slotExitTestState7()
{
    qDebug() << "EXIT test state" << lastTest7;
}

void MainControl::slotEnterTestState8()
{
    qDebug() << "entered test state8" << currentTest8;
    if(mainWindow)
    {
        mainWindow->updateView8();
    }
    emit signalUpdateTestItem8();
}

void MainControl::slotExitTestState8()
{
    qDebug() << "EXIT test state" << lastTest8;
}

void MainControl::slotEnterTestState9()
{
    qDebug() << "entered test state9" << currentTest9;
    if(mainWindow)
    {
        mainWindow->updateView9();
    }
    emit signalUpdateTestItem9();
}

void MainControl::slotExitTestState9()
{
    qDebug() << "EXIT test state" << lastTest9;
}

void MainControl::slotEnterTestState10()
{
    qDebug() << "entered test state10" << currentTest10;
    if(mainWindow)
    {
        mainWindow->updateView10();
    }
    emit signalUpdateTestItem10();
}

void MainControl::slotExitTestState10()
{
    qDebug() << "EXIT test state" << lastTest10;
}

void MainControl::slotEnterTestState11()
{
    qDebug() << "entered test state11" << currentTest11;
    if(mainWindow)
    {
        mainWindow->updateView11();
    }
    emit signalUpdateTestItem11();
}

void MainControl::slotExitTestState11()
{
    qDebug() << "EXIT test state" << lastTest11;
}

void MainControl::slotEnterTestState12()
{
    qDebug() << "entered test state12" << currentTest12;
    if(mainWindow)
    {
        mainWindow->updateView12();
    }
    emit signalUpdateTestItem12();
}

void MainControl::slotExitTestState12()
{
    qDebug() << "EXIT test state" << lastTest12;
}

void MainControl::slotEnterTestState13()
{
    qDebug() << "entered test state13" << currentTest13;
    if(mainWindow)
    {
        mainWindow->updateView13();
    }
    emit signalUpdateTestItem13();
}

void MainControl::slotExitTestState13()
{
    qDebug() << "EXIT test state" << lastTest13;
}

void MainControl::slotEnterTestState14()
{
    qDebug() << "entered test state14" << currentTest14;
    if(mainWindow)
    {
        mainWindow->updateView14();
    }
    emit signalUpdateTestItem14();
}

void MainControl::slotExitTestState14()
{
    qDebug() << "EXIT test state" << lastTest14;
}

void MainControl::slotEnterTestState15()
{
    qDebug() << "entered test state15" << currentTest15;
    if(mainWindow)
    {
        mainWindow->updateView15();
    }
    emit signalUpdateTestItem15();
}

void MainControl::slotExitTestState15()
{
    qDebug() << "EXIT test state" << lastTest15;
}

void MainControl::slotEnterTestState16()
{
    qDebug() << "entered test state16" << currentTest16;
    if(mainWindow)
    {
        mainWindow->updateView16();
    }
    emit signalUpdateTestItem16();
}

void MainControl::slotExitTestState16()
{
    qDebug() << "EXIT test state" << lastTest16;
}

void MainControl::slotTryUpadateStates1(int status)
{
    lastTest1 = currentTest1;
    currentTest1 = static_cast<TestItemIdentify>(status);
    qDebug() << "last1 is " << lastTest1 << " current1 is " << currentTest1;
    if(!updateTestState1())
    {
    }
    else
    {
        emitSignals1();
    }
}

void MainControl::slotTryUpadateStates2(int status)
{
    lastTest2 = currentTest2;
    currentTest2 = static_cast<TestItemIdentify>(status);
    qDebug() << "last2 is " << lastTest2 << " current2 is " << currentTest2;
    if(!updateTestState2())
    {
    }
    else
    {
        emitSignals2();
    }
}

void MainControl::slotTryUpadateStates3(int status)
{
    lastTest3 = currentTest3;
    currentTest3 = static_cast<TestItemIdentify>(status);
    qDebug() << "last3 is " << lastTest3 << " current3 is " << currentTest3;
    if(!updateTestState3())
    {
    }
    else
    {
        emitSignals3();
    }
}

void MainControl::slotTryUpadateStates4(int status)
{
    lastTest4 = currentTest4;
    currentTest4 = static_cast<TestItemIdentify>(status);
    qDebug() << "last4 is " << lastTest4 << " current4 is " << currentTest4;
    if(!updateTestState4())
    {
    }
    else
    {
        emitSignals4();
    }
}

void MainControl::slotTryUpadateStates5(int status)
{
    lastTest5 = currentTest5;
    currentTest5 = static_cast<TestItemIdentify>(status);
    qDebug() << "last5 is " << lastTest5 << " current5 is " << currentTest5;
    if(!updateTestState5())
    {
    }
    else
    {
        emitSignals5();
    }
}

void MainControl::slotTryUpadateStates6(int status)
{
    lastTest6 = currentTest6;
    currentTest6 = static_cast<TestItemIdentify>(status);
    qDebug() << "last6 is " << lastTest6 << " current6 is " << currentTest6;
    if(!updateTestState6())
    {
    }
    else
    {
        emitSignals6();
    }
}

void MainControl::slotTryUpadateStates7(int status)
{
    lastTest7 = currentTest7;
    currentTest7 = static_cast<TestItemIdentify>(status);
    qDebug() << "last7 is " << lastTest7 << " current7 is " << currentTest7;
    if(!updateTestState7())
    {
    }
    else
    {
        emitSignals7();
    }
}

void MainControl::slotTryUpadateStates8(int status)
{
    lastTest8 = currentTest8;
    currentTest8 = static_cast<TestItemIdentify>(status);
    qDebug() << "last8 is " << lastTest8 << " current8 is " << currentTest8;
    if(!updateTestState8())
    {
    }
    else
    {
        emitSignals8();
    }
}

void MainControl::slotTryUpadateStates9(int status)
{
    lastTest9 = currentTest9;
    currentTest9 = static_cast<TestItemIdentify>(status);
    qDebug() << "last9 is " << lastTest9 << " current9 is " << currentTest9;
    if(!updateTestState9())
    {
    }
    else
    {
        emitSignals9();
    }
}

void MainControl::slotTryUpadateStates10(int status)
{
    lastTest10 = currentTest10;
    currentTest10 = static_cast<TestItemIdentify>(status);
    qDebug() << "last10 is " << lastTest10 << " current10 is " << currentTest10;
    if(!updateTestState10())
    {
    }
    else
    {
        emitSignals10();
    }
}

void MainControl::slotTryUpadateStates11(int status)
{
    lastTest11 = currentTest11;
    currentTest11 = static_cast<TestItemIdentify>(status);
    qDebug() << "last11 is " << lastTest11 << " current11 is " << currentTest11;
    if(!updateTestState11())
    {
    }
    else
    {
        emitSignals11();
    }
}

void MainControl::slotTryUpadateStates12(int status)
{
    lastTest12 = currentTest12;
    currentTest12 = static_cast<TestItemIdentify>(status);
    qDebug() << "last12 is " << lastTest12 << " current12 is " << currentTest12;
    if(!updateTestState12())
    {
    }
    else
    {
        emitSignals12();
    }
}

void MainControl::slotTryUpadateStates13(int status)
{
    lastTest13 = currentTest13;
    currentTest13 = static_cast<TestItemIdentify>(status);
    qDebug() << "last13 is " << lastTest13 << " current13 is " << currentTest13;
    if(!updateTestState13())
    {
    }
    else
    {
        emitSignals13();
    }
}

void MainControl::slotTryUpadateStates14(int status)
{
    lastTest14 = currentTest14;
    currentTest14 = static_cast<TestItemIdentify>(status);
    qDebug() << "last14 is " << lastTest14 << " current14 is " << currentTest14;
    if(!updateTestState14())
    {
    }
    else
    {
        emitSignals14();
    }
}

void MainControl::slotTryUpadateStates15(int status)
{
    lastTest15 = currentTest15;
    currentTest15 = static_cast<TestItemIdentify>(status);
    qDebug() << "last15 is " << lastTest15 << " current15 is " << currentTest15;
    if(!updateTestState15())
    {
    }
    else
    {
        emitSignals15();
    }
}

void MainControl::slotTryUpadateStates16(int status)
{
    lastTest16 = currentTest16;
    currentTest16 = static_cast<TestItemIdentify>(status);
    qDebug() << "last16 is " << lastTest16 << " current16 is " << currentTest16;
    if(!updateTestState16())
    {
    }
    else
    {
        emitSignals16();
    }
}

void MainControl::slotStartTest1()
{
    currentTest1 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test1";
    if(!updateTestState1())
    {
    }
    else
    {
        emitSignals1();
    }
}

void MainControl::slotStartTest2()
{
    currentTest2 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test2";
    if(!updateTestState2())
    {
    }
    else
    {
        emitSignals2();
    }
}

void MainControl::slotStartTest3()
{
    currentTest3 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test3";
    if(!updateTestState3())
    {
    }
    else
    {
        emitSignals3();
    }
}

void MainControl::slotStartTest4()
{
    currentTest4 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test4";
    if(!updateTestState4())
    {
    }
    else
    {
        emitSignals4();
    }
}

void MainControl::slotStartTest5()
{
    currentTest5 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test5";
    if(!updateTestState5())
    {
    }
    else
    {
        emitSignals5();
    }
}

void MainControl::slotStartTest6()
{
    currentTest6 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test6";
    if(!updateTestState6())
    {
    }
    else
    {
        emitSignals6();
    }
}

void MainControl::slotStartTest7()
{
    currentTest7 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test7";
    if(!updateTestState7())
    {
    }
    else
    {
        emitSignals7();
    }
}

void MainControl::slotStartTest8()
{
    currentTest8 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test8";
    if(!updateTestState8())
    {
    }
    else
    {
        emitSignals8();
    }
}

void MainControl::slotStartTest9()
{
    currentTest9 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test9";
    if(!updateTestState9())
    {
    }
    else
    {
        emitSignals9();
    }
}

void MainControl::slotStartTest10()
{
    currentTest10 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test10";
    if(!updateTestState10())
    {
    }
    else
    {
        emitSignals10();
    }
}

void MainControl::slotStartTest11()
{
    currentTest11 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test11";
    if(!updateTestState11())
    {
    }
    else
    {
        emitSignals11();
    }
}

void MainControl::slotStartTest12()
{
    currentTest12 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test12";
    if(!updateTestState12())
    {
    }
    else
    {
        emitSignals12();
    }
}

void MainControl::slotStartTest13()
{
    currentTest13 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test13";
    if(!updateTestState13())
    {
    }
    else
    {
        emitSignals13();
    }
}

void MainControl::slotStartTest14()
{
    currentTest14 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test14";
    if(!updateTestState14())
    {
    }
    else
    {
        emitSignals14();
    }
}

void MainControl::slotStartTest15()
{
    currentTest15 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test15";
    if(!updateTestState15())
    {
    }
    else
    {
        emitSignals15();
    }
}

void MainControl::slotStartTest16()
{
    currentTest16 = static_cast<TestItemIdentify>(TestSetBatteryVoltage);
    qDebug() << "start test16";
    if(!updateTestState16())
    {
    }
    else
    {
        emitSignals16();
    }
}

void MainControl::slotErrorFinish1()
{
    if(!ifContinue)
    {
        lastTest1 = currentTest1;
        currentTest1 = StateFinishTest;
        if(updateTestState1())
        {
            emitSignals1();
        }
    }
}

void MainControl::slotErrorFinish2()
{
    if(!ifContinue)
    {
        lastTest2 = currentTest2;
        currentTest2 = StateFinishTest;
        if(updateTestState2())
        {
            emitSignals2();
        }
    }
}

void MainControl::slotErrorFinish3()
{
    if(!ifContinue)
    {
        lastTest3 = currentTest3;
        currentTest3 = StateFinishTest;
        if(updateTestState3())
        {
            emitSignals3();
        }
    }
}

void MainControl::slotErrorFinish4()
{
    if(!ifContinue)
    {
        lastTest4 = currentTest4;
        currentTest4 = StateFinishTest;
        if(updateTestState4())
        {
            emitSignals4();
        }
    }
}

void MainControl::slotErrorFinish5()
{
    if(!ifContinue)
    {
        lastTest5 = currentTest5;
        currentTest5 = StateFinishTest;
        if(updateTestState5())
        {
            emitSignals5();
        }
    }
}

void MainControl::slotErrorFinish6()
{
    if(!ifContinue)
    {
        lastTest6 = currentTest6;
        currentTest6 = StateFinishTest;
        if(updateTestState6())
        {
            emitSignals6();
        }
    }
}

void MainControl::slotErrorFinish7()
{
    if(!ifContinue)
    {
        lastTest7 = currentTest7;
        currentTest7 = StateFinishTest;
        if(updateTestState7())
        {
            emitSignals7();
        }
    }
}

void MainControl::slotErrorFinish8()
{
    if(!ifContinue)
    {
        lastTest8 = currentTest8;
        currentTest8 = StateFinishTest;
        if(updateTestState8())
        {
            emitSignals8();
        }
    }
}

void MainControl::slotErrorFinish9()
{
    if(!ifContinue)
    {
        lastTest9 = currentTest9;
        currentTest9 = StateFinishTest;
        if(updateTestState9())
        {
            emitSignals9();
        }
    }
}

void MainControl::slotErrorFinish10()
{
    if(!ifContinue)
    {
        lastTest10 = currentTest10;
        currentTest10 = StateFinishTest;
        if(updateTestState10())
        {
            emitSignals10();
        }
    }
}

void MainControl::slotErrorFinish11()
{
    if(!ifContinue)
    {
        lastTest11 = currentTest11;
        currentTest11 = StateFinishTest;
        if(updateTestState11())
        {
            emitSignals11();
        }
    }
}

void MainControl::slotErrorFinish12()
{
    if(!ifContinue)
    {
        lastTest12 = currentTest12;
        currentTest12 = StateFinishTest;
        if(updateTestState12())
        {
            emitSignals12();
        }
    }
}

void MainControl::slotErrorFinish13()
{
    if(!ifContinue)
    {
        lastTest13 = currentTest13;
        currentTest13 = StateFinishTest;
        if(updateTestState13())
        {
            emitSignals13();
        }
    }
}

void MainControl::slotErrorFinish14()
{
    if(!ifContinue)
    {
        lastTest14 = currentTest14;
        currentTest14 = StateFinishTest;
        if(updateTestState14())
        {
            emitSignals14();
        }
    }
}

void MainControl::slotErrorFinish15()
{
    if(!ifContinue)
    {
        lastTest15 = currentTest15;
        currentTest15 = StateFinishTest;
        if(updateTestState15())
        {
            emitSignals15();
        }
    }
}

void MainControl::slotErrorFinish16()
{
    if(!ifContinue)
    {
        lastTest16 = currentTest16;
        currentTest16 = StateFinishTest;
        if(updateTestState16())
        {
            emitSignals16();
        }
    }
}

void MainControl::slotChangeContinue(int s)
{
    ifContinue = s;
}

void MainControl::slotStartMainWindow(bool mod, int type1, int type2, int type3, int type4, int type5
                                      , int type6, int type7, int type8, int type9, int type10
                                      , int type11, int type12, int type13, int type14, int type15, int type16)
{
    if(typeDialog)
    {
        delete typeDialog;
        typeDialog = NULL;
    }
    qDebug() << "start as " << (mod ? "mes" : "hand") << " at " << type1;
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
    m_mod = mod;
    mainWindow->setWindowType(mod, type1, type2, type3, type4, type5, type6, type7, type8, type9, type10,
                              type11, type12, type13, type14, type15, type16);
    if(mod == false)
    {
        excelControl1 = new ExcelControl(this,  type1);
        connect(mainWindow, SIGNAL(signalSaveData1(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel1(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel1(const QString &)),
                excelControl1, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem1(std::vector<ExcelItem>)),
                             excelControl1, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult1(bool)), this, SLOT(slotTestResult1(bool)));

        excelControl2 = new ExcelControl(this,  type2);
        connect(mainWindow, SIGNAL(signalSaveData2(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel2(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel2(const QString &)),
                excelControl2, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem2(std::vector<ExcelItem>)),
                             excelControl2, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult2(bool)), this, SLOT(slotTestResult2(bool)));

        excelControl3 = new ExcelControl(this,  type3);
        connect(mainWindow, SIGNAL(signalSaveData3(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel3(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel3(const QString &)),
                excelControl3, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem3(std::vector<ExcelItem>)),
                             excelControl3, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult3(bool)), this, SLOT(slotTestResult3(bool)));

        excelControl4 = new ExcelControl(this,  type4);
        connect(mainWindow, SIGNAL(signalSaveData4(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel4(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel4(const QString &)),
                excelControl4, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem4(std::vector<ExcelItem>)),
                             excelControl4, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult4(bool)), this, SLOT(slotTestResult4(bool)));

        excelControl5 = new ExcelControl(this,  type5);
        connect(mainWindow, SIGNAL(signalSaveData5(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel5(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel5(const QString &)),
                excelControl5, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem5(std::vector<ExcelItem>)),
                             excelControl5, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult5(bool)), this, SLOT(slotTestResult5(bool)));

        excelControl6 = new ExcelControl(this,  type6);
        connect(mainWindow, SIGNAL(signalSaveData6(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel6(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel6(const QString &)),
                excelControl6, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem6(std::vector<ExcelItem>)),
                             excelControl6, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult6(bool)), this, SLOT(slotTestResult6(bool)));

        excelControl7 = new ExcelControl(this,  type7);
        connect(mainWindow, SIGNAL(signalSaveData7(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel7(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel7(const QString &)),
                excelControl7, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem7(std::vector<ExcelItem>)),
                             excelControl7, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult7(bool)), this, SLOT(slotTestResult7(bool)));

        excelControl8 = new ExcelControl(this,  type8);
        connect(mainWindow, SIGNAL(signalSaveData8(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel8(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel8(const QString &)),
                excelControl8, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem8(std::vector<ExcelItem>)),
                             excelControl8, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult8(bool)), this, SLOT(slotTestResult8(bool)));

        excelControl9 = new ExcelControl(this,  type9);
        connect(mainWindow, SIGNAL(signalSaveData9(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel9(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel9(const QString &)),
                excelControl9, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem9(std::vector<ExcelItem>)),
                             excelControl9, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult9(bool)), this, SLOT(slotTestResult9(bool)));

        excelControl10 = new ExcelControl(this,  type10);
        connect(mainWindow, SIGNAL(signalSaveData10(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel10(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel10(const QString &)),
                excelControl10, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem10(std::vector<ExcelItem>)),
                             excelControl10, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult10(bool)), this, SLOT(slotTestResult10(bool)));

        excelControl11 = new ExcelControl(this,  type11);
        connect(mainWindow, SIGNAL(signalSaveData11(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel11(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel11(const QString &)),
                excelControl11, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem11(std::vector<ExcelItem>)),
                             excelControl11, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult11(bool)), this, SLOT(slotTestResult11(bool)));

        excelControl12 = new ExcelControl(this,  type12);
        connect(mainWindow, SIGNAL(signalSaveData12(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel12(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel12(const QString &)),
                excelControl12, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem12(std::vector<ExcelItem>)),
                             excelControl12, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult12(bool)), this, SLOT(slotTestResult12(bool)));

        excelControl13 = new ExcelControl(this,  type13);
        connect(mainWindow, SIGNAL(signalSaveData13(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel13(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel13(const QString &)),
                excelControl13, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem13(std::vector<ExcelItem>)),
                             excelControl13, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult13(bool)), this, SLOT(slotTestResult13(bool)));

        excelControl14 = new ExcelControl(this,  type14);
        connect(mainWindow, SIGNAL(signalSaveData14(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel14(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel14(const QString &)),
                excelControl14, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem14(std::vector<ExcelItem>)),
                             excelControl14, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult14(bool)), this, SLOT(slotTestResult14(bool)));

        excelControl15 = new ExcelControl(this,  type15);
        connect(mainWindow, SIGNAL(signalSaveData15(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel15(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel15(const QString &)),
                excelControl15, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem15(std::vector<ExcelItem>)),
                             excelControl15, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult15(bool)), this, SLOT(slotTestResult15(bool)));

        excelControl16 = new ExcelControl(this,  type16);
        connect(mainWindow, SIGNAL(signalSaveData16(std::vector<ExcelItem>)), this, SLOT(slotCreateExcel16(std::vector<ExcelItem>)));
        connect(this, SIGNAL(signalCreateExcel16(const QString &)),
                excelControl16, SLOT(slotStartExcel(const QString &)));
        connect(this, SIGNAL(signalSetExcelItem16(std::vector<ExcelItem>)),
                             excelControl16, SLOT(slotSetExcel(std::vector<ExcelItem>)));
        connect(mainWindow, SIGNAL(signalTestResult16(bool)), this, SLOT(slotTestResult16(bool)));
    }
    else
    {
        merge = new JsonMerge();
        api = new NetWorkAPI(this);
        qRegisterMetaType<std::vector<ErrorDetial>>("std::vector<ErrorDetial>");
        connect(api, SIGNAL(signalGetVersion(const QString &, const QString &)), mainWindow,
                SLOT(slotGetVersion(const QString &, const QString &)));
        connect(mainWindow, SIGNAL(signalGetVersion(QJsonObject)), api, SLOT(slotGetVersion(QJsonObject)));
        connect(merge, SIGNAL(singalSendData(RequestData)), api, SLOT(slotSendRequest(RequestData)));
        connect(mainWindow, SIGNAL(signalSendSerialList(const QString &, const QString &, const QString &, const QString &, const QString &)), api,
                SLOT(slotSendSerialList(const QString &, const QString &, const QString &, const QString &, const QString &)));
        connect(api, SIGNAL(signalGetSerialStatus(std::vector<ErrorDetial>)), this, SLOT(slotGetSerialStatus(std::vector<ErrorDetial>)));
        connect(api, SIGNAL(signalNetError()), mainWindow, SLOT(slotNetError()));
        connect(merge, SIGNAL(singalResult(bool)), mainWindow, SLOT(slotResult(bool)));
        connect(mainWindow, SIGNAL(signalSendTestResult1(QJsonObject)),
                   this, SLOT(slotSetJson1(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult2(QJsonObject)),
                   this, SLOT(slotSetJson2(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult3(QJsonObject)),
                   this, SLOT(slotSetJson3(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult4(QJsonObject)),
                   this, SLOT(slotSetJson4(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult5(QJsonObject)),
                   this, SLOT(slotSetJson5(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult6(QJsonObject)),
                   this, SLOT(slotSetJson6(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult7(QJsonObject)),
                   this, SLOT(slotSetJson7(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult8(QJsonObject)),
                   this, SLOT(slotSetJson8(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult9(QJsonObject)),
                   this, SLOT(slotSetJson9(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult10(QJsonObject)),
                   this, SLOT(slotSetJson10(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult11(QJsonObject)),
                   this, SLOT(slotSetJson11(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult12(QJsonObject)),
                   this, SLOT(slotSetJson12(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult13(QJsonObject)),
                   this, SLOT(slotSetJson13(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult14(QJsonObject)),
                   this, SLOT(slotSetJson14(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult15(QJsonObject)),
                   this, SLOT(slotSetJson15(QJsonObject)));
        connect(mainWindow, SIGNAL(signalSendTestResult16(QJsonObject)),
                   this, SLOT(slotSetJson16(QJsonObject)));
        connect(this, SIGNAL(signalSendJsonData(QJsonObject)), merge, SLOT(slotReceiveTestResult(QJsonObject)));
        connect(mainWindow, SIGNAL(signalGetGDList()), api, SLOT(slotGetGDList()));
        connect(api, SIGNAL(signalReceiveGDList(const QStringList &)), mainWindow, SLOT(slotReceiveGDList(const QStringList &)));
        connect(api, SIGNAL(signalSetDetial(QString)), mainWindow, SLOT(slotSetDetial(QString)));
        connect(mainWindow, SIGNAL(signalSetGD(QString)), api, SLOT(slotSetGD(QString)));
    }
    auto initState1 = findState1("StateStartTest");
    m_stateMachine1->setInitialState(initState1);
    m_stateMachine1->start();
    auto initState2 = findState2("StateStartTest");
    m_stateMachine2->setInitialState(initState2);
    m_stateMachine2->start();
    auto initState3 = findState3("StateStartTest");
    m_stateMachine3->setInitialState(initState3);
    m_stateMachine3->start();
    auto initState4 = findState4("StateStartTest");
    m_stateMachine4->setInitialState(initState4);
    m_stateMachine4->start();
    auto initState5 = findState5("StateStartTest");
    m_stateMachine5->setInitialState(initState5);
    m_stateMachine5->start();
    auto initState6 = findState6("StateStartTest");
    m_stateMachine6->setInitialState(initState6);
    m_stateMachine6->start();
    auto initState7 = findState7("StateStartTest");
    m_stateMachine7->setInitialState(initState7);
    m_stateMachine7->start();
    auto initState8 = findState8("StateStartTest");
    m_stateMachine8->setInitialState(initState8);
    m_stateMachine8->start();
    auto initState9 = findState9("StateStartTest");
    m_stateMachine9->setInitialState(initState9);
    m_stateMachine9->start();
    auto initState10 = findState10("StateStartTest");
    m_stateMachine10->setInitialState(initState10);
    m_stateMachine10->start();
    auto initState11 = findState11("StateStartTest");
    m_stateMachine11->setInitialState(initState11);
    m_stateMachine11->start();
    auto initState12 = findState12("StateStartTest");
    m_stateMachine12->setInitialState(initState12);
    m_stateMachine12->start();
    auto initState13 = findState13("StateStartTest");
    m_stateMachine13->setInitialState(initState13);
    m_stateMachine13->start();
    auto initState14 = findState14("StateStartTest");
    m_stateMachine14->setInitialState(initState14);
    m_stateMachine14->start();
    auto initState15 = findState15("StateStartTest");
    m_stateMachine15->setInitialState(initState15);
    m_stateMachine15->start();
    auto initState16 = findState16("StateStartTest");
    m_stateMachine16->setInitialState(initState16);
    m_stateMachine16->start();
    mainWindow->show();
}

void MainControl::slotQuit()
{
    QTimer *myTimer = new QTimer();
    myTimer->start(1000);
    connect(myTimer, &QTimer::timeout, this, [=](){
           qApp->quit();
    });
}

void MainControl::slotSetSerial(const QString &serial)
{
    m_serial1 = serial;
}

void MainControl::slotTestResult1(bool result)
{
    testResult1 = result;
}

void MainControl::slotTestResult2(bool result)
{
    testResult2 = result;
}

void MainControl::slotTestResult3(bool result)
{
    testResult3 = result;
}

void MainControl::slotTestResult4(bool result)
{
    testResult4 = result;
}

void MainControl::slotTestResult5(bool result)
{
    testResult5 = result;
}

void MainControl::slotTestResult6(bool result)
{
    testResult6 = result;
}

void MainControl::slotTestResult7(bool result)
{
    testResult7 = result;
}

void MainControl::slotTestResult8(bool result)
{
    testResult8 = result;
}

void MainControl::slotTestResult9(bool result)
{
    testResult9 = result;
}

void MainControl::slotTestResult10(bool result)
{
    testResult10 = result;
}

void MainControl::slotTestResult11(bool result)
{
    testResult11 = result;
}

void MainControl::slotTestResult12(bool result)
{
    testResult12 = result;
}

void MainControl::slotTestResult13(bool result)
{
    testResult13 = result;
}

void MainControl::slotTestResult14(bool result)
{
    testResult14 = result;
}

void MainControl::slotTestResult15(bool result)
{
    testResult15 = result;
}

void MainControl::slotTestResult16(bool result)
{
    testResult16 = result;
}

void MainControl::slotGetSerialStatus(std::vector<ErrorDetial> fails)
{
    if(fails.size() == 0)
    {
        emit signalStartTest();
    }
    else
    {
        emit signalErrorSerial(fails.at(0).detial);
    }
}

void MainControl::slotFinishSet1()
{
    currentTest1 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState1();
}

void MainControl::slotFinishSet2()
{
    currentTest2 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState2();
}


void MainControl::slotFinishSet3()
{
    currentTest3 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState3();
}


void MainControl::slotFinishSet4()
{
    currentTest4 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState4();
}


void MainControl::slotFinishSet5()
{
    currentTest5 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState5();
}


void MainControl::slotFinishSet6()
{
    currentTest6 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState6();
}


void MainControl::slotFinishSet7()
{
    currentTest7 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState7();
}


void MainControl::slotFinishSet8()
{
    currentTest8 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState8();
}


void MainControl::slotFinishSet9()
{
    currentTest9 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState9();
}


void MainControl::slotFinishSet10()
{
    currentTest10 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState10();
}


void MainControl::slotFinishSet11()
{
    currentTest11 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState11();
}


void MainControl::slotFinishSet12()
{
    currentTest12 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState12();
}


void MainControl::slotFinishSet13()
{
    currentTest13 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState13();
}


void MainControl::slotFinishSet14()
{
    currentTest14 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState14();
}


void MainControl::slotFinishSet15()
{
    currentTest15 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState15();
}


void MainControl::slotFinishSet16()
{
    currentTest16 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState16();
}

void MainControl::slotFinishSet21()
{
    currentTest1 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState1();
}

void MainControl::slotFinishSet22()
{
    currentTest2 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState2();
}


void MainControl::slotFinishSet23()
{
    currentTest3 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState3();
}


void MainControl::slotFinishSet24()
{
    currentTest4 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState4();
}


void MainControl::slotFinishSet25()
{
    currentTest5 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState5();
}


void MainControl::slotFinishSet26()
{
    currentTest6 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState6();
}


void MainControl::slotFinishSet27()
{
    currentTest7 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState7();
}


void MainControl::slotFinishSet28()
{
    currentTest8 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState8();
}


void MainControl::slotFinishSet29()
{
    currentTest9 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState9();
}


void MainControl::slotFinishSet210()
{
    currentTest10 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState10();
}


void MainControl::slotFinishSet211()
{
    currentTest11 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState11();
}


void MainControl::slotFinishSet212()
{
    currentTest12 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState12();
}


void MainControl::slotFinishSet213()
{
    currentTest13 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState13();
}


void MainControl::slotFinishSet214()
{
    currentTest14 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState14();
}


void MainControl::slotFinishSet215()
{
    currentTest15 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState15();
}


void MainControl::slotFinishSet216()
{
    currentTest16 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState16();
}

void MainControl::slotReceiveSerial1(QString serial)
{
    m_serial1 = serial;
}

void MainControl::slotReceiveSerial2(QString serial)
{
    m_serial2 = serial;
}

void MainControl::slotReceiveSerial3(QString serial)
{
    m_serial3 = serial;
}

void MainControl::slotReceiveSerial4(QString serial)
{
    m_serial4 = serial;
}

void MainControl::slotReceiveSerial5(QString serial)
{
    m_serial5 = serial;
}

void MainControl::slotReceiveSerial6(QString serial)
{
    m_serial6 = serial;
}

void MainControl::slotReceiveSerial7(QString serial)
{
    m_serial7 = serial;
}

void MainControl::slotReceiveSerial8(QString serial)
{
    m_serial8 = serial;
}

void MainControl::slotReceiveSerial9(QString serial)
{
    m_serial9 = serial;
}

void MainControl::slotReceiveSerial10(QString serial)
{
    m_serial10 = serial;
}

void MainControl::slotReceiveSerial11(QString serial)
{
    m_serial11 = serial;
}

void MainControl::slotReceiveSerial12(QString serial)
{
    m_serial12 = serial;
}

void MainControl::slotReceiveSerial13(QString serial)
{
    m_serial13 = serial;
}

void MainControl::slotReceiveSerial14(QString serial)
{
    m_serial14 = serial;
}

void MainControl::slotReceiveSerial15(QString serial)
{
    m_serial15 = serial;
}

void MainControl::slotReceiveSerial16(QString serial)
{
    m_serial16 = serial;
}

void MainControl::slotSetJson1(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result1.remove(result);
        result1.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson1()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson1(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result1.remove(result);
        result1.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson1()
{
    emit signalSendJsonData(result1);
}

void MainControl::slotSetJson2(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result2.remove(result);
        result2.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson2()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson2(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result2.remove(result);
        result2.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson2()
{
    emit signalSendJsonData(result2);
}

void MainControl::slotSetJson3(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result3.remove(result);
        result3.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson3()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson3(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result3.remove(result);
        result3.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson3()
{
    emit signalSendJsonData(result3);
}

void MainControl::slotSetJson4(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result4.remove(result);
        result4.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson4()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson4(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result4.remove(result);
        result4.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson4()
{
    emit signalSendJsonData(result4);
}

void MainControl::slotSetJson5(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result5.remove(result);
        result5.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson5()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson5(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result5.remove(result);
        result5.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson5()
{
    emit signalSendJsonData(result5);
}

void MainControl::slotSetJson6(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result6.remove(result);
        result6.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson6()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson6(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result6.remove(result);
        result6.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson6()
{
    emit signalSendJsonData(result6);
}

void MainControl::slotSetJson7(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result7.remove(result);
        result7.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson7()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson7(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result7.remove(result);
        result7.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson7()
{
    emit signalSendJsonData(result7);
}

void MainControl::slotSetJson8(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result8.remove(result);
        result8.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson8()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson8(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result8.remove(result);
        result8.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson8()
{
    emit signalSendJsonData(result8);
}

void MainControl::slotSetJson9(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result9.remove(result);
        result9.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson9()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson9(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result9.remove(result);
        result9.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson9()
{
    emit signalSendJsonData(result9);
}

void MainControl::slotSetJson10(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result10.remove(result);
        result10.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson10()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson10(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result10.remove(result);
        result10.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson10()
{
    emit signalSendJsonData(result10);
}

void MainControl::slotSetJson11(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result11.remove(result);
        result11.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson11()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson11(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result11.remove(result);
        result11.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson11()
{
    emit signalSendJsonData(result11);
}

void MainControl::slotSetJson12(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result12.remove(result);
        result12.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson12()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson12(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result12.remove(result);
        result12.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson12()
{
    emit signalSendJsonData(result12);
}

void MainControl::slotSetJson13(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result13.remove(result);
        result13.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson13()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson13(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result13.remove(result);
        result13.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson13()
{
    emit signalSendJsonData(result13);
}

void MainControl::slotSetJson14(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result14.remove(result);
        result14.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson14()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson14(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result14.remove(result);
        result14.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson14()
{
    emit signalSendJsonData(result14);
}

void MainControl::slotSetJson15(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result15.remove(result);
        result15.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson15()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson15(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result15.remove(result);
        result15.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson15()
{
    emit signalSendJsonData(result15);
}

void MainControl::slotSetJson16(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result16.remove(result);
        result16.insert(result, data.value(result));
    }
    QTimer *tempTimer = new QTimer(this);
    connect(tempTimer, SIGNAL(timeout()), this, SLOT(slotSendJson16()));
    tempTimer->setSingleShot(true);
    tempTimer->start(5000);
}

void MainControl::slotAddJson16(QJsonObject data)
{
    QStringList list = data.keys();
    for(auto result : list)
    {
        result16.remove(result);
        result16.insert(result, data.value(result));
    }
}

void MainControl::slotSendJson16()
{
    emit signalSendJsonData(result16);
}

void MainControl::slotSingleFail1()
{
    if_test1 = false;
    currentTest1 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState1();
}

void MainControl::slotSingleFail2()
{
    if_test2 = false;
    currentTest2 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState2();
}


void MainControl::slotSingleFail3()
{
    if_test3 = false;
    currentTest3 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState3();
}


void MainControl::slotSingleFail4()
{
    if_test4 = false;
    currentTest4 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState4();
}


void MainControl::slotSingleFail5()
{
    if_test5 = false;
    currentTest5 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState5();
}


void MainControl::slotSingleFail6()
{
    if_test6 = false;
    currentTest6 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState6();
}


void MainControl::slotSingleFail7()
{
    if_test7 = false;
    currentTest7 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState7();
}


void MainControl::slotSingleFail8()
{
    if_test8 = false;
    currentTest8 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState8();
}


void MainControl::slotSingleFail9()
{
    if_test9 = false;
    currentTest9 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState9();
}


void MainControl::slotSingleFail10()
{
    if_test10 = false;
    currentTest10 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState10();
}


void MainControl::slotSingleFail11()
{
    if_test11 = false;
    currentTest11 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState11();
}


void MainControl::slotSingleFail12()
{
    if_test12 = false;
    currentTest12 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState12();
}


void MainControl::slotSingleFail13()
{
    if_test13 = false;
    currentTest13 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState13();
}


void MainControl::slotSingleFail14()
{
    if_test14 = false;
    currentTest14 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState14();
}


void MainControl::slotSingleFail15()
{
    if_test15 = false;
    currentTest15 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState15();
}


void MainControl::slotSingleFail16()
{
    if_test16 = false;
    currentTest16 = static_cast<TestItemIdentify>(StateTestSetBatteryVoltage);
    updateTestState16();
}



void MainControl::slotSingleFail21()
{
    if_test1 = false;
    currentTest1 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState1();
}

void MainControl::slotSingleFail22()
{
    if_test2 = false;
    currentTest2 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState2();
}


void MainControl::slotSingleFail23()
{
    if_test3 = false;
    currentTest3 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState3();
}


void MainControl::slotSingleFail24()
{
    if_test4 = false;
    currentTest4 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState4();
}


void MainControl::slotSingleFail25()
{
    if_test5 = false;
    currentTest5 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState5();
}


void MainControl::slotSingleFail26()
{
    if_test6 = false;
    currentTest6 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState6();
}


void MainControl::slotSingleFail27()
{
    if_test7 = false;
    currentTest7 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState7();
}


void MainControl::slotSingleFail28()
{
    if_test8 = false;
    currentTest8 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState8();
}


void MainControl::slotSingleFail29()
{
    if_test9 = false;
    currentTest9 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState9();
}


void MainControl::slotSingleFail210()
{
    if_test10 = false;
    currentTest10 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState10();
}


void MainControl::slotSingleFail211()
{
    if_test11 = false;
    currentTest11 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState11();
}


void MainControl::slotSingleFail212()
{
    if_test12 = false;
    currentTest12 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState12();
}


void MainControl::slotSingleFail213()
{
    if_test13 = false;
    currentTest13 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState13();
}


void MainControl::slotSingleFail214()
{
    if_test14 = false;
    currentTest14 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState14();
}


void MainControl::slotSingleFail215()
{
    if_test15 = false;
    currentTest15 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState15();
}


void MainControl::slotSingleFail216()
{
    if_test16 = false;
    currentTest16 = static_cast<TestItemIdentify>(StateTestBatteryVoltage);
    updateTestState16();
}

void MainControl::slotTimeout1()
{
    if_test1 = false;
}

void MainControl::slotTimeout2()
{
    if_test2 = false;
}

void MainControl::slotTimeout3()
{
    if_test3 = false;
}

void MainControl::slotTimeout4()
{
    if_test4 = false;
}

void MainControl::slotTimeout5()
{
    if_test5 = false;
}

void MainControl::slotTimeout6()
{
    if_test6 = false;
}

void MainControl::slotTimeout7()
{
    if_test7 = false;
}

void MainControl::slotTimeout8()
{
    if_test8 = false;
}

void MainControl::slotTimeout9()
{
    if_test9 = false;
}

void MainControl::slotTimeout10()
{
    if_test10 = false;
}

void MainControl::slotTimeout11()
{
    if_test11 = false;
}

void MainControl::slotTimeout12()
{
    if_test12 = false;
}

void MainControl::slotTimeout13()
{
    if_test13 = false;
}

void MainControl::slotTimeout14()
{
    if_test14 = false;
}

void MainControl::slotTimeout15()
{
    if_test15 = false;
}

void MainControl::slotTimeout16()
{
    if_test16 = false;
}
