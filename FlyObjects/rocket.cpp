#include "rocket.h"
#include <QDebug>
#include <QtMath>
#include <QFile>

#define isDoubleEqualToZero(x) ( fabs(x) < 0.1e-5)

double CalcAngle(double dx, double dy);
using std::vector;

Rocket::Rocket(double x, double y, double z, double V, double n_xv,
               double n_yv, double teta, double psi, double gamma, LA *target, double K,
               const char *name)
      :LA(x,y,z,V,
       n_xv,
       n_yv,
       teta,psi,gamma,name),
       target(target),
       K(K)
{
    this->teta.check = false;//CHANGE
    this->gamma.check = false;//CHANGE
    r_explode = 11; //CHANGE

    TargetCoor = toSpeedCoordinateSystem(vector<double> ({{target->getX(),target->getY(),target->getZ()}}));
    r = sqrt(pow(TargetCoor[0],2)+pow(TargetCoor[2],2));
    r_XY = sqrt(pow(TargetCoor[0],2)+pow(TargetCoor[1],2));
}

void Rocket::update(double dt)
{
    qDebug("%.3f,%.3f,%.3f",x,y,z);

    QString filename = "C:/Users/Kokao/PycharmProjects/untitled/rocket.csv";
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream stream(&file);
        stream << x << "," << y << "," << z << endl;
    }
    file.close();

    TargetCoordinatesInSpeed = toSpeedCoordinateSystem(vector<double> ({{target->getX()-x,
                                                           target->getY()-y,
                                                           target->getZ()-z}}));
//Gravity compensation
    vector<double> grav = {0,1,0};
    grav = toTrajectoryCoordinateSystem(grav);

    gamma += isDoubleEqualToZero(grav[2]) ? 0 : atan(grav[2]/
                                            sqrt(pow(grav[0],2)+pow(grav[1],2)));



    grav = {0,1,0};
    grav = toTrajectoryCoordinateSystem(grav);
    n_xv = grav[0];
    n_yv = grav[1];

//Calculation target position, vector of target speed, vector of self speed
    TargetCoor = toSpeedCoordinateSystem(vector<double> ({{target->getX()-x,
                                                           target->getY()-y,
                                                           target->getZ()-z}}));


    vector<double> TargetSpeed = {target->GetV()*qCos(target->getTeta().getValue())*qCos(target->getPsi().getValue()),
                                  target->GetV()*qSin(target->getTeta().getValue()),
                                  -target->GetV()*qCos(target->getTeta().getValue())*qSin(target->getPsi().getValue())};

    TargetSpeed = toSpeedCoordinateSystem(TargetSpeed);

    vector<double>* toSpherical = new vector<double>{sqrt(pow(TargetSpeed[0],2)+
                                                          pow(TargetSpeed[1],2)+
                                                          pow(TargetSpeed[2],2)),
                                                     qAcos(sqrt(pow(TargetSpeed[0],2)+
                                                                pow(TargetSpeed[2],2))/
                                                           sqrt(pow(TargetSpeed[0],2)+
                                                                pow(TargetSpeed[1],2)+
                                                                pow(TargetSpeed[2],2)))*(TargetSpeed[1] < 0 ? -1 : 1),
                                                     CalcAngle(TargetSpeed[0],-TargetSpeed[2])};
    std::copy(toSpherical->begin(),toSpherical->end(),TargetSpeed.begin());
    delete toSpherical;

    vector<double> SelfSpeed = {V*qCos(teta.getValue())*qCos(psi.getValue()),
                                V*qSin(teta.getValue()),
                                -V*qCos(teta.getValue())*qSin(psi.getValue())};
    SelfSpeed = toSpeedCoordinateSystem(SelfSpeed);

    toSpherical = new vector<double>{sqrt(pow(SelfSpeed[0],2)+
                                          pow(SelfSpeed[1],2)+
                                          pow(SelfSpeed[2],2)),
                                     qAcos(sqrt(pow(SelfSpeed[0],2)+
                                                pow(SelfSpeed[2],2))/
                                           sqrt(pow(SelfSpeed[0],2)+
                                           pow(SelfSpeed[1],2)+
                                           pow(SelfSpeed[2],2))),
                                     CalcAngle(SelfSpeed[0],-SelfSpeed[2])};

    std::copy(toSpherical->begin(),toSpherical->end(),SelfSpeed.begin());
    delete toSpherical;

