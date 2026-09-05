#pragma once

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
    bool targetDetected = false;
    double currentAngle  = 0.0;  // Текущий угол поворота луча
    double rotationSpeed = 60.0; // Скорость вращения лучв [град/с]
    double target_x = 100;
    double target_y = 150;
    double target_z = 50;

    double calcTargetAzimuth() const;
    double calcTargetRange() const;
    bool   isTargetDetected() const;
};
