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
};

#endif // STEREONETWIDGET_H