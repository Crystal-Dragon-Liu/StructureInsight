#ifndef STEREONETWIDGET_H
#define STEREONETWIDGET_H

#include <QWidget>
#include <QVector>
#include "stereonet.h"

class StereonetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StereonetWidget(QWidget *parent = nullptr);
    
    void setProjectionType(StereonetType type);
    void addPlane(const Plane& plane);
    void clearPlanes();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    // 鼠标事件声明
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void computeGrid();
    QPointF mapToWidget(const QPointF& point) const;
    double widgetToStereonet(double value) const;

    StereonetType m_projectionType;
    Stereonet m_stereonet;
    QVector<Plane> m_planes;
    QVector<QVector<QPointF>> m_greatCircles;
    QVector<QVector<QPointF>> m_smallCircles;
    double m_radius;

    // 旋转相关变量
    double m_rotY = 0.0;                  // 绕南北轴的旋转角度（弧度）
    QPoint m_lastMousePos;                // 记录鼠标按下位置
    QVector<QVector<QPointF>> m_originalGreatCircles; // 保存原始大圆（避免旋转污染）
    QVector<QVector<QPointF>> m_originalSmallCircles; // 保存原始小圆（避免旋转污染）
    QVector<Line> m_originalGreatCirclePoles; // 保存生成每个大圆的原始极点
};

#endif // STEREONETWIDGET_H
