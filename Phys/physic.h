#ifndef PHYSIC_H
#define PHYSIC_H

#include <QObject>
#include <QVector>
#include <QThread>
#include <QThreadPool>

#include "mainwindow.h"
#include "la.h"

class Physic : public QObject
{
    Q_OBJECT

public:
    explicit Physic(double dt, QObject *parent = 0);
    void pushBackLA(LA *object);
    QThread* connect_with_graph(MainWindow *wnd);

private:
    QVector<LA*> LAObjects;
    void UpdatePhys(double dt);
    QThreadPool* threadPool;
    double currentTime;
    double dt;
    bool updateFlag;

signals:
    void UpdateGraph(const QVector<LA*>* objects);

public slots:
    void StartToFly();
    void RocketEvent();

};

#endif // PHYSIC_H
