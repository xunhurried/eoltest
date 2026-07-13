#ifndef COMMUNICATESETTINGDIALOG_H
#define COMMUNICATESETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class CommunicateSettingDialog;
}

class CommunicateSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommunicateSettingDialog(QWidget *parent = nullptr);
    ~CommunicateSettingDialog();

private:
    Ui::CommunicateSettingDialog *ui;

signals:
    void signalStartSeriol(const QString &, int);

public slots:
    void accept() override;
};

#endif // COMMUNICATESETTINGDIALOG_H
