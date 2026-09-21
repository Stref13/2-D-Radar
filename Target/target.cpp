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

    timeSinceLastPoint += deltaTime;
    if(timeSinceLastPoint >= TRAJECTORY_SAMPLE_PERIOD)
    {
        addTrajectoryPoint();
        timeSinceLastPoint = 0.0;
    }
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

    if (trajectory.size() > MAX_TRAJECTORY_POINTS)
    {
        trajectory.removeFirst();
    }
}