//FOR XY (pitch)
    vector<double> SelfSpeedXY = {SelfSpeed[0], 0};

    vector<double> TargetSpeedXY = {TargetSpeed[0]*cos(TargetSpeed[1])*cos(TargetSpeed[2]),
                                    TargetSpeed[0]*sin(TargetSpeed[1])};

    TargetSpeedXY = {sqrt(pow(TargetSpeedXY[0],2) + pow(TargetSpeedXY[1],2)),
                     CalcAngle(TargetSpeedXY[0],TargetSpeedXY[1])};

    double sigma_R_XY = SelfSpeedXY[1] - CalcLambdaYX();
    double sigma_T_XY = TargetSpeedXY[1] - CalcLambdaYX();

    r_XY = sqrt(pow(TargetCoor[0],2) + pow(TargetCoor[1],2));

    double d_lambda_XY = (TargetSpeedXY[0]*sin(sigma_T_XY) - SelfSpeedXY[0]*sin(sigma_R_XY))/r_XY;
    //qDebug("---dLambda = %5.f", d_lambda_XY);
    double W_XY = K*V*d_lambda_XY;
    double n_pitch = W_XY/_g;

// FOR XZ (roll)
    vector<double> SelfSpeedXZ = {SelfSpeed[0]*cos(SelfSpeed[1]),
                                  SelfSpeed[2]};
    vector<double> TargetSpeedXZ = {TargetSpeed[0]*cos(TargetSpeed[1]),
                                    TargetSpeed[2]};

    double sigma_R = SelfSpeedXZ[1] - CalcLambdaXZ();
    double sigma_T = TargetSpeedXZ[1] - CalcLambdaXZ();
    r = sqrt(pow(TargetCoor[0],2) + pow(TargetCoor[2],2));
    double d_lambda = (TargetSpeedXZ[0]*sin(sigma_T) - SelfSpeedXZ[0]*sin(sigma_R))/r;
    double W = -K*V*d_lambda;
    double n_roll = W/_g;

// Target get reached by rocket
    if (r <= 800 && r_XY <= 800){
        emit targetGetReached();
    }

//Summ of gravity, roll and pitch
    n_yv += n_pitch;

    gamma += isDoubleEqualToZero(n_roll) ? 0 : atan(n_roll/n_yv);
    n_yv = sqrt(pow(n_yv,2) + pow(n_roll,2))*(n_yv > 0 ? 1 : -1);
    n_roll = 0;

//Equations of motion
    V += (n_xv - sin(teta.getValue()))*_g*dt;
    teta += (n_yv*cos(gamma.getValue())-cos(teta.getValue()))*_g/V*dt;
    psi += -n_yv*sin(gamma.getValue())*_g/(V*cos(teta.getValue()))*dt;
    x += V*cos(teta.getValue())*cos(psi.getValue())*dt;
    y += V*sin(teta.getValue())*dt;
    z += -V*cos(teta.getValue())*sin(psi.getValue())*dt;
}

double Rocket::GetSigmaT()
{
    double dx = sqrt(pow(target->getX()-x,2)+pow(target->getZ()-z,2));
    double dy = target->getY()-y;

    if (isDoubleEqualToZero(dx)){
        if (dy>0){
            return M_PI_2;
        }else{
            return -M_PI_2;
        }
    }

    if (isDoubleEqualToZero(dy)){
        if (dx>0){
            return 0;
        }
    }

    return atan(dy/dx);
}

double Rocket::CalcLambdaXZ()
{
    double dx = TargetCoor[0];  //X
    double dy = -TargetCoor[2];  //Z

    return CalcAngle(dx,dy);
}

