#include "networkapi.h"
#include <algorithm>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QCryptographicHash>
#include <QString>
#include <QByteArray>
#include <QLocale>
#include <QDateTime>
#include <QJsonArray>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <QFile>
#include <QCoreApplication>
#include <stdlib.h>
#include <QSettings>
#include <stdio.h>
#include "include/curl/curl.h"
#include "include/json.hpp"
#include <sys/stat.h>
#include <QTimer>

#include <openssl/md5.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

bool LessSort (ErrorDetial a,ErrorDetial b)
{
    return (a.number < b.number);
}

QString EnCodeURL(const QString& u)
{
    QByteArray encode = u.toLocal8Bit();
    encode = encode.toPercentEncoding(":/?=");
    return QString(encode);
}

// 获取所有网络接口
QList<QNetworkInterface> NetWorkAPI::getAllInterfaces()
{
    return QNetworkInterface::allInterfaces();
}

// 获取特定网络接口的IP地址
QHostAddress NetWorkAPI::getIPAddress(QNetworkInterface inter)
{
    foreach (QNetworkAddressEntry entry, inter.addressEntries())
    {
        if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
        {
            return entry.ip();
        }
    }
    return QHostAddress();
}

NetWorkAPI::NetWorkAPI(QObject *parent)
    : QObject{parent}
{
    connect(this, SIGNAL(signalAddFail(int, const QString &)), this, SLOT(slotAddFail(int, const QString &)));
}

QByteArray NetWorkAPI::HexHmacSha1(const char * input)
{
    unsigned char md[20];
    char mdBufs[20*2+1] = {0};
    unsigned int len = 0;
    char * key = "YCHKVPAGAY";
    HMAC(EVP_sha1(), key, strlen(key), (unsigned char*)input, strlen(input), md, &len);

    for (unsigned int i = 0; i < len; ++i)
    {
        sprintf(mdBufs + i*2, "%02x", md[i]);
    }
    //qDebug() << "in: " <<  input << " out: "  << mdBufs;

    return QString::fromStdString(mdBufs).toLatin1();
}

QString NetWorkAPI::getUTCTime()
{
    QDateTime local(QDateTime::currentDateTime());
    QDateTime UTC(local.toUTC());
    QLocale locale = QLocale(QLocale::English, QLocale::UnitedStates);
    QString timeString = locale.toString(UTC, "ddd, dd MMM yyyy hh:mm:ss ") + "GMT";
    return timeString;
}

void NetWorkAPI::slotSendRequest(RequestData requestData)
{
    QNetworkRequest request;
    request.setUrl(QUrl(("http://10.100.101.15:8888/ims-integrate/api/updateImsData")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject jsonData = QJsonObject();
    jsonData.insert("org_code", "200");
    jsonData.insert("mo_code", m_gd);
    jsonData.insert("lb_code", requestData.json.value("LotSN"));
    if(requestData.json.value("TestType") == "first")
    {
        jsonData.insert("wp_code", "GX018");
    }
    else
    {
        jsonData.insert("wp_code", "GX019");
    }
    jsonData.insert("wp_rs", requestData.json.value("TestResult") == "success" ? "1" : "0");
   // qDebug() << "json is " << requestData.json;
    jsonData.insert("data_info", requestData.json);
    QJsonObject bigJson = QJsonObject();
    bigJson.insert("docType", "SFI_WP_SUBMIT_LB");
    bigJson.insert("updateType", "UPDATE");
    QJsonArray array = QJsonArray();
    array.insert(0, jsonData);
    bigJson.insert("data", array);
    QJsonDocument doc(bigJson);
    //request.setRawHeader("data", doc.toJson(QJsonDocument::Compact));
   // qDebug() << "reply is " << doc.toJson(QJsonDocument::Compact);
    QNetworkReply *reply = manager.post(request, doc.toJson(QJsonDocument::Compact)); connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
        [=](QNetworkReply::NetworkError code){qDebug() << code;});
    connect(reply, &QNetworkReply::finished, [this, reply, requestData]() mutable
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << "Error: " << reply->errorString();
            emit signalNetError();
        }
        else
        {
            QByteArray replyArray =  reply->readAll();
            //qDebug() << "Response: " << replyArray;
            QString replyString = QString::fromStdString(replyArray.toStdString());
            if(replyString.contains("_resultcode_"))
            {
                //emit signalSendError(replyString);
            }
        }
        reply->deleteLater();
    });
}


