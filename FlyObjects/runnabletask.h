#ifndef RUNNABLETASK_H
#define RUNNABLETASK_H

#include <QObject>
#include <QRunnable>
#include "la.h"


class runnableTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    runnableTask(LA* la,double dt);
    void run() override;

private:
    LA* la;
    double dt;
};

#endif // RUNNABLETASK_H
