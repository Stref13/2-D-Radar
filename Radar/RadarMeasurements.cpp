#include "RadarMeasurements.h"
#include <cmath>

namespace{
constexpr double kPi = 3.14159265358979323846;
constexpr double kRadToDeg = 180.0 / kPi;
}

RadarMeasurement calcMeasurements(const Target& target)
{
    double x = target.GetX();
    double y = target.GetY();
    double z = target.GetZ();

    const double horizontal = std::hypot(x,y);
    RadarMeasurement m;

    m.range = std::hypot(horizontal,z);

    m.azimuth = std::atan2(-y,x) * kRadToDeg;

    if (m.azimuth < 0)
    {
        m.azimuth += 360.0;
    }

    m.elevation =std::atan2(z,horizontal) * kRadToDeg;

    return m;
}
