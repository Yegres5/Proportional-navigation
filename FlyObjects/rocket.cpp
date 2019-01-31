#include "rocket.h"
#include <QDebug>
#include <QtMath>

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
//Gravity compensation
//    vector<double> grav = {0,1,0};
//    grav = toTrajectoryCoordinateSystem(grav);

//    gamma += isDoubleEqualToZero(grav[2]) ? 0 : atan(grav[2]/
//                                            sqrt(pow(grav[0],2)+pow(grav[1],2)));
    gamma = 0;
//    grav = {0,1,0};
//    grav = toTrajectoryCoordinateSystem(grav);
//    n_yv = grav[1];
//    n_xv = grav[0];

//Calculation target position, vector of target speed, vector of self speed
//TargetCoor speed coordinate system, TargetSpeed, SelfSpeed
    TargetCoor = {target->getX()-x,
                  target->getY()-y,
                  target->getZ()-z};

    vector<double> TargetSpeed = {target->GetV()*qCos(target->getTeta().getValue())*qCos(target->getPsi().getValue()),
                                  target->GetV()*qSin(target->getTeta().getValue()),
                                  -target->GetV()*qCos(target->getTeta().getValue())*qSin(target->getPsi().getValue())};

    TargetSpeed = toSpeedCoordinateSystem(TargetSpeed);

    vector<double> SelfSpeed = {V*qCos(teta.getValue())*qCos(psi.getValue()),
                                V*qSin(teta.getValue()),
                                -V*qCos(teta.getValue())*qSin(psi.getValue())};

//Proportional navigation

    double Ptmx = TargetCoor[0];
    double Ptmy = TargetCoor[1];
    double Ptmz = TargetCoor[2];

    double distance_to_target = sqrt(pow(Ptmx,2)+pow(Ptmy,2)+pow(Ptmz,2));

    double lambda_xy = CalcAngle(Ptmx,Ptmy);
    double lambda_xz = CalcAngle(Ptmx,-Ptmz);
    double lambda_yz = CalcAngle(Ptmz,Ptmy);

    double beta_xy = CalcAngle(TargetSpeed[0], TargetSpeed[1]);
    double beta_xz = CalcAngle(TargetSpeed[0], -TargetSpeed[2]);
    double beta_yz = CalcAngle(TargetSpeed[2], TargetSpeed[1]);

    double Vtxy = sqrt(pow(TargetSpeed[0],2) + pow(TargetSpeed[1],2));
    double Vtxz = sqrt(pow(TargetSpeed[0],2) + pow(TargetSpeed[2],2));
    double Vtyz = sqrt(pow(TargetSpeed[1],2) + pow(TargetSpeed[2],2));

    double Lxy = asin((Vtxy*sin(beta_xy + lambda_xy))/V);
    double Lxz = asin((Vtxz*sin(beta_xz + lambda_xz))/V);
    double Lyz = asin((Vtyz*sin(beta_yz + lambda_yz))/V);

    double Vtmx = TargetSpeed[0] - SelfSpeed[0];
    double Vtmy = TargetSpeed[1] - SelfSpeed[1];
    double Vtmz = TargetSpeed[2] - SelfSpeed[2];

    //increment lambda
    double lambda_xy_ = ( !isDoubleEqualToZero(pow(Ptmx,2) + pow(Ptmy,2)) )
            ? (Ptmx*Vtmy - Ptmy*Vtmx)/(pow(Ptmx,2) + pow(Ptmy,2))
            : 0;
    double lambda_xz_ = ( !isDoubleEqualToZero((pow(Ptmx,2) + pow(Ptmz,2))) )
            ? (Ptmx*Vtmz - Ptmz*Vtmx)/(pow(Ptmx,2) + pow(Ptmz,2))
            : 0;
    double lambda_yz_ = ( !isDoubleEqualToZero(pow(Ptmy,2) + pow(Ptmz,2)) )
            ? (Ptmy*Vtmz - Ptmz*Vtmy)/(pow(Ptmy,2) + pow(Ptmz,2))
            : 0;

    double Vcxy = ( sqrt(pow(Ptmx,2) + pow(Ptmy,2)) != 0 )
            ? -(Ptmx*Vtmx + Ptmy*Vtmy)/sqrt(pow(Ptmx,2) + pow(Ptmy,2))
            : 0;
    double Vcxz = ( sqrt(pow(Ptmx,2) + pow(Ptmz,2)) != 0 )
            ? -(Ptmx*Vtmx + Ptmz*Vtmz)/sqrt(pow(Ptmx,2) + pow(Ptmz,2))
            : 0;
    double Vcyz = ( sqrt(pow(Ptmy,2) + pow(Ptmz,2)) != 0 )
            ? -(Ptmy*Vtmy + Ptmz*Vtmz)/sqrt(pow(Ptmy,2) + pow(Ptmz,2))
            : 0;

    double nc_xy = K*Vcxy*lambda_xy_;   // pitch
    double nc_xz = K*Vcxz*lambda_xz_;   // roll
    double nc_yz = K*Vcyz*lambda_yz_;   // yaw

    double amx = - nc_xy*sin(lambda_xy) - nc_xz*sin(lambda_xz);
    double amy =   nc_xy*cos(lambda_xy) - nc_yz*sin(lambda_yz);
    double amz =   nc_xz*cos(lambda_xz) + nc_yz*cos(lambda_yz);

    double a_pitch = amy*cos(teta.getValue());
    double a_roll = - amx*sin(psi.getValue()) + amz*cos(psi.getValue());// a_roll = amz
    qDebug() << "Roll acceleration is = " << a_roll;

//Summ of gravity, roll and pitch
    vector<double> grav = {0,1,0};
    grav = toTrajectoryCoordinateSystem(grav);

    n_xv = grav[0];
    n_yv = a_pitch + grav[1];
    double n_roll = a_roll + grav[2];
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

qDebug() << "Distance to target is = " << distance_to_target;

// Target get reached by rocket
    if (distance_to_target - sqrt(pow(Vtmx,2)+pow(Vtmy,2) + pow(Vtmz,2))
            <= 0 || distance_to_target <= r_explode){
        emit targetGetReached();
    }
    qDebug() << "";
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
        if (isDoubleEqualToZero(dy)){
            return 0;
        }else{
            if (dy>0){
                return M_PI_2;
            }else{
                return M_PI+M_PI_2;
            }
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
