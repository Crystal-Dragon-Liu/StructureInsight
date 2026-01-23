#include "stereonet.h"
#include <cmath>
#include <QVector>

Stereonet::Stereonet()
{
}

QPointF Stereonet::stCoordLine(double trend, double plunge, StereonetType type) const
{
    // 处理负倾角
    if (plunge < 0) {
        trend = zeroTwoPi(trend + PI);
        plunge *= -1.0;
    }
    
    double x = 0.0;
    
    // Equal angle stereonet
    if (type == StereonetType::EqualAngle) {
        x = tan(PI/4 - plunge/2);
    }
    // Equal area stereonet
    else if (type == StereonetType::EqualArea) {
        x = sqrt(2) * sin(PI/4 - plunge/2);
    }
    
    // 计算坐标
    double xp = x * sin(trend);
    double yp = x * cos(trend);
    
    return QPointF(xp, yp);
}

Line Stereonet::poleFromPlane(const Plane& plane) const
{
    Line pole;
    pole.trend = zeroTwoPi(plane.strike - EAST);
    pole.plunge = EAST - plane.dip;
    return pole;
}

Plane Stereonet::planeFromPole(const Line& pole) const
{
    Plane plane;
    Line adjustedPole = pole;
    
    // 处理向上的极点
    if (adjustedPole.plunge < 0.0) {
        adjustedPole.trend += PI;
        adjustedPole.plunge *= -1.0;
    }
    
    plane.strike = zeroTwoPi(adjustedPole.trend + EAST);
    plane.dip = EAST - adjustedPole.plunge;
    
    return plane;
}

QVector<QPointF> Stereonet::greatCircle(const Plane& plane, StereonetType type) const
{
    QVector<QPointF> path;
    
    // 计算平面的极点
    Line pole = poleFromPlane(plane);
    
    // 选择与立体网原始交点的走向线
    double trd = plane.strike;
    double plg = 0.0;
    
    // 旋转180度，步长1度
    for (int i = 0; i <= 180; ++i) {
        double rot = static_cast<double>(i) * PI / 180.0;
        
        // 避免连接路径的两端
        if (rot == PI) {
            rot *= 0.9999;
        }
        
        // 使用完整的旋转算法，将线视为向量
        double rtrd, rplg;
        rotate(pole.trend, pole.plunge, rot, trd, plg, false, rtrd, rplg);
        
        // 计算立体网坐标
        QPointF point = stCoordLine(rtrd, rplg, type);
        path.append(point);
    }
    
    return path;
}

void Stereonet::computeStereonetGrid(double interval, StereonetType type, 
                                     QVector<QVector<QPointF>>& greatCircles,
                                     QVector<QVector<QPointF>>& smallCircles) const
{
    greatCircles.clear();
    smallCircles.clear();
    
    // 计算小圆圈数量
    int ncircles = static_cast<int>(PI / (interval * 2.0));
    double newInterval = PI / (ncircles * 2.0);
    
    // 绘制小圆圈，从北极开始
    Line axis;
    axis.trend = 0.0;
    axis.plunge = 0.0;
    
    for (int i = 1; i <= ncircles; ++i) {
        double cangle = i * newInterval;
        QVector<QPointF> circle = smallCircle(axis, cangle, type);
        if (!circle.isEmpty()) {
            smallCircles.append(circle);
        }
    }
    
    // 绘制大圆圈
    for (int i = 0; i <= ncircles * 2; ++i) {
        Line pole;
        
        // 西半部分
        if (i <= ncircles) {
            pole.trend = WEST;
            pole.plunge = i * newInterval;
        }
        // 东半部分
        else {
            pole.trend = EAST;
            pole.plunge = (i - ncircles) * newInterval;
        }
        
        // 如果极点垂直，稍微调整
        if (pole.plunge == EAST) {
            pole.plunge *= 0.9999;
        }
        
        // 从极点计算平面
        Plane plane = planeFromPole(pole);
        
        // 计算大圆弧路径
        QVector<QPointF> circle = greatCircle(plane, type);
        if (!circle.isEmpty()) {
            greatCircles.append(circle);
        }
    }
}

