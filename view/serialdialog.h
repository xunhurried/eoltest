#ifndef SERIALDIALOG_H
#define SERIALDIALOG_H

#include <QDialog>

namespace Ui {
class SerialDialog;
}

class SerialDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialDialog(QWidget *parent = nullptr);
    ~SerialDialog();

private:
    Ui::SerialDialog *ui;

public slots:
    void accept() override;

signals:
    void signalSerial(const QString &);
};

#endif // SERIALDIALOG_H
