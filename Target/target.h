#pragma once
#include <QVector>
#include <QPointF>

static constexpr int MAX_TRAJECTORY_POINTS = 300;
static constexpr double TRAJECTORY_SAMPLE_PERIOD = 0.2;

class Target
{
public:
    Target(double x, double y, double z, double vx,double vy,double vz);

    void update(double deltaTime);

    void addTrajectoryPoint();
    const QVector<QPointF>& GetTrajectory() const {return trajectory;}
    double GetX() const {return x;}
    double GetY() const {return y;}
    double GetZ() const {return z;}

    void SetX(double coord_x) {x = coord_x;}
    void SetY(double coord_y) {y = coord_y;}
    void SetZ(double coord_z) {z = coord_z;}
    void SetXYZ(double coord_x, double coord_y, double coord_z);

    double GetVx() const {return vx;}
    double GetVy() const {return vy;}
    double GetVz() const {return vz;}
private:
    QVector<QPointF> trajectory;

    double timeSinceLastPoint = 0.0;

    double x; // Координата x [м]
    double y; // Координата y [м]
    double z; // Координата z [м]

    double vx; // Составляющая скорости vx [м/с]
    double vy; // Составляющая скорости vy [м/с]
    double vz; // Составляющая скорости vz [м/с]
};

