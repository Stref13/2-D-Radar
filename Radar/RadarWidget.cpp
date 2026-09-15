#include "RadarWidget.h"
#include <QPainter>
#include <QDateTime>
#include <cmath>
#include <QDebug>
#include <QIcon>
RadarWidget::RadarWidget(QWidget *parent)
    : QWidget(parent),
      target(100.0,150.0,50.0)
{
    setWindowTitle("Radar");
    setWindowIcon(QIcon(":/icons/radar_icon.jpg"));
    setMinimumSize(600,600);
    connect(&timer,&QTimer::timeout,this,&RadarWidget::updateFrame);
    timer.start(16); // ~60 FPS
}

void RadarWidget::updateFrame()
{
    double deltaTime = 0.016; // 16 ms

    if(calcTargetRange() > 250)
    {
        target.SetXYZ(100.0,150.0,50.0);
    }

    target.update(deltaTime);

    currentAngle += rotationSpeed * deltaTime;

    if (currentAngle >= 360.0)
    {
        currentAngle -= 360.0;
    }

    targetDetected = isTargetDetected();
    if (targetDetected)
    {
        qDebug() << "TARGET DETECTED";
        qDebug() << "Azimuth:"
                 << calcTargetAzimuth();
        qDebug() << "Range:"
                 << calcTargetRange();
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

    painter.setPen(QPen(Qt::green,2));
    painter.drawEllipse(QPointF(0,0),radius,radius);

    for(int i = 1; i<=4;++i)
    {
        painter.drawEllipse(QPointF(0,0),radius * i/4,radius * i/4);
    }

    painter.drawLine(-radius,0,radius,0);
    painter.drawLine(0,-radius,0,radius);

    double screen_targ_x = target.GetX();
    double screen_targ_y = -target.GetY();

    painter.setPen(Qt::NoPen);
    if(targetDetected)
        painter.setBrush(Qt::yellow);
    else
        painter.setBrush(Qt::red);
    double targSize = targetDetected ? 10 : 6;
    painter.drawEllipse(QPointF(screen_targ_x,screen_targ_y),targSize,targSize);

    painter.setPen(Qt::green);
    QString targ_info = QString("x: %1 y: %2").arg(target.GetX(),0,'f',1).arg(target.GetY(),0,'f',1);
    painter.drawText(screen_targ_x - 30, screen_targ_y - 12, targ_info);

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

double RadarWidget::calcTargetAzimuth() const
{
    double azimuthRad = atan2(-target.GetY(),target.GetX());

    double azimuthDegrees =
        azimuthRad * 180.0 / M_PI;

    if (azimuthDegrees < 0)
    {
        azimuthDegrees += 360.0;
    }

    return azimuthDegrees;
}


double RadarWidget::calcTargetRange() const
{
    double x = target.GetX();
    double y = target.GetY();
    return sqrt(x*x + y * y);
}

bool RadarWidget::isTargetDetected() const
{
    double targAz = calcTargetAzimuth();

    double beamStart = currentAngle;
    double beamEnd = currentAngle + 5.0;

    if (beamEnd < 360.0)
    {
        return targAz >= beamStart &&
               targAz <= beamEnd;
    }

    beamEnd -= 360.0;

    return targAz >= beamStart ||
           targAz <= beamEnd;

}