void NetWorkAPI::slotSendSerialList(const QString &serial,  const QString &gd,
                                    const QString &op, const QString &mod, const QString &mac)
{
    QString temp = serial;
    QNetworkRequest request;
    m_gd = gd;
    request.setUrl(QUrl(("http://10.100.101.15:8888/ims-integrate/api/updateImsData")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject jsonData = QJsonObject();
    jsonData.insert("", "200");
    jsonData.insert("lb_code", temp);
    jsonData.insert("mo_code", gd);
    //jsonData.insert("wp_code", "ZDH005");
    if(mod == "first")
    {
        jsonData.insert("wp_code", "GX018");
    }
    else
    {
        jsonData.insert("wp_code", "GX019");
    }
    QJsonObject bigJson = QJsonObject();
    bigJson.insert("docType", "SFI_WP_CHECK_LB");
    bigJson.insert("updateType", "UPDATE");
    QJsonArray array = QJsonArray();
    array.insert(0, jsonData);
    bigJson.insert("data", array);
    QJsonDocument doc(bigJson);
    //request.setRawHeader("data", doc.toJson(QJsonDocument::Compact));
    qDebug() << "reply is " << doc.toJson(QJsonDocument::Compact);
    QNetworkReply *reply = manager.post(request, doc.toJson(QJsonDocument::Compact)); connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
        [=](QNetworkReply::NetworkError code){qDebug() << code;});
    connect(reply, &QNetworkReply::finished, [this, reply]() mutable
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << "Error: " << reply->errorString();
            emit signalNetError();
        }
        else
        {
            QByteArray replyArray =  reply->readAll();
            QJsonParseError pe;
            QJsonDocument doc = QJsonDocument::fromJson(replyArray, &pe);
            QString replyString = QString::fromStdString(replyArray.toStdString());
            if(!doc.object().value("resultCode").toString().contains("0000"))
            {
                emit signalAddFail(0, doc.object().value("resultMsg").toString());
            }
        }
        reply->deleteLater();
    });
    QTimer *sendTimer = new QTimer(this);
    sendTimer->setSingleShot(true);
    connect(sendTimer, SIGNAL(timeout()), this, SLOT(slotSendResult()));
    sendTimer->start(1000);
}

void NetWorkAPI::slotGetGDList()
{
    QNetworkRequest request;
    request.setUrl(QUrl("http://10.100.101.15:8888/ims-integrate/api/updateImsData"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject bigJson = QJsonObject();
    bigJson.insert("docType", "SFI_STAT_MO_LIST");
    bigJson.insert("updateType", "UPDATE");
    QJsonObject jsonData = QJsonObject();
    jsonData.insert("org_code", "200");
    //jsonData.insert("pl_code", "goldsc003");
    //jsonData.insert("pl_code", "ZDH-1");
    QJsonArray array = QJsonArray();
    array.insert(0, jsonData);
    bigJson.insert("data", array);
    QJsonDocument doc(bigJson);
    QStringList gdList;
    //request.setRawHeader("data", doc.toJson(QJsonDocument::Compact));
    qDebug() << "reply is " << doc.toJson(QJsonDocument::Compact);
    QNetworkReply *reply = manager.post(request, doc.toJson(QJsonDocument::Compact));
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            [=](QNetworkReply::NetworkError code){qDebug() << code;});
    connect(reply, &QNetworkReply::finished, [this, reply, gdList]() mutable
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << "Error: " << reply->errorString();
        }
        else
        {
            QByteArray replyArray =  reply->readAll();
            qDebug() << "Response: " << replyArray;
            std::string sJson = replyArray.toStdString();
            if (!nlohmann::json::accept(sJson))
            {
                qDebug() << "parse error";
            }
            else
            {
                auto json = nlohmann::json::parse(sJson);
                nlohmann::json monos = json.at("resultData").at(0).at("data");
                for(int i = 0; i < monos.size(); i++)
                {
                    std::string tempstr = monos[i].at("mo_code");
                    gdList.append(QString::fromStdString(tempstr));
                }
                qDebug() << "gdList is " << gdList;
                emit signalReceiveGDList(gdList);
            }
        }
        reply->deleteLater();
    });
}

