#include "mathvector.h"

#include <QtMath>

#define roundDouble(x,y) (int(x*pow(10,y))/pow(10,y))

MathVector::MathVector(double value, double fi, double teta):
    value(value),fi(fi),teta(teta)
{}

MathVector::~MathVector()
{}

MathVector &MathVector::operator+=(const MathVector vec)
{
    if (value){
        this->value = sqrt(this->value*this->value+
                           vec.value*vec.value-
                           2*this->value*vec.value*
                           roundDouble(cos(M_PI-vec.angle.getValue()+this->angle.getValue()),5));
        if (value)
            this->angle.incValue(qAsin(vec.value/this->value*sin(M_PI-vec.angle.getValue()+this->angle.getValue())));

    }else{
        this->value = vec.value;
        this->angle = vec.angle;
    }

    return *this;
}

MathVector MathVector::operator*(double vec)
{
    double value = this->value*vec;
    MathVector pVec = MathVector(value,this->angle.getValue());
    return pVec;
}
