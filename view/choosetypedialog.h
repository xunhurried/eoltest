#ifndef CHOOSETYPEDIALOG_H
#define CHOOSETYPEDIALOG_H

#include <QDialog>

namespace Ui {
class chooseTypeDialog;
}

class ChooseTypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseTypeDialog(QWidget *parent = nullptr);
    ~ChooseTypeDialog();

private:
    Ui::chooseTypeDialog *ui;
    int type = 0;

public slots:
    void accept() override;
    void reject() override;
    void slotChangeGroup1(int index);
    void slotChangeGroup2(int index);
    void slotChangeGroup3(int index);
    void slotChangeGroup4(int index);

signals:
    void signalStartType(bool, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int);
    void signalQuit();
};

#endif // CHOOSETYPEDIALOG_H
