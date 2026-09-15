#pragma once

#include "../Target/Target.h"
#include "RadarMeasurements.h"
#include <QWidget>
#include <QTimer>

class RadarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RadarWidget(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
private slots:
    void updateFrame();
private:
    QTimer timer;
    Target target;
    double currentAngle  = 0.0;  // Текущий угол поворота луча
    double rotationSpeed = 60.0; // Скорость вращения лучв [град/с]

    bool targetDetected = false;
    bool isTargetDetected(const RadarMeasurement& measurement) const;

    RadarMeasurement calcMeasurements() const;
};
