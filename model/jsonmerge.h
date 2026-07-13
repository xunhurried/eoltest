#ifndef JSONMERGE_H
#define JSONMERGE_H

#include <QObject>
#include <QJsonObject>
#include <vector>
#include "networkapi.h"

class JsonMerge : public QObject
{
    Q_OBJECT
public:
    JsonMerge();
    void initJson();

private:
    QJsonObject jsonData;
    QString errorString;

public slots:
    void slotReceiveResult(QJsonObject results);
    void slotReceiveTestResult(QJsonObject results);

signals:
    void singalSendData(RequestData data);
    void singalResult(bool);
};

#endif // JSONMERGE_H
