#include "stereonetwidget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPainterPath>
#include <QMouseEvent>

StereonetWidget::StereonetWidget(QWidget *parent)
    : QWidget(parent),
      m_projectionType(StereonetType::EqualArea),
      m_radius(200)
{
    computeGrid();
    // 初始化原始网格
    m_originalGreatCircles = m_greatCircles;
    m_originalSmallCircles = m_smallCircles;
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
    
    // 绘制小圆（纬线，绕南北极轴，旋转时不动）
    painter.setPen(QPen(Qt::gray, 0.5));
    for (const auto& circle : m_originalSmallCircles) {
        QPainterPath path;
        bool first = true;
        for (const auto& p : circle) {
            QPointF wp = mapToWidget(p);
            if (first) { path.moveTo(wp); first = false; }
            else path.lineTo(wp);
        }
        painter.drawPath(path);
    }
    
    // 绘制大圆（经线，先旋转极点，再重新生成）
    painter.setPen(QPen(Qt::gray, 0.5));
    for (int i = 0; i < m_originalGreatCirclePoles.size(); ++i) {
        // 1. 获取原始极点
        Line originalPole = m_originalGreatCirclePoles[i];

        // 2. 绕南北极轴（trend=0, plunge=0）旋转这个极点
        double rotatedTrend, rotatedPlunge;
        m_stereonet.rotate(0.0, 0.0, m_rotY, originalPole.trend, originalPole.plunge, false, rotatedTrend, rotatedPlunge);

        // 3. 用旋转后的极点生成新的平面
        Line rotatedPole;
        rotatedPole.trend = rotatedTrend;
        rotatedPole.plunge = rotatedPlunge;
        Plane rotatedPlane = m_stereonet.planeFromPole(rotatedPole);

        // 4. 用新平面重新生成大圆弧
        QVector<QPointF> path = m_stereonet.greatCircle(rotatedPlane, m_projectionType);

        // 5. 绘制这条经线
        QPainterPath painterPath;
        bool first = true;
        for (const auto& p : path) {
            QPointF wp = mapToWidget(p);
            if (first) { painterPath.moveTo(wp); first = false; }
            else painterPath.lineTo(wp);
        }
        painter.drawPath(painterPath);
    }
    
    // 绘制平面的大圆弧
    painter.setPen(QPen(Qt::red, 1.5));
    for (const auto& plane : m_planes) {
        // 步骤1：获取平面的原始极点（核心：和网格旋转逻辑对齐）
        Line planePole = m_stereonet.poleFromPlane(plane);

        // 步骤2：绕南北极轴旋转这个极点（和网格用同一个旋转角度m_rotY）
        double rotatedTrend, rotatedPlunge;
        m_stereonet.rotate(0.0, 0.0, m_rotY, planePole.trend, planePole.plunge, false, rotatedTrend, rotatedPlunge);

        // 步骤3：用旋转后的极点生成新平面
        Line rotatedPole;
        rotatedPole.trend = rotatedTrend;
        rotatedPole.plunge = rotatedPlunge;
        Plane rotatedPlane = m_stereonet.planeFromPole(rotatedPole);

        // 步骤4：用新平面生成旋转后的大圆弧（原生规则，无扭曲）
        QVector<QPointF> rotatedGreatCircle = m_stereonet.greatCircle(rotatedPlane, m_projectionType);

        // 步骤5：绘制旋转后的平面大圆弧（无需额外过滤，原生圆弧自动适配基圆）
        QPainterPath path;
        bool first = true;
        for (const auto& p : rotatedGreatCircle) {
            QPointF wp = mapToWidget(p);
            if (first) { path.moveTo(wp); first = false; }
            else path.lineTo(wp);
        }
        painter.drawPath(path);

        // 绘制极点（旋转后的极点，和平面同步）
        QPointF polePoint = m_stereonet.stCoordLine(rotatedTrend, rotatedPlunge, m_projectionType);
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
    double interval = M_PI / 18.0; // 10度
    m_stereonet.computeStereonetGrid(interval, m_projectionType, m_greatCircles, m_smallCircles);

    // 保存原始网格
    m_originalGreatCircles = m_greatCircles;
    m_originalSmallCircles = m_smallCircles;

    // 重新生成并保存原始极点，以匹配computeStereonetGrid的规则
    m_originalGreatCirclePoles.clear();
    int ncircles = static_cast<int>(Stereonet::PI / (interval * 2.0));
    double newInterval = Stereonet::PI / (ncircles * 2.0);
    for (int i = 0; i <= ncircles * 2; ++i) {
        Line pole;
        if (i <= ncircles) {
            pole.trend = Stereonet::WEST;
            pole.plunge = i * newInterval;
        } else {
            pole.trend = Stereonet::EAST;
            pole.plunge = (i - ncircles) * newInterval;
        }
        if (pole.plunge == Stereonet::EAST) {
            pole.plunge *= 0.9999;
        }
        m_originalGreatCirclePoles.append(pole);
    }
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

// 鼠标按下事件：记录初始位置
void StereonetWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_lastMousePos = event->pos();
    }
}

// 鼠标拖动事件：仅处理左右拖动，更新旋转角度
void StereonetWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int dx = event->pos().x() - m_lastMousePos.x();
        m_rotY -= dx * 0.003; // 灵敏度
        m_rotY = m_stereonet.zeroTwoPi(m_rotY);
        m_lastMousePos = event->pos();
        update();
    }
}

