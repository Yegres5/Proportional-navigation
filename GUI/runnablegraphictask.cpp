#include "runnablegraphictask.h"

#include <QDebug>
#include <QThread>
runnableGraphicTask::runnableGraphicTask(drawingObject *obj, int x, int y)
    :obj(obj),x(x),y(y)
{
    qDebug() << "Constructor of runnable task";
    qDebug() << "Runnable thread: " << QThread::currentThreadId();
}

void runnableGraphicTask::run()
{
    qDebug() << "Auto delete " << autoDelete();
    obj->setPos(x,y);
}
