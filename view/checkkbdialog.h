#ifndef CHECKKBDIALOG_H
#define CHECKKBDIALOG_H

#include <QDialog>
#include "xlsxdocument.h"

namespace Ui {
class CheckKBDialog;
}

class CheckKBDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CheckKBDialog(QWidget *parent, int type, QString gz, QString op);
    ~CheckKBDialog();
    void initExcel();

private:
    Ui::CheckKBDialog *ui;
    float m_k = 0.0;
    float m_b = 0.0;
    QXlsx::Document *excelConnector;
    int m_type;
    QString m_gz;
    QString m_op;
    int m_currentRead = 1;
    QTimer *m_delayTimer = nullptr;

public slots:
    void slotTypeChange();
    void slotCalKB();
    void slotCalKB2();
    void slotCalKB3();
    void slotCalKB4();
    void slotCalKB5();
    void slotCalKB6();
    void slotCalKB7();
    void slotCalKB8();
    void slotCalKB9();
    void slotCalKB10();
    void slotCalKB11();
    void slotCalKB12();
    void slotCalKB13();
    void slotCalKB14();
    void slotCalKB15();
    void slotCalKB16();
    void slotSetKB();
    void slotSetKB2();
    void slotSetKB3();
    void slotSetKB4();
    void slotSetKB5();
    void slotSetKB6();
    void slotSetKB7();
    void slotSetKB8();
    void slotSetKB9();
    void slotSetKB10();
    void slotSetKB11();
    void slotSetKB12();
    void slotSetKB13();
    void slotSetKB14();
    void slotSetKB15();
    void slotSetKB16();
    void slotGetKB(float k, float b);
    void slotReadInfo1();
    void slotReadInfo2();
    void slotGetInfo(int type, float a, int locate);
    void slotSaveData();
    void slotSaveExcel();
    void slotSetDIr();
    void slotChange1();
    void slotChange2();
    void slotChange3();
    void slotOpenDO();

signals:
    void signalReadKB(int, int locate);
    void signalSetKB(int type, float k, float b, int locate);
    void signalReadInfo(int type, int locate);
    void signalOpenDO(int locate);
    void signalCloseDO(int locate);
    void signalChange(int, int locate);
};

#endif // CHECKKBDIALOG_H
