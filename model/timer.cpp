#include "timer.h"

Timer::Timer()
{
    timer = new QTimer();
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimeOut()), Qt::DirectConnection);
}

void Timer::slotTimeOut()
{
    emit signalTimeOut();
}

void Timer::slotStopTimer()
{
    if(timer->isActive())
    {
        timer->stop();
    }
}

void Timer::slotStartTimer()
{
    timer->start(200);
}
