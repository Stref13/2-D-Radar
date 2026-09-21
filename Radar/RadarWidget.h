#pragma once

#include "../Target/target.h"
#include "RadarMeasurements.h"

#include <vector>
#include <QWidget>
#include <QTimer>
#include <QMouseEvent>
#include <QElapsedTimer>

class RadarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RadarWidget(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
private slots:
    void updateFrame();
private:
    QTimer timer;
    QElapsedTimer frameClock;
    std::vector<Target> targets;
    std::vector<bool> wasDetected;
    double beamWidth = 10.0;     // Ширина луча [град]
    double scale = 1.0;          // Масштаб: пикселей на метр
    double maxRange = 1000.0;    // Дальность обнаружения [м]
    double currentAngle  = 0.0;  // Текущий угол поворота луча
    double rotationSpeed = 60.0; // Скорость вращения лучв [град/с]
    int    selectedTarget = -1;
    bool isTargetDetected(const RadarMeasurement& measurement) const;

};
