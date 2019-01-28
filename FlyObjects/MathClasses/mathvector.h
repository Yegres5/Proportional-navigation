#ifndef MATHVECTOR_H
#define MATHVECTOR_H

#include "angle.h"

//TODO: Make a sum for two vectors in spherical coordinate system.

class MathVector
{
public:
    MathVector(double value, double fi, double teta);
    ~MathVector();

    double value;
    Angle  fi,teta;

    MathVector& operator+=(const MathVector vec);
    MathVector  operator* (double vec);

};

#endif // MATHVECTOR_H
