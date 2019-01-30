#include "mainwindow.h"
#include <QApplication>

#include "rocket.h"
#include "la.h"
#include "physic.h"
#include <QtMath>


double AngleToTarget(double x, double y, LA *target);
int randomBTW(int LOW, int HIGH);

#define isDoubleEqualToZero(x) ( fabs(x) < 0.1e-5)
#define LA_Rocket "Rocket"
#define LA_Target "Target"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    Physic objects(1,nullptr);

    LA* la = new LA(30000,0,0,320*1,0,1,   //25000,-7000,7000
                    M_PI/180*(0),  //Teta
                    M_PI/180*(0),  //Psi
                    M_PI/180*(0),  //Gamma
                    LA_Target);

    la->setObjectName("LA");
    objects.pushBackLA(la);
    w.createHash(la);

    for(int i(0);i<1;i++){
        LA* roc = new Rocket(0,0,0,320*1.1,0,1,
                             M_PI/180*(0),  //Teta
                             M_PI/180*(0),  //Psi
                             M_PI/180*(0),  //Gamma
                             la,1,LA_Rocket);
        roc->setObjectName("Rocket");

        objects.pushBackLA(roc);
        w.createHash(roc);
    }


    QThread* thread = objects.connect_with_graph(&w);
    w.show();
    thread->start();

    return a.exec();
}

double AngleToTarget(double x, double y,LA* target)
{
    double dx = target->getX()-x;
    double dy = target->getY()-y;

    if (isDoubleEqualToZero(dx)){
        if (dy>0){
            return M_PI_2;
        }else{
            return M_PI+M_PI_2;
        }
    }

    if (isDoubleEqualToZero(dy)){
        if (dx>0){
            return 0;
        }else{
            return M_PI;
        }
    }

    if (dx < 0){
        if(dy < 0){

            return atan(fabs(dy/dx))+M_PI;
        }else{
            return M_PI-atan(fabs(dy/dx));
        }
    }else{
        if (dy < 0){
            return 2*M_PI-atan(fabs(dy/dx));
        }else{
            return atan(fabs(dy/dx));
        }
    }
}

int randomBTW(int LOW, int HIGH){
    return rand() % ((HIGH + 1) - LOW) + LOW;
}
