#include "dipwidgetbase.h"
#include "floatpropertyitem.h"
#include <QPainter>
#include <QPoint>

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
    FloatPropertyItem* infoFontSize  = new FloatPropertyItem(INFO_FONT_SIZE, 0, 20, 12, m_propertyGroup);
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

void DipWidgetBase::debug_drawRect(
    QPainter* painter,
    const QColor& color,
    const QRect& rect,
    const QString& title){
    painter->save();

    QPen pen;
    pen.setColor(color);            // 自定义颜色
    pen.setWidth(1);                // 线宽
    pen.setStyle(Qt::DashLine);     // 虚线（自带几种可选
    painter->setPen(pen);
    painter->drawRect(rect);
    if(title != ""){
        painter->drawText(rect.left() + 10, rect.top() +10 + TEXT_HEIGHT_OFFSET, title);
    }
    painter->restore();
}

void DipWidgetBase::drawCanvas(QPainter* painter, const QRect& rect){

    // 保存painter后续再恢复
    painter->save();
    int shadowWidth = 5;   // 阴影深度（越大越立体）
    // 绘制边框
    QColor mainColor = Qt::white;   // 主体背景色
    QColor shadowColor(160, 160, 160, 200);

    // 绘制阴影(右下)
    QRect shadowRight(rect.right() + 1, rect.top() + shadowWidth, shadowWidth, rect.height());
    painter->fillRect(shadowRight, shadowColor);
    QRect shadowBottom(rect.left() + shadowWidth, rect.bottom() + 1, rect.width(), shadowWidth);
    painter->fillRect(shadowBottom, shadowColor);

    QPen borderPen(Qt::black);
    borderPen.setWidth(1);
    painter->setPen(borderPen);
    painter->setBrush(mainColor);
    painter->drawRect(rect);
    painter->restore();
}

void DipWidgetBase::drawTypeLabel(
    QPainter* painter,
    const QRect& rect,
    const QStringList& labelList){
    painter->save();
    // TODO 绘制类型label
    QList<QRect> returned;
    // 查看有多少种产状类型被加载
    QStringList keys = m_buffer.keys();
    // 找到最长的字符宽度
    float fontSize = getInfoFontSize();
    auto font = QFont("Arial", fontSize);
    QFontMetricsF metrics(font);
    // label文本和color之间的空隙
    int space = 5;

    // 每个type label(包含label + space + color) 之间的space
    int spaceBetweenLabel = TEXT_HEIGHT_OFFSET * 2;
    // label的高度
    int keyMaxHeight = metrics.height();
    // label的最大宽度
    qreal keyMaxWidth = -99999.0;
    foreach(auto key, keys){
        qDebug() << key;
        qreal keyWidth = metrics.boundingRect(key).width();
        keyWidth = std::max(keyMaxWidth, ceil(keyWidth));
        // keyWidth = metrics.horizontalAdvance(key);
        if(keyWidth > keyMaxWidth){
            keyMaxWidth = keyWidth;
        }
    }
    keyMaxWidth *= 1.5;
    // label颜色标记的宽度
    int labelColorRectSize = keyMaxHeight;

    // 单个label所占区域的宽度
    int labelRectWidth = keyMaxWidth + labelColorRectSize + space;
    int labelStartPosX = 0;
    int curRowIdx = 0;
    for(int i = 0; i < keys.size(); i++){
        // col之间的space
        int tmpSBLCol = spaceBetweenLabel;
        // row之间的space
        int tmpSBLRow = spaceBetweenLabel;
        if((rect.left() + (1+ labelStartPosX) * (labelRectWidth + spaceBetweenLabel)) > rect.right()){
            labelStartPosX = 0;
            curRowIdx++;
            tmpSBLCol = 0;
        }
        if(i == 0){
            tmpSBLCol = 0;
            // tmpSBLRow = 0;
        }
        int currentLeft = rect.left() + labelStartPosX * (labelRectWidth + tmpSBLCol);
        int currentTopBase = rect.top() + curRowIdx * (labelColorRectSize + tmpSBLRow);
        QRect labelRect = QRect(
            currentLeft,
            currentTopBase,
            labelRectWidth, labelColorRectSize);
        QRect colorLabelRect = QRect(
            currentLeft,
            currentTopBase,
            labelColorRectSize, labelColorRectSize
            );
        QRect textLabelRect = QRect(
            currentLeft + labelColorRectSize + space,
            rect.top() + curRowIdx * labelColorRectSize, keyMaxWidth, labelColorRectSize
        );
        painter->save();
        painter->setBrush(m_buffer[keys[i]].color);
        painter->drawRect(colorLabelRect);
        painter->restore();
        painter->drawText(textLabelRect.left(), textLabelRect.bottom(), keys[0]);

        labelStartPosX++;
        returned.append(labelRect);

    }
    painter->restore();
}

void DipWidgetBase::drawAbstractInfo(QPainter* painter, const QRect& rect, const QStringList& infoItemList){
    // TODO 把这个代码移到base去
    painter->save();
    // 绘制在中间
    float fontSize = getInfoFontSize();
    painter->setFont(QFont("Arial", fontSize));
    QFontMetrics metrics(painter->font());
    int infoRectWidth = -99999;
    foreach(auto info, infoItemList){
        auto tmpWidth = metrics.boundingRect(info).width();
        if(tmpWidth > infoRectWidth){
            infoRectWidth = tmpWidth;
        }
    }

    int infoMargin = 25;
    for(int i = 0; i < infoItemList.size(); i++){
        QPoint textPos(
            rect.center().x() - metrics.horizontalAdvance(infoItemList[i]) / 2,
            rect.top() + (i+1) * metrics.height() - TEXT_HEIGHT_OFFSET
        );
        painter->drawText(textPos, infoItemList[i]);
    }
    painter->restore();
}
