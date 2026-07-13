#ifndef ADMINLOGDIALOG_H
#define ADMINLOGDIALOG_H

#include <QDialog>

namespace Ui {
class AdminLogDialog;
}

class AdminLogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdminLogDialog(QWidget *parent = nullptr);
    ~AdminLogDialog();

private:
    Ui::AdminLogDialog *ui;

public slots:
    void accept() override;

signals:
    void signalSuccess();
};

#endif // ADMINLOGDIALOG_H
