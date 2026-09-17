#include "target.h"


Target::Target(double x, double y, double z,double vx,double vy,double vz)
    : x(x),
      y(y),
      z(z),
      vx(vx),
      vy(vy),
      vz(vz) {}

void Target::update(double deltaTime)
{
    x += vx * deltaTime;
    y += vy * deltaTime;
    z += vz * deltaTime;
}

void Target::SetXYZ(double coord_x, double coord_y, double coord_z)
{
    x = coord_x;
    y = coord_y;
    z = coord_z;
}

void Target::addTrajectoryPoint()
{
    trajectory.append(QPointF(x,y));
}
