#ifndef DIPWIDGETBASE_H
#define DIPWIDGETBASE_H

#include <QWidget>
#include "propertygroup.h"


class PropertyGroup;
class QPainter;

class DipWidgetBase : public QWidget
{
    Q_OBJECT
public:
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
    void drawInfo(QPainter* painter, const QRect& rect);
    /*@brief 注册监控变量*/
    void registerMonitorInfo(const QString& name, void* monitorData);

public slots:
    void onUpdateSelfPaint();


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

protected:
    // 通用属性创建函数
    template<typename PropType, typename... Args>
    void createProperty(Args&&... args)
    {
        PropType* item = new PropType(std::forward<Args>(args)..., m_propertyGroup);
        connect(item, &PropType::valueChangedNoArgs, this, &DipWidgetBase::onUpdateSelfPaint);
        m_propertyGroup->addProperty(item);
    }

};

#endif // DIPWIDGETBASE_H
