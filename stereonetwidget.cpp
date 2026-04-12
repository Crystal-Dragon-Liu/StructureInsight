#include "stereonetwidget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPainterPath>
#include <QMouseEvent>
#include <QLineF>
#include <QtMath>
#include "propertygroup.h"
#include "floatpropertyitem.h"
#include "singleselectpropertyitem.h"
const QString& STEREONET_WIDGET_NAME = QObject::tr("Stereonet");


StereonetWidget::StereonetWidget(QWidget *parent)
    : DipWidgetBase(parent, STEREONET_WIDGET_NAME),
    m_projectionType(StereonetType::EqualArea),
    m_rotX(0.0),
    m_rotY(0.0)
{
    // 设置大小策略为可扩展，这样会填充整个可用空间
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // 初始计算半径
    m_radius = qMin(width(), height()) / 2.0 - 20;
    if (m_radius < 50) m_radius = 50; // 设置最小半径
    
    computeGrid();
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

    QPoint center = rect().center();
    painter.drawRect(rect());

    // 绘制参考圆
    painter.setPen(QPen(Qt::black, 1));
    painter.drawEllipse(center, static_cast<int>(m_radius), static_cast<int>(m_radius));

    // 1. 绘制小圆
    painter.setPen(QPen(Qt::gray, 0.5));
    const double DIST_THRESHOLD = 0.05 * m_radius;
    for (const auto& circle : m_originalSmallCircles) {
        QPainterPath path;
        bool first = true;
        QPointF lastWp;

        for (const auto& p : circle) {
            // 反推原始球坐标
            double x = p.x();
            double y = p.y();
            double x_proj = sqrt(x*x + y*y);
            if (x_proj < 0.01) {
                first = true;
                lastWp = QPointF();
                continue;
            }
            double trend = atan2(x, y);
            double plunge = 0.0;
            if (m_projectionType == StereonetType::EqualArea) {
                plunge = M_PI/2.0 - 2 * asin(x_proj / sqrt(2));
            } else {
                plunge = M_PI/2.0 - 2 * atan(x_proj);
            }
            plunge = qMax(0.0, plunge);

            // 全向旋转：先绕南北轴（m_rotY），再绕东西轴（m_rotX）
            double rotatedTrend, rotatedPlunge;
            // 第一步：绕南北轴旋转（东西转）
            m_stereonet.rotate(0.0, 0.0, m_rotY, trend, plunge, false, rotatedTrend, rotatedPlunge);
            // 第二步：绕东西轴旋转（南北转）
            m_stereonet.rotate(Stereonet::EAST, 0.0, m_rotX, rotatedTrend, rotatedPlunge, false, rotatedTrend, rotatedPlunge);

            // 转回平面坐标+绘制
            QPointF rotatedP = m_stereonet.stCoordLine(rotatedTrend, rotatedPlunge, m_projectionType);
            QPointF wp = mapToWidget(rotatedP);

            if (first) {
                path.moveTo(wp);
                first = false;
                lastWp = wp;
            } else {
                double dist = QLineF(lastWp, wp).length();
                if (dist < DIST_THRESHOLD) {
                    path.lineTo(wp);
                    lastWp = wp;
                } else {
                    first = true;
                    lastWp = QPointF();
                }
            }
        }
        painter.drawPath(path);
    }

    // 2. 绘制大圆（经线，同步全向旋转）
    painter.setPen(QPen(Qt::gray, 0.5));
    for (int i = 0; i < m_originalGreatCirclePoles.size(); ++i) {
        Line originalPole = m_originalGreatCirclePoles[i];

        // 全向旋转极点：先绕南北轴，再绕东西轴
        double rotatedTrend, rotatedPlunge;
        // 第一步：绕南北轴旋转（东西转）
        m_stereonet.rotate(0.0, 0.0, m_rotY, originalPole.trend, originalPole.plunge, false, rotatedTrend, rotatedPlunge);
        // 第二步：绕东西轴旋转（南北转）
        m_stereonet.rotate(Stereonet::EAST, 0.0, m_rotX, rotatedTrend, rotatedPlunge, false, rotatedTrend, rotatedPlunge);

        // 生成旋转后的大圆
        Line rotatedPole;
        rotatedPole.trend = rotatedTrend;
        rotatedPole.plunge = rotatedPlunge;
        Plane rotatedPlane = m_stereonet.planeFromPole(rotatedPole);
        QVector<QPointF> circle = m_stereonet.greatCircle(rotatedPlane, m_projectionType);

        // 绘制大圆（过滤收敛点，避免射线）
        QPainterPath painterPath;
        bool first = true;
        QPointF lastWp;
        for (const auto& p : circle) {
            QPointF wp = mapToWidget(p);
            if (first) {
                painterPath.moveTo(wp);
                first = false;
                lastWp = wp;
            } else {
                double dist = QLineF(lastWp, wp).length();
                if (dist < DIST_THRESHOLD) {
                    painterPath.lineTo(wp);
                    lastWp = wp;
                } else {
                    first = true;
                    lastWp = QPointF();
                }
            }
        }
        painter.drawPath(painterPath);
    }

    // 3. 绘制平面
    painter.setPen(QPen(Qt::red, 1.5));
    for (const auto& plane : m_planes) {
        // 获取平面原始极点
        Line planePole = m_stereonet.poleFromPlane(plane);

        // 全向旋转极点
        double rotatedTrend, rotatedPlunge;
        m_stereonet.rotate(0.0, 0.0, m_rotY, planePole.trend, planePole.plunge, false, rotatedTrend, rotatedPlunge);
        m_stereonet.rotate(Stereonet::EAST, 0.0, m_rotX, rotatedTrend, rotatedPlunge, false, rotatedTrend, rotatedPlunge);

        // 生成旋转后的平面大圆弧
        Line rotatedPole;
        rotatedPole.trend = rotatedTrend;
        rotatedPole.plunge = rotatedPlunge;
        Plane rotatedPlane = m_stereonet.planeFromPole(rotatedPole);
        QVector<QPointF> rotatedGreatCircle = m_stereonet.greatCircle(rotatedPlane, m_projectionType);

        // 绘制平面大圆弧
        QPainterPath path;
        bool first = true;
        for (const auto& p : rotatedGreatCircle) {
            QPointF wp = mapToWidget(p);
            if (first) { path.moveTo(wp); first = false; }
            else path.lineTo(wp);
        }
        painter.drawPath(path);

        // 绘制平面极点
        QPointF polePoint = m_stereonet.stCoordLine(rotatedTrend, rotatedPlunge, m_projectionType);
        QPointF widgetPolePoint = mapToWidget(polePoint);
        painter.setBrush(QBrush(Qt::blue));
        painter.drawEllipse(widgetPolePoint, 3, 3);
        painter.setBrush(Qt::NoBrush);
    }

    // 方向标记
    painter.setPen(QPen(Qt::black, 1));


    // drawInfo(&painter, rect());
}

void StereonetWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    // 计算新的半径，取宽度和高度的最小值的一半，并留出边距
    m_radius = qMin(width(), height()) / 2.0 - 20;
    // 设置最小半径，确保在小窗口中也能显示
    if (m_radius < 50) m_radius = 50;
    // 重新计算网格并更新显示
    computeGrid();
    update();
}

// 生成网格
void StereonetWidget::computeGrid()
{
    double interval = M_PI / 18.0; // 10度
    // 调用Stereonet生成网格
    m_stereonet.computeStereonetGrid(interval, m_projectionType, m_greatCircles, m_smallCircles);

    // 保存原始小圆
    m_originalSmallCircles = m_smallCircles;

    // 重新生成并保存原始大圆极点
    m_originalGreatCirclePoles.clear();
    int ncircles = static_cast<int>(M_PI / (interval * 2.0));
    double newInterval = M_PI / (ncircles * 2.0);
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

    // 保存原始大圆
    m_originalGreatCircles = m_greatCircles;
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

void StereonetWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_lastMousePos = event->pos();
    }
}

// 全向旋转鼠标事件
void StereonetWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int dx = event->pos().x() - m_lastMousePos.x(); // 左右拖动=东西转
        int dy = event->pos().y() - m_lastMousePos.y(); // 上下拖动=南北转

        // 更新旋转角度
        m_rotY -= dx * 0.003; // 绕南北轴（东西转）
        m_rotX += dy * 0.003; // 绕东西轴（南北转）

        // 角度限制
        // m_rotY = m_stereonet.zeroTwoPi(m_rotY);
        // m_rotX = qBound(-1.471, m_rotX, 1.471);
        m_lastMousePos = event->pos();
        update();
    }
}

void StereonetWidget::initProperties(){
    DipWidgetBase::initProperties();

    // Stereonet 相关属性
    if(!m_propertyGroup){
        m_propertyGroup = new PropertyGroup(STEREONET_WIDGET_NAME, this);
    }

    // Stereonet相关属性
    // 初始南北偏移角度
    FloatPropertyItem* initialXRotation  = new FloatPropertyItem(tr("Initial X Rotation"), 0.0, 360.0, 90.0, m_propertyGroup);
    m_propertyGroup->addProperty(initialXRotation);
    // 初始东西偏移角度
    FloatPropertyItem* initialYRotation  = new FloatPropertyItem(tr("Initial Y Rotation"), 0.0, 360.0, 0.0, m_propertyGroup);
    m_propertyGroup->addProperty(initialYRotation);
    // 是否显示平面
    SingleSelectPropertyItem* showPlane = new SingleSelectPropertyItem(tr("Show Plane"), {tr("Yes"), tr("No")}, 0, m_propertyGroup);
    m_propertyGroup->addProperty(showPlane);
    // Stereonet的投影类型
    SingleSelectPropertyItem* projectionType = new SingleSelectPropertyItem(tr("Projection Type"), {tr("Equal-Area"), tr("Equal-Angle")}, 0, m_propertyGroup);
    m_propertyGroup->addProperty(projectionType);

}

void StereonetWidget::onUpdateWithPropertyChanged(const QString& propertyName, const QVariant& value){
    Q_UNUSED(propertyName)
    Q_UNUSED(value);
    // TODO 处理属性更改时的信号
    return;
}
