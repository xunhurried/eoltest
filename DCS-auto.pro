QT       += core gui network serialbus serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++17
VERSION = 1.0.0.4
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
QMAKE_TARGET_COMPANY ="Gold"
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    controller/canbuscontrol.cpp \
    controller/excelcontrol.cpp \
    controller/maincontrol.cpp \
    controller/serialportcontroller.cpp \
    main.cpp \
    model/SerialPort.cpp \
    model/canbusfxynew.cpp \
    model/canbusfxyold.cpp \
    model/canbusgc.cpp \
    model/canbuszlg.cpp \
    model/cangetandsend.cpp \
    model/hallcurrentvector.cpp \
    model/jsonmerge.cpp \
    model/networkapi.cpp \
    model/ping.cpp \
    model/rs485parsethread.cpp \
    model/timer.cpp \
    view/adminlogdialog.cpp \
    view/checkkbdialog.cpp \
    view/choosedevicedialog.cpp \
    view/choosetypedialog.cpp \
    view/communicatesettingdialog.cpp \
    view/macdialog.cpp \
    view/mainwindow.cpp \
    view/serialdialog.cpp

HEADERS += \
    controller/canbuscontrol.h \
    controller/excelcontrol.h \
    controller/maincontrol.h \
    controller/serialportcontroller.h \
    model/Rs485RingBuf.h \
    model/SerialPort.hpp \
    model/canbusfxynew.h \
    model/canbusfxyold.h \
    model/canbusgc.h \
    model/canbuszlg.h \
    model/cangetandsend.h \
    model/canringbuffer.h \
    model/hallcurrentvector.h \
    model/jsonmerge.h \
    model/networkapi.h \
    model/ping.h \
    model/rs485parsethread.h \
    model/testitemenums.h \
    model/timer.h \
    view/adminlogdialog.h \
    view/checkkbdialog.h \
    view/choosedevicedialog.h \
    view/choosetypedialog.h \
    view/communicatesettingdialog.h \
    view/macdialog.h \
    view/mainwindow.h \
    view/serialdialog.h

FORMS += \
    view/adminlogdialog.ui \
    view/checkkbdialog.ui \
    view/choosedevicedialog.ui \
    view/choosetypedialog.ui \
    view/communicatesettingdialog.ui \
    view/macdialog.ui \
    view/mainwindow.ui \
    view/serialdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -L$$PWD/lib/ -llLogger
LIBS += -L$$PWD/lib/ -lControlCAN
LIBS += -L$$PWD/lib/ -lECanVci64
LIBS += -L$$PWD/lib/ -lControlCANnew
LIBS += -L$$PWD/lib/ -lControlCANold

LIBS += -L'C:/PROGRA~1/FireDa~1/lib/' -llibssl
LIBS += -L'C:/PROGRA~1/FireDa~1/lib/' -llibcrypto

INCLUDEPATH += -L$$PWD/lib/include/curl
INCLUDEPATH += 'C:/PROGRA~1/FireDa~1/include'
DEPENDPATH += 'C:/PROGRA~1/FireDa~1/include'
DEPENDPATH += -L$$PWD/lib/include/curl

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -llibcurl_imp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -llibcurl_imp
else:unix: LIBS += -L$$PWD/lib/ -llibcurl_imp
INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

RESOURCES += \
    icons.qrc

unix|win32: LIBS += -L$$PWD/QXlsx/bin64/ -lQXlsx

INCLUDEPATH += $$PWD/QXlsx/header
DEPENDPATH += $$PWD/QXlsx/header

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/QXlsx/bin64/QXlsx.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/QXlsx/bin64/libQXlsx.a
RC_ICONS = $$PWD/image/logo.ico
