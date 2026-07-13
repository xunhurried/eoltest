#include "controller/maincontrol.h"
#include "include/llogger.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMessageHandler(LLogger::messageHandler);
    MainControl control;
    control.initializeStateMachine();
    return a.exec();
}