double Rocket::CalcLambdaYX()
{
    double dx = TargetCoor[0];  //X
    double dy = TargetCoor[1];  //Z

    return CalcAngle(dx,dy);
}

vector<double> Rocket::CalcV_XYprojection(vector<double> speed)
{
  vector<double> resultSpeed(2);
  resultSpeed[0] = sqrt(pow(speed[0]*cos(speed[1])*cos(speed[2]),2)+
                        pow(speed[0]*sin(speed[1]),2));
  resultSpeed[1] = CalcAngle(speed[0]*cos(speed[1])*cos(speed[2]),
                             speed[0]*sin(speed[1]));
  return resultSpeed;
}

double Rocket::GetVT()
{
  return target->GetV();
}

double Rocket::CalcXY_Angle()
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
            return atan(dy/dx)+M_PI;
        }else{
            return atan(dy/dx)+M_PI;
        }
    }else{
        if (dy < 0){
            return 2*M_PI-atan(dy/dx);
        }else{
            return atan(dy/dx);
        }
    }
}

bool Rocket::checkForRoll()
{

    if(teta.getValue() > M_PI_2){
        teta = M_PI-teta.getValue();
        psi.incValue(M_PI);
        return true;
    }else{
        if(teta.getValue() < -M_PI_2){
            teta = -M_PI-teta.getValue();
            psi.incValue(M_PI);
            return true;
        }
    }
    return false;

}

std::vector<double> Rocket::toSpeedCoordinateSystem(std::vector<double> vec)
{
    double teta  = -this->teta.getValue();
    double psi   = -this->psi.getValue();
    double gamma = -this->gamma.getValue();

    std::vector<std::vector<double>> arr{
                        {qCos(teta)*qCos(psi),                                    qSin(teta),              -qCos(teta)*qSin(psi)},
                        {-qCos(gamma)*qSin(teta)*qCos(psi)+qSin(gamma)*qSin(psi), qCos(gamma)*qCos(teta),  qCos(gamma)*qSin(teta)*qSin(psi)+qSin(gamma)*qCos(psi)},
                        {qSin(gamma)*qSin(teta)*qCos(psi)+qCos(gamma)*qSin(psi),  -qSin(gamma)*qCos(teta), -qSin(psi)*qSin(teta)*qSin(gamma)+qCos(psi)*qCos(gamma)}};

    vector<double> Result(3);

    for(size_t i = 0; i < arr.at(0).size(); i++){
        for(size_t j = 0; j < vec.size(); j++){
            Result[i] += vec.at(j)*arr.at(j).at(i);
        }
    }
    return Result;
}

vector<double> Rocket::toTrajectoryCoordinateSystem(vector<double> vec)
{
    double teta  = -this->teta.getValue();
    double psi   = 0;//-this->psi.getValue();
    double gamma = -this->gamma.getValue();

    std::vector<std::vector<double>> arr{
                        {qCos(teta)*qCos(psi),                                    qSin(teta),              -qCos(teta)*qSin(psi)},
                        {-qCos(gamma)*qSin(teta)*qCos(psi)+qSin(gamma)*qSin(psi), qCos(gamma)*qCos(teta),  qCos(gamma)*qSin(teta)*qSin(psi)+qSin(gamma)*qCos(psi)},
                        {qSin(gamma)*qSin(teta)*qCos(psi)+qCos(gamma)*qSin(psi),  -qSin(gamma)*qCos(teta), -qSin(psi)*qSin(teta)*qSin(gamma)+qCos(psi)*qCos(gamma)}};

    vector<double> Result(3);

    for(size_t i = 0; i < arr.at(0).size(); i++){
        for(size_t j = 0; j < vec.size(); j++){
            Result[i] += vec.at(j)*arr.at(j).at(i);
        }
    }
    return Result;
}

double CalcAngle(double dx, double dy){
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
            return atan(dy/dx)+M_PI;
        }else{
            return atan(dy/dx)+M_PI;
        }
    }else{
        if (dy < 0){
            return 2*M_PI+atan(dy/dx);
        }else{
            return atan(dy/dx);
        }
    }
}
