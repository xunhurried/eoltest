#ifndef LLOGGER_H
#define LLOGGER_H

#include <QString>

class LLogger
{
public:
    static void messageHandler(QtMsgType type, const QMessageLogContext &, const QString &message_);
};

#endif // LLOGGER_H