QVector<QPointF> Stereonet::smallCircle(const Line& axis, double angle, StereonetType type) const
{
    QVector<QPointF> path;
    
    // 找到小圆圈的起点
    double trd, plg;
    if ((axis.plunge - angle) >= 0.0) {
        trd = axis.trend;
        plg = axis.plunge - angle;
    } else {
        double adjustedPlga = axis.plunge;
        if (adjustedPlga == PI/2.0) {
            adjustedPlga *= 0.9999;
        }
        double angleRad = acos(cos(angle) / cos(adjustedPlga));
        trd = zeroTwoPi(axis.trend + angleRad);
        plg = 0.0;
    }
    
    // 旋转360度，步长1度
    for (int i = 0; i <= 360; ++i) {
        double rot = static_cast<double>(i) * PI / 180.0;
        
        // 使用完整的旋转算法，注意这里我们旋转的是向量
        double rtrd, rplg;
        rotate(axis.trend, axis.plunge, rot, trd, plg, false, rtrd, rplg);
        
        // 只添加下半球的点（倾角为正）
        if (rplg >= 0.0) {
            // 计算立体网坐标
            QPointF point = stCoordLine(rtrd, rplg, type);
            path.append(point);
        }
    }
    
    return path;
}

double Stereonet::zeroTwoPi(double angle) const
{
    while (angle < 0) {
        angle += 2 * PI;
    }
    while (angle >= 2 * PI) {
        angle -= 2 * PI;
    }
    return angle;
}

void Stereonet::sphToCart(double trend, double plunge, double& x, double& y, double& z) const
{
    double azimuth = trend;
    double inclination = PI/2.0 - plunge;
    
    x = sin(inclination) * cos(azimuth);
    y = sin(inclination) * sin(azimuth);
    z = cos(inclination); // 上半球投影，z坐标为正
}

void Stereonet::cartToSph(double x, double y, double z, double& trend, double& plunge) const
{
    double azimuth, inclination;
    
    if (x == 0.0 && y == 0.0) {
        azimuth = 0.0;
    } else {
        azimuth = atan2(y, x);
    }
    
    double r = sqrt(x*x + y*y + z*z);
    if (r == 0.0) {
        trend = 0.0;
        plunge = 0.0;
        return;
    }
    
    inclination = acos(z / r); // 考虑到z坐标为正
    
    trend = zeroTwoPi(azimuth);
    plunge = PI/2.0 - inclination;
}

void Stereonet::rotate(double trda, double plga, double rot, double trd, double plg, bool isAxis, 
                       double& rtrd, double& rplg) const
{
    // 转换旋转轴到方向余弦
    double raxis[3];
    sphToCart(trda, plga, raxis[0], raxis[1], raxis[2]);
    
    // 计算旋转矩阵
    double a[3][3];
    double x = 1.0 - cos(rot);
    double sinrot = sin(rot);
    double cosrot = cos(rot);
    
    a[0][0] = cosrot + raxis[0]*raxis[0]*x;
    a[0][1] = -raxis[2]*sinrot + raxis[0]*raxis[1]*x;
    a[0][2] = raxis[1]*sinrot + raxis[0]*raxis[2]*x;
    a[1][0] = raxis[2]*sinrot + raxis[1]*raxis[0]*x;
    a[1][1] = cosrot + raxis[1]*raxis[1]*x;
    a[1][2] = -raxis[0]*sinrot + raxis[1]*raxis[2]*x;
    a[2][0] = -raxis[1]*sinrot + raxis[2]*raxis[0]*x;
    a[2][1] = raxis[0]*sinrot + raxis[2]*raxis[1]*x;
    a[2][2] = cosrot + raxis[2]*raxis[2]*x;
    
    // 转换要旋转的线到方向余弦
    double line[3];
    sphToCart(trd, plg, line[0], line[1], line[2]);
    
    // 执行坐标变换
    double liner[3] = {0.0, 0.0, 0.0};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            liner[i] += a[i][j] * line[j];
        }
    }
    
    // 确保旋转后的线是单位向量
    double norm = sqrt(liner[0]*liner[0] + liner[1]*liner[1] + liner[2]*liner[2]);
    for (int i = 0; i < 3; ++i) {
        liner[i] /= norm;
    }
    
    // 如果是轴，确保它在参考半球（我们使用上半球，所以z应该为正）
    if (liner[2] < 0.0 && isAxis) {
        for (int i = 0; i < 3; ++i) {
            liner[i] *= -1.0;
        }
    }
    
    // 从方向余弦转换回走向和倾角
    cartToSph(liner[0], liner[1], liner[2], rtrd, rplg);
}
