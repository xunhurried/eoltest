#ifndef CHOOSEDEVICEDIALOG_H
#define CHOOSEDEVICEDIALOG_H

#include <QDialog>

namespace Ui {
class ChooseDeviceDialog;
}

class ChooseDeviceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseDeviceDialog(QWidget *parent = nullptr, int type = 1);
    ~ChooseDeviceDialog();

private:
    void hideLabel();

private:
    Ui::ChooseDeviceDialog *ui;
    int m_type;

public slots:
    void slotConfirm();

signals:
    void signalChooseDevice(int, int, int, int);
    void signalChooseZLGType(int, int, int, int);
    void signalChooseGCType(int, int, int, int);
};

#endif // CHOOSEDEVICEDIALOG_H
