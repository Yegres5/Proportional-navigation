#include "physic.h"

#include <vector>
#include <QTime>
#include <QThreadPool>

#include "runnabletask.h"
#include "rocket.h"

#include <QDebug>

Physic::Physic(double dt, QObject *parent) :
  QObject(parent),
  currentTime(0),
  dt(dt),
  updateFlag(true)
{
    threadPool = new QThreadPool(this);
    threadPool->setMaxThreadCount(4);
}

void Physic::pushBackLA(LA* object)
{
    LAObjects.push_back(object);
    if (object->objectName() == "Rocket"){
        connect(qobject_cast<Rocket*> (object), SIGNAL(targetGetReached()),
                this  , SLOT(RocketEvent()));
    }
}

QThread *Physic::connect_with_graph(MainWindow* wnd)
{
    QThread *thread= new QThread();
    this->moveToThread(thread);

    connect(this,SIGNAL(UpdateGraph(const QVector<LA*>*)),
            wnd,SLOT(reDraw(const QVector<LA*>*)));

    connect(this,SIGNAL(UpdateSpeedGraph(const QVector<double>*)),
            wnd,SLOT(reDrawForSpeedWindow(const QVector<double>*)));

    connect(wnd,SIGNAL(DrawEnd()),
            this,SLOT(StartToFly()));

    return thread;
}

void Physic::UpdatePhys(double dt)
{
    QVector<runnableTask*> rocketTasks;

    QTime physTime;
    physTime.restart();
    for(QVector<LA*>::Iterator iter = LAObjects.begin();iter != LAObjects.end();iter++){
        LA* aaa = *iter;
        runnableTask* task = new runnableTask(aaa,dt);
        task->setAutoDelete(true);
        if (aaa->objectName() == "LA"){
            threadPool->start(task);
        }else{
            rocketTasks.push_back(task);
        }
    }
    threadPool->waitForDone();
    while (!rocketTasks.empty()){
        threadPool->start(rocketTasks.takeLast());
    }
    threadPool->waitForDone();
}

void Physic::StartToFly()
{
  if (updateFlag){
      currentTime += dt;
      UpdatePhys(dt);

      for(QVector<LA*>::Iterator iter = LAObjects.begin();iter != LAObjects.end();iter++){
          LA* aaa = *iter;
          if (aaa->objectName() == "Rocket"){
              Rocket* roc = dynamic_cast<Rocket*>(aaa);
              QVector<double>* vec = new QVector<double>(QVector<double>::fromStdVector(roc->TargetCoordinatesInSpeed));
              emit UpdateSpeedGraph(vec);
          }
      }

      emit UpdateGraph(&LAObjects);
    }
}

void Physic::RocketEvent()
{
    updateFlag = false;
}

