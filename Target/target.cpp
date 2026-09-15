#include "target.h"


Target::Target(double x, double y, double z)
    : x(x),
      y(y),
      z(z),
      vx(20.0),
      vy(-10.0),
      vz(0.0) {}

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
