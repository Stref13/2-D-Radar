#pragma once

#include "../Target/Target.h"
#include "RadarMeasurements.h"

#include <vector>
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
    std::vector<Target> targets;
    double scale;                //Перевод из пикселей в метры
    double maxRange = 1000.0;    // Дальность обнаружения [м]
    double currentAngle  = 0.0;  // Текущий угол поворота луча
    double rotationSpeed = 60.0; // Скорость вращения лучв [град/с]

    // std::vector<bool> detectedTargets = false;
    bool isTargetDetected(const RadarMeasurement& measurement) const;

    RadarMeasurement calcMeasurements(const Target& target) const;
};
