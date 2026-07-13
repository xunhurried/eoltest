#ifndef LCREATEEXCEL_H
#define LCREATEEXCEL_H

#include <QObject>
#include <QString>
#include <vector>
enum RETURNCODE
{
    ERRORPATH     = 1,
    SAVESUCCESS   = 2,
};

struct ExcelItem
{
    int row;  //行
    int col;  //列
    QString word; //内容
};

class LCreateExcel : public QObject
{
    Q_OBJECT
public:
    LCreateExcel();
    ~LCreateExcel();

public slots:
    void slotCreateExcel(const QString &path, std::vector<ExcelItem> data);

signals:
    void signalExcelReturnCode(RETURNCODE);
};

#endif // LCREATEEXCEL_H
