#ifndef LDATABASE_H
#define LDATABASE_H

#include <QObject>
#include <QString>
#include <QSqlDatabase>

enum DataBaseStatus
{
    CONNECTSUCCESS    = 1,
    CONNECTIONERROR   = 2,
    STATEMENTERROR    = 3,
    TRANSACTIONERROR  = 4,
    UNKNOWNERROR      = 5,
    EXCUTESUCCESS     = 6
};

class QSqlQuery;
class LDataBase : public QObject
{
    Q_OBJECT
public:
    LDataBase(QObject *parent = nullptr);
    ~LDataBase();

public slots:
    void slotConnectDatabase(const QString &type, const QString &name);
    void slotExcuteSql(const QString &sql);

signals:
    void signalReturnDatabaseStatus(DataBaseStatus);
    void signalReturnExcuteResult(const QString &);

private:
    QSqlDatabase database;
    QSqlQuery *sqlQuery;
};

#endif // LDATABASE_H