void NetWorkAPI::slotSendResult()
{
    sort(failSerials.begin(),failSerials.end(), LessSort);
    emit signalGetSerialStatus(failSerials);
    failSerials.clear();
}

void NetWorkAPI::slotAddFail(int num, const QString &detial)
{
    ErrorDetial temp;
    temp.number = num;
    temp.detial = detial;
    failSerials.push_back(temp);
}

void NetWorkAPI::slotGetVersion(QJsonObject requestData)
{
    QNetworkRequest request;
    QString locate;
    QString ftpLocate;
    request.setUrl(QUrl(("http://10.100.101.15:8888/ims-integrate/api/updateImsData")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject bigJson = QJsonObject();
    bigJson.insert("docType", "GET_BURN_ATT");
    bigJson.insert("updateType", "UPDATE");
    QJsonArray array = QJsonArray();
    array.insert(0, requestData);
    bigJson.insert("data", array);
    QJsonDocument doc(bigJson);
    //request.setRawHeader("data", doc.toJson(QJsonDocument::Compact));
    qDebug() << "reply is " << doc.toJson(QJsonDocument::Compact);
    QNetworkReply *reply = manager.post(request, doc.toJson(QJsonDocument::Compact));
    connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
        [=](QNetworkReply::NetworkError code){qDebug() << code;});
    connect(reply, &QNetworkReply::finished, [this, reply, requestData, locate, ftpLocate]() mutable
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << "Error: " << reply->errorString();
        }
        else
        {
            QByteArray replyArray =  reply->readAll();
            qDebug() << "Response: " << replyArray;
            QString replyString = QString::fromStdString(replyArray.toStdString());
            if(!replyString.contains("0000"))
            {

            }
            else
            {
                std::string sJson = replyArray.toStdString();
                if (!nlohmann::json::accept(sJson))
                {
                    qDebug() << "parse error";
                }
                else
                {
                    auto json = nlohmann::json::parse(sJson);
                    nlohmann::json mono = json.at("resultData").at(0).at("data");
                    QString name;
                    try
                    {
                        name = QString::fromStdString(mono[0].at("prod_desc"));
                    }
                    catch(const std::out_of_range& e)
                    {
                        qDebug() << "Element not found: " << e.what();
                    }
                    QStringList namelist = name.split("/");
                    name = namelist.at(0);
                    qDebug() << "name list is " << namelist << " and name is " << name;
                    QString version;
                    QString soft;
                    QString detial;
                    try
                    {
                        version = QString::fromStdString(mono[0].at("gold_hardware_ver"));
                    }
                    catch(const std::out_of_range& e)
                    {
                        qDebug() << "Element not found: " << e.what();
                    }
                    try
                    {
                        soft = QString::fromStdString(mono[0].at("burn_ver"));
                    }
                    catch(const std::out_of_range& e)
                    {
                        qDebug() << "Element not found: " << e.what();
                    }
                    try
                    {
                        detial = QString::fromStdString(mono[0].at("remark"));
                    }
                    catch(const std::out_of_range& e)
                    {
                        qDebug() << "Element not found: " << e.what();
                    }
                    emit signalGetVersion(version, soft);
                    emit signalSetDetial(detial);
                }
            }
        }
        reply->deleteLater();
    });
}

void NetWorkAPI::slotSetGD(QString gd)
{
    m_gd = gd;
}

static size_t writefunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
    return fwrite(ptr, size, nmemb, (FILE*)stream);
}
