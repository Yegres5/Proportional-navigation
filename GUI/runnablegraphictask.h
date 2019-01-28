#ifndef RUNNABLEGRAPHICTASK_H
#define RUNNABLEGRAPHICTASK_H

#include <QObject>
#include <QRunnable>
#include "drawingobject.h"

//TODO: multithreading.
//Class for multithreaded rendering items on QGraphicsScene.

class runnableGraphicTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    runnableGraphicTask(drawingObject* obj,int x,int y);
    void run() override;

private:
    drawingObject* obj;
    int x,y;
};


#endif // RUNNABLEGRAPHICTASK_H
