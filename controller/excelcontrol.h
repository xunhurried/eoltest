#ifndef EXCELCONTROL_H
#define EXCELCONTROL_H

#include <QObject>
#include <QString>
#include "xlsxdocument.h"

struct ExcelItem
{
    int row;  //行
    int col;  //列
    QString word; //内容
};

class ExcelControl : public QObject
{
    Q_OBJECT
public:
    ExcelControl(QObject *parent, int type);
    ~ExcelControl();

private:
    void initExcel();
    void saveExcel();

private:
    QXlsx::Document *excelConnector;
    QString savePath;
    bool ifSuccess = true;
    int m_type;

public slots:
    void slotStartExcel(const QString &path);
    void slotSetExcel(std::vector<ExcelItem> items);
    void slotSave();
};

#endif // EXCELCONTROL_H
