#include "RadarWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <cmath>
#include <QDebug>
#include <QIcon>
#include <algorithm>

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

    wasDetected.assign(targets.size(),false);
    // Записываем начальное положение каждой цели
    for (Target& target:targets)
    {
        target.addTrajectoryPoint();
    }

    connect(&timer,&QTimer::timeout,this,&RadarWidget::updateFrame);
    timer.setTimerType(Qt::PreciseTimer);
    frameClock.start();
    timer.start(16); // ~60 FPS
}

void RadarWidget::updateFrame()
{
    double deltaTime = frameClock.restart()/1000.0;
    deltaTime = std::min(deltaTime, 0.1);

    for(Target& target : targets)
    {
        target.update(deltaTime);
    }

    currentAngle += rotationSpeed * deltaTime;

    if (currentAngle >= 360.0)
    {
        currentAngle -= 360.0;
    }

    for(size_t i =0;i<targets.size();++i)
    {
        RadarMeasurement measurement = calcMeasurements(targets[i]);
        bool detected = isTargetDetected(measurement);
        if (detected && !wasDetected[i]) // Выводим информацию один раз за проход лучем
        {
            qDebug() << "TARGET DETECTED"<<i+1;
            qDebug() << "Range:"
                     << measurement.range<<"м";
            qDebug() << "Azimuth:"
                     << measurement.azimuth<<"град";
            qDebug() << "Elevation:"
                     << measurement.elevation<<"град";
        }
        wasDetected[i] = detected;
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

    double radius = side/2.0 - 20;
    scale  = radius/maxRange;

    painter.setPen(QPen(Qt::green, 1));
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

    painter.save();
    QPainterPath clip;
    clip.addEllipse(QPointF(0, 0), radius, radius);
    painter.setClipPath(clip);
    // Отрисовка пунктиром траектории
    for(const Target& target:targets)
    {
        const QVector<QPointF>& trajectory = target.GetTrajectory();
        if(trajectory.size() < 2) continue;
        QPainterPath path;
        QPointF first_point(trajectory[0].x()*scale,-trajectory[0].y()*scale);
        path.moveTo(first_point);

        for(int j = 1 ; j<trajectory.size();++j)
        {
            QPointF point(trajectory[j].x()*scale ,-trajectory[j].y()*scale);
            path.lineTo(point);
        }
        path.lineTo(target.GetX() * scale, -target.GetY() * scale);

        QPen trajectory_pen(Qt::green,2);
        trajectory_pen.setStyle(Qt::DashLine);
        painter.setPen(trajectory_pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }
    painter.restore();
    for (size_t i = 0; i < targets.size();++i)
    {
        const Target& target = targets[i];
        RadarMeasurement measurement = calcMeasurements(target);
        if (measurement.range > maxRange) {continue;}
        double screen_targ_x = target.GetX()  * scale;
        double screen_targ_y = -target.GetY() * scale;
        bool detected = isTargetDetected(measurement);

        if(static_cast<int>(i) == selectedTarget)
        {
            painter.setPen(QPen(Qt::white,2));
            painter.drawEllipse(QPointF(screen_targ_x,screen_targ_y),7.0,7.0);
        }

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
    }

    if(selectedTarget >=0 &&
        selectedTarget < static_cast<int>(targets.size()))
    {
        const Target& target = targets[selectedTarget];
        RadarMeasurement measurement = calcMeasurements(target);
        if(measurement.range < maxRange){
            painter.save();
            painter.resetTransform();

            QRectF infoRect(20,20,90,140);

            painter.setBrush(QColor(0,0,0,200));
            painter.setPen(QPen(Qt::green, 2));

            painter.drawRect(infoRect);
            painter.setPen(Qt::green);

            QString info = QString("Target %1 \n"
                                   "\n"
                                   "R: %2\n"
                                   "Az: %3\n"
                                   "El: %4\n"
                                   "\n"
                                   "Vx: %5 m/s\n"
                                   "Vy: %6 m/s\n"
                                   "Vz: %7 m/s"
                                ).arg(selectedTarget + 1)
                                .arg(measurement.range, 0, 'f', 1)
                                .arg(measurement.azimuth, 0, 'f', 1)
                                .arg(measurement.elevation, 0, 'f', 1)
                                .arg(target.GetVx(), 0, 'f', 1)
                                .arg(target.GetVy(), 0, 'f', 1)
                                .arg(target.GetVz(), 0, 'f', 1);

            painter.drawText(infoRect.adjusted(10,10,-10,-10),Qt::AlignLeft | Qt::AlignTop,info);
            painter.restore();
        }

    }

    painter.save();
    painter.rotate(currentAngle);

    QRadialGradient gradient(0,0,radius);
    gradient.setColorAt(0, QColor(0,255,0,180));
    gradient.setColorAt(1, QColor(0,255,0,0));

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);

    painter.drawPie(QRectF(-radius, -radius, radius*2, radius*2),
                    0,
                    static_cast<int>(beamWidth*16));

    painter.restore();
}

void RadarWidget::mousePressEvent(QMouseEvent *event)
{

    double mouseX = event->pos().x() - width()/2.0;
    double mouseY = event->pos().y() - height()/2.0;

    double minDistance = 15.0;
    int newSelection = -1;
    for (size_t i = 0;i<targets.size();++i)
    {
        const Target& target = targets[i];
        if(calcMeasurements(target).range > maxRange) continue;
        double targetX = target.GetX()  * scale;
        double targetY = -target.GetY() * scale;

        double dx = mouseX - targetX;
        double dy = mouseY - targetY;

        double distance = sqrt(dx*dx+dy*dy);
        if (distance < minDistance)
        {
            minDistance = distance;
            newSelection = static_cast<int>(i);
        }
    }
    selectedTarget = newSelection;
    update();
}

bool RadarWidget::isTargetDetected(const RadarMeasurement& measurement) const
{
    if(measurement.range > maxRange)  return false;

    double behind = std::fmod(currentAngle - measurement.azimuth+360.0,360.0);

    return behind <= beamWidth;

}

