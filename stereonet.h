#ifndef STEREONET_H
#define STEREONET_H

#include <QPointF>
#include <QVector>
#include <cmath>

enum class StereonetType {
    EqualAngle = 0,
    EqualArea = 1
};

struct Plane {
    double strike; // 走向，弧度
    double dip;    // 倾角，弧度
};

struct Line {
    double trend;  // 倾向，弧度
    double plunge; // 倾角，弧度
};

class Stereonet {
public:
    Stereonet();
    
    // 计算直线在立体网上的坐标
    QPointF stCoordLine(double trend, double plunge, StereonetType type) const;
    
    // 从平面计算极点
    Line poleFromPlane(const Plane& plane) const;
    
    // 从极点计算平面
    Plane planeFromPole(const Line& pole) const;
    
    // 计算大圆弧路径
    QVector<QPointF> greatCircle(const Plane& plane, StereonetType type) const;
    
    // 计算立体网网格
    void computeStereonetGrid(double interval, StereonetType type, 
                             QVector<QVector<QPointF>>& greatCircles,
                             QVector<QVector<QPointF>>& smallCircles) const;
    
    // 计算小圆弧路径
    QVector<QPointF> smallCircle(Line& axis, double angle, StereonetType type) const;

    // 计算上下圆弧路径
    std::pair<QVector<QPointF>, QVector<QPointF>> smallCircleWithDoubleDirection(Line& axis, double angle, StereonetType type) const;
    
private:
    // 角度处理
    double zeroTwoPi(double angle) const;
    
    // 球坐标到笛卡尔坐标转换
    void sphToCart(double trend, double plunge, double& x, double& y, double& z) const;
    
    // 笛卡尔坐标到球坐标转换
    void cartToSph(double x, double y, double z, double& trend, double& plunge) const;
    
    // 旋转函数
    void rotate(double trda, double plga, double rot, double trd, double plg, bool isAxis, 
                double& rtrd, double& rplg) const;
    
    static constexpr double PI = M_PI;
    static constexpr double EAST = M_PI / 2.0;
    static constexpr double WEST = 3.0 * M_PI / 2.0;
};

#endif // STEREONET_H
