#include "runnabletask.h"

runnableTask::runnableTask(LA *la, double dt)
    :la(la),dt(dt)
{}

void runnableTask::run()
{
    la->update(dt);
}



