#include "dipwidgetbase.h"
#include "propertygroup.h"
#include "floatpropertyitem.h"
#include <QPainter>

const QString& INFO_FONT_SIZE = QObject::tr("Info Font Size");

DipWidgetBase::DipWidgetBase(QWidget *parent, const QString& widgetName)
    : QWidget{parent},
    m_dataNum(-1),
    m_topDepth(-99999.0f),
    m_bottomDepth(-99999.0f),
    m_widgetName(widgetName),
    m_propertyGroup(nullptr)
{}


void DipWidgetBase::initProperties(){
    if(!m_propertyGroup){
        m_propertyGroup = new PropertyGroup(m_widgetName, this);
    }

    // Info大小
    FloatPropertyItem* infoFontSize  = new FloatPropertyItem(INFO_FONT_SIZE, 0, 20, 14, m_propertyGroup);
    connect(infoFontSize, SIGNAL(valueChangedNoArgs()), this, SLOT(onUpdateSelfPaint()));

    m_propertyGroup->addProperty(infoFontSize);
}


float DipWidgetBase::getInfoFontSize() const{
    if(m_propertyGroup){
        PropertyItem* property = m_propertyGroup->property(INFO_FONT_SIZE);
        return property->value().toFloat();
    }
    else{
        return 9.0f;
    }
}


void DipWidgetBase::onUpdateSelfPaint(){
    update();
}

void DipWidgetBase::registerMonitorInfo(const QString& name, void* monitorData){
    m_infoMap.insert(std::make_pair(name, monitorData));
}

void DipWidgetBase::drawInfo(QPainter* painter, const QRect& rect){
    // TODO 把这个代码移到base去
    painter->save();
    // Rect为画布整体大小
    QString topDepthInfo = QString(tr("Top Depth: %1").arg(getTopDepth()));
    QString bottomDepthInfo = QString(tr("Bottom Depth: %1").arg(getBottomDepth()));
    QString dataNumInfo = QString(tr("Data counts: %1").arg(getDataNum()));
    QStringList infoItemList = {topDepthInfo, bottomDepthInfo, dataNumInfo};

    float fontSize = getInfoFontSize();
    painter->setFont(QFont("Arial", fontSize));
    QFontMetrics metrics(painter->font());
    int infoRectWidth = metrics.boundingRect(topDepthInfo).width();
    foreach(auto info, infoItemList){
        auto tmpWidth = metrics.boundingRect(info).width();
        if(tmpWidth > infoRectWidth){
            infoRectWidth = tmpWidth;
        }
    }
    int infoMargin = 25;
    int infoRectHeight = metrics.height() * infoItemList.size();
    // 计算面板信息需要的空间
    QRect infoRect(rect.left() + infoMargin,
                   rect.top() + infoMargin,
                   infoRectWidth,
                   infoRectHeight);
    // painter->drawRect(infoRect);

    for(int i = 0; i < infoItemList.size(); i++){
        painter->drawText(infoRect.left(),
                          infoRect.top() + metrics.height() * (i+1),
                          infoItemList[i]);
    }
    painter->restore();
}
