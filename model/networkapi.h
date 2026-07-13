#ifndef NETWORKAPI_H
#define NETWORKAPI_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QJsonObject>
#include <QQueue>
#include <vector>

struct RequestData
{
    QJsonObject json;
    int retryCount = 0;
};

struct ErrorDetial
{
    int number = 0;
    QString detial;
};

class NetWorkAPI : public QObject
{
    Q_OBJECT
public:
    explicit NetWorkAPI(QObject *parent = nullptr);
    QString getUTCTime();
    QByteArray HexHmacSha1(const char * input);

private:
    QList<QNetworkInterface> getAllInterfaces();
    QHostAddress getIPAddress(QNetworkInterface inter);

private:
    QNetworkAccessManager manager;
    std::vector<ErrorDetial> failSerials;
    QString m_gd;

public slots:
    void slotSendRequest(RequestData requestData);
    void slotSendSerialList(const QString &serial,  const QString &gd,
                            const QString &op, const QString &mod, const QString &mac);
    void slotGetGDList();
    void slotSendResult();
    void slotAddFail(int num, const QString &detial);
    void slotGetVersion(QJsonObject requestData);
    void slotSetGD(QString gd);

signals:
    void signalReceiveGDList(const QStringList &);
    void signalReceiveGDList2(const QStringList &);
    void signalGetSerialStatus(std::vector<ErrorDetial>);
    void signalAddFail(int, const QString &);
    void signalName(QString);
    void signalAddSerial(int, QString);
    void signalNetError();
    void signalGetVersion(const QString &, const QString &);
    void signalSetDetial(QString);
};

#endif // NETWORKAPI_H
