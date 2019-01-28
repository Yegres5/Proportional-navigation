#ifndef LA_H
#define LA_H

#include <QObject>
#include "angle.h"

#define _g 9.81

class LA : public QObject
{
    Q_OBJECT

protected:
    double x,y,z;
    double V;
    double n_xv,n_yv;
    Angle teta,psi,gamma;

public:
    LA(double x,double y,double z,double V,double n_xv,
       double n_yv,double teta,double psi,double gamma,
       const char* name = "LA");

    explicit LA(QObject *parent = nullptr);
    LA();

    double getX();
    double getY();
    double getZ();
    double GetV();
    double GetNy();
    double GetNx();
    Angle getTeta();
    Angle getPsi();
    Angle getGamma();

public slots:
   virtual void update(double dt);

};

#endif // LA_H
