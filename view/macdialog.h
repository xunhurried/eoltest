#ifndef MACDIALOG_H
#define MACDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class MACDialog;
}

class MACDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MACDialog(QWidget *parent = nullptr, int type = 0);
    ~MACDialog();

private:
    Ui::MACDialog *ui;
    int m_type;
    bool if_first = true;

public slots:
    void accept() override;

signals:
    void signalMAC(const QString &);
    void signalSerial(const QString &);
};

#endif // MACDIALOG_H
