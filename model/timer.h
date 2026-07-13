#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QTimer>

class Timer : public QObject
{
    Q_OBJECT
public:
    Timer();

private:
    QTimer *timer;

public slots:
    void slotTimeOut();
    void slotStopTimer();
    void slotStartTimer();

signals:
    void signalTimeOut();
};

#endif // TIMER_H
