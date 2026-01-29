#include "stereonetwidget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPainterPath>

StereonetWidget::StereonetWidget(QWidget *parent)
    : QWidget(parent),
      m_projectionType(StereonetType::EqualArea),
      m_radius(200)
{
    computeGrid();
}

void StereonetWidget::setProjectionType(StereonetType type)
{
    m_projectionType = type;
    computeGrid();
    update();
}

void StereonetWidget::addPlane(const Plane& plane)
{
    m_planes.append(plane);
    update();
}

void StereonetWidget::clearPlanes()
{
    m_planes.clear();
    update();
}

void StereonetWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    // 计算中心点
    QPoint center = rect().center();
    
    // 绘制立体网参考圆
    painter.setPen(QPen(Qt::black, 1));
    painter.drawEllipse(center, static_cast<int>(m_radius), static_cast<int>(m_radius));
    
    // 绘制小圆圈, 此为错误代码
    painter.setPen(QPen(Qt::gray, 0.5));
    for (const auto& circle : m_smallCircles) {
        QPainterPath path;
        bool first = true;
        QVector<QPointF> tmpPoints;
        for (const auto& point : circle) {
            QPointF widgetPoint = mapToWidget(point);
            tmpPoints.push_back(widgetPoint);
            if (first) {
                path.moveTo(widgetPoint);
                first = false;
            } else {
                path.lineTo(widgetPoint);
            }
        }
        painter.drawPath(path);
    }
    
    // 绘制大圆圈
    painter.setPen(QPen(Qt::gray, 0.5));
    for (const auto& circle : m_greatCircles) {
        QPainterPath path;
        bool first = true;
        for (const auto& point : circle) {
            QPointF widgetPoint = mapToWidget(point);
            if (first) {
                path.moveTo(widgetPoint);
                first = false;
            } else {
                path.lineTo(widgetPoint);
            }
        }
        painter.drawPath(path);
    }
    
    // 绘制平面的大圆弧
    painter.setPen(QPen(Qt::red, 1.5));
    for (const auto& plane : m_planes) {
        QVector<QPointF> greatCirclePath = m_stereonet.greatCircle(plane, m_projectionType);
        QPainterPath path;
        bool first = true;
        for (const auto& point : greatCirclePath) {
            QPointF widgetPoint = mapToWidget(point);
            if (first) {
                path.moveTo(widgetPoint);
                first = false;
            } else {
                path.lineTo(widgetPoint);
            }
        }
        painter.drawPath(path);
        
        // 绘制极点
        Line pole = m_stereonet.poleFromPlane(plane);
        QPointF polePoint = m_stereonet.stCoordLine(pole.trend, pole.plunge, m_projectionType);
        QPointF widgetPolePoint = mapToWidget(polePoint);
        painter.setBrush(QBrush(Qt::blue));
        painter.drawEllipse(widgetPolePoint, 3, 3);
        painter.setBrush(Qt::NoBrush);
    }
    
    // 绘制方向标记
    painter.setPen(QPen(Qt::black, 1));
    painter.drawText(mapToWidget(QPointF(0, 1)) + QPointF(0, -10), "N");
    painter.drawText(mapToWidget(QPointF(1, 0)) + QPointF(10, 0), "E");
    painter.drawText(mapToWidget(QPointF(0, -1)) + QPointF(0, 10), "S");
    painter.drawText(mapToWidget(QPointF(-1, 0)) + QPointF(-10, 0), "W");
}

void StereonetWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    
    // 更新半径
    m_radius = qMin(width(), height()) / 2.0 - 20;
    computeGrid();
}

void StereonetWidget::computeGrid()
{
    // 计算立体网网格，间隔为10度
    double interval = M_PI / 18.0; // 10度
    m_stereonet.computeStereonetGrid(interval, m_projectionType, m_greatCircles, m_smallCircles);
}

QPointF StereonetWidget::mapToWidget(const QPointF& point) const
{
    QPoint center = rect().center();
    return QPointF(center.x() + point.x() * m_radius, center.y() - point.y() * m_radius);
}

double StereonetWidget::widgetToStereonet(double value) const
{
    return value / m_radius;
}
