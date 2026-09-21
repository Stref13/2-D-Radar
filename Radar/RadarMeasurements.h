#pragma once
#include "../Target/target.h"

struct RadarMeasurement
{
    double range;     // Наклонная дальность, м
    double azimuth;   // Азимут, градусы
    double elevation; // Угол места, градусы
};

RadarMeasurement calcMeasurements(const Target& target);
