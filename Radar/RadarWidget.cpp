#include "RadarWidget.h"
#include <QPainter>
#include <QDateTime>
#include <cmath>
#include <QDebug>
#include <QIcon>
RadarWidget::RadarWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Radar");
    setWindowIcon(QIcon(":/icons/radar_icon.jpg"));
    setMinimumSize(600,600);
    targets.emplace_back(400.0,600.0,120.0,
                         20.0, -10.0, 0.0);
    targets.emplace_back(-500.0,300.0,200.0,
                         15.0, 5.0, 0.0);
    targets.emplace_back(200.0,-700.0,80.0,
                         -10.0, 20.0, 0.0);
    connect(&timer,&QTimer::timeout,this,&RadarWidget::updateFrame);
    timer.start(16); // ~60 FPS
}

void RadarWidget::updateFrame()
{
    double deltaTime = 0.016; // 16 ms

    for(Target& target : targets)
    {
        target.update(deltaTime);
    }

    currentAngle += rotationSpeed * deltaTime;

    if (currentAngle >= 360.0)
    {
        currentAngle -= 360.0;
    }

    // RadarMeasurement measurement = calcMeasurements(target);
    // if(measurement.range > 1000)
    // {
    //     target.SetXYZ(400.0,600.0,120.0);
    // }
    for(size_t i =0;i<targets.size();++i)
    {
        const Target& target = targets[i];
        RadarMeasurement measurement = calcMeasurements(target);
        bool detected = isTargetDetected(measurement);/**/
        if (detected)
        {
            qDebug() << "TARGET DETECTED"<<i+1;
            qDebug() << "Range:"
                     << measurement.range<<"м";
            qDebug() << "Azimuth:"
                     << measurement.azimuth<<"град";
            qDebug() << "Elevation:"
                     << measurement.elevation<<"град";
        }
    }
    update(); // Перерисовка
}

void RadarWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::black);

    int side = qMin(width(), height());
    painter.translate(width()/2, height()/2);

    double radius = side/2 - 20;
    scale  = radius/maxRange;

    painter.setPen(QPen(Qt::green,2));
    painter.drawEllipse(QPointF(0,0),radius,radius);

    for(int i = 1; i<=4;++i)
    {
        double ringRadius = radius * i / 4;
        painter.drawEllipse(QPointF(0,0),ringRadius,ringRadius);
        painter.setPen(Qt::green);
        QString ringText = QString::number(maxRange * i /4) + "м";
        painter.drawText(0,-ringRadius-5,ringText);
    }

    painter.drawLine(-radius,0,radius,0);
    painter.drawLine(0,-radius,0,radius);

    for (size_t i = 0; i < targets.size();++i)
    {
        const Target& target = targets[i];
        RadarMeasurement measurement = calcMeasurements(target);
        if (measurement.range > maxRange) {continue;}
        double screen_targ_x = target.GetX()  * scale;
        double screen_targ_y = -target.GetY() * scale;
        bool detected = isTargetDetected(measurement);
        if(detected)
            painter.setBrush(Qt::yellow);
        else
            painter.setBrush(Qt::red);
        painter.setPen(Qt::NoPen);
        double targSize = detected ? 10 : 6;
        painter.drawEllipse(QPointF(screen_targ_x,screen_targ_y),targSize,targSize);

        painter.setPen(Qt::green);
        QString targetLabel = QString("T%1").arg(i+1);
        painter.drawText(screen_targ_x+8, screen_targ_y+5, targetLabel);
        QString targ_info = QString("Targ: %1\n" " R: %2\n" " Az: %3\n" " El: %4")
                .arg(i+1)
                .arg(measurement.range,0,'f',1)
                .arg(measurement.azimuth,0,'f',1)
                .arg(measurement.elevation,0,'f',1);
        painter.drawText(-radius+15, -radius+30+i*15, targ_info);
    }

    painter.save();
    painter.rotate(currentAngle);

    QRadialGradient gradient(0,0,radius);
    gradient.setColorAt(0, QColor(0,255,0,180));
    gradient.setColorAt(1, QColor(0,255,0,0));

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);

    painter.drawPie(QRectF(-radius, -radius, radius*2, radius*2),
                    0 * 16,
                    10 * 16);

    painter.restore();
}

bool RadarWidget::isTargetDetected(const RadarMeasurement& measurement) const
{
    if(measurement.range > maxRange)  return false;
    double beamStart = currentAngle;
    double beamEnd = currentAngle + 5.0;

    if (beamEnd < 360.0)
    {
        return measurement.azimuth >= beamStart &&
               measurement.azimuth <= beamEnd;
    }

    beamEnd -= 360.0;

    return measurement.azimuth >= beamStart ||
           measurement.azimuth <= beamEnd;

}

RadarMeasurement RadarWidget::calcMeasurements(const Target& target) const
{
    double x = target.GetX();
    double y = target.GetY();
    double z = target.GetZ();

    RadarMeasurement measurement;

    measurement.range =
            sqrt(x*x + y*y + z*z);

    measurement.azimuth =
            atan2(-y,x)*180/M_PI;

    if (measurement.azimuth < 0)
    {
        measurement.azimuth += 360.0;
    }

    double horizontalRange =
        sqrt(x * x + y * y);

    measurement.elevation =
        atan2(z,horizontalRange) * 180.0 / M_PI;

    return measurement;
}
