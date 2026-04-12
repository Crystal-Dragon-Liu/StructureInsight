#ifndef DIPWIDGETBASE_H
#define DIPWIDGETBASE_H

#include <QWidget>
#include "propertygroup.h"
#include "dipdataaccess.h"


class PropertyGroup;
class QPainter;

////////////////////////////////  DEBUG 用颜色   ////////////////////////////////
/// \brief CANVAS_RECT_COLOR
/////////////////////////////////////////////////////////////////////////////////
#define DEBUG_CANVAS_RECT_COLOR     QColor(0, 255, 0, 100)      // 绿色半透明, 用于标记绘图中widget的Rect
#define DEBUG_PLOT_RECT_COLOR       QColor(100, 135, 165, 100)  // 灰色半透明，用于标记绘图中实际绘制区域
#define DEBUG_INFO_RECT_COLOR       QColor(42,  45,  50, 100)   // 深色半透明，用于标记绘图中信息绘制区域
#define TEXT_HEIGHT_OFFSET          2

class DipWidgetBase : public QWidget
{
    Q_OBJECT
public:
    struct AzimuthStatistic{
        QList<DipData> dips;
        QVector<float> hist;
        QColor color;
    };
    explicit DipWidgetBase(QWidget *parent = nullptr, const QString& widgetName = "Dip Plot");

    /*@brief 初始化属性*/
    virtual void initProperties();

    /*@brief 获得信息面板字体大小*/
    float getInfoFontSize() const;

    /*@brief 顶深*/
    float getTopDepth() const {return m_topDepth;}
    /*@brief 底深*/
    float getBottomDepth() const {return m_bottomDepth;}
    /*@brief 数据长度*/
    int getDataNum() const {return m_dataNum;}

    void setTopDepth(float data) {m_topDepth = data;}
    /*@brief 底深*/
    void setBottomDepth(float data) {m_bottomDepth = data;}
    /*@brief 数据长度*/
    void setDataNum(float data) {m_dataNum = data;}

protected:
    /*@brief 绘制监控变量*/
    void drawAbstractInfo(QPainter* painter, const QRect& rect, const QStringList& infoList);

    /*@brief 绘制label 有几种类型就绘制几种*/
    void drawTypeLabel(QPainter* painter, const QRect& rect, const QStringList& labelList);

    /*@brief 绘制画布*/
    void drawCanvas(QPainter* painter, const QRect& rect);

    /*@brief 注册监控变量*/
    void registerMonitorInfo(const QString& name, void* monitorData);

    /*@brief 绘制调试边框*/
    void debug_drawRect(QPainter* painter, const QColor& color, const QRect& rect, const QString& title = "");


public slots:
    void onUpdateSelfPaint();

    virtual void onUpdateWithPropertyChanged(const QString& propertyName, const QVariant& value) = 0;

protected:
    // 属性组
    PropertyGroup* m_propertyGroup;

    // 面板数据显示
    float m_topDepth;
    float m_bottomDepth;
    int m_dataNum;

    // TODO 需要一个信息面板管理模块, 使得各个基类可以实时更新到这个map, 后面再说吧
    std::map<QString, void*> m_infoMap;

    // 窗口类型名称, 用于属性适配
    QString m_widgetName;
    // 分组记录不同类型的histogram
    QMap<QString, AzimuthStatistic> m_buffer;

protected:
    // 通用属性创建函数
    template<typename PropType, typename... Args>
    void createProperty(Args&&... args)
    {
        PropType* item = new PropType(std::forward<Args>(args)..., m_propertyGroup);
        connect(item, &PropType::valueChangedNoArgs, this, &DipWidgetBase::onUpdateSelfPaint);
        connect(item, &PropType::valueChangedWithName, this, &DipWidgetBase::onUpdateWithPropertyChanged);
        m_propertyGroup->addProperty(item);
    }

};

#endif // DIPWIDGETBASE_H
