#include "rosewidget.h"
#include <QPainter>
#include <QFontMetrics>
#include "propertygroup.h"
#include "floatpropertyitem.h"
#include "singleselectpropertyitem.h"

// 属性名常量
const QString& PROPERTY_MAX_FREQ        = QObject::tr("Max Frequency");
const QString& PROPERTY_ANGLE_SIZE      = QObject::tr("Angle Size");
const QString& PROPERTY_FREQ_SIZE       = QObject::tr("Freq Size");
const QString& ROSE_WIDGET_NAME         = QObject::tr("Rose Diagram");
const QString& PROPERTY_SHOW_AZI_TYPE   = QObject::tr("Azimuth Type");

const QString& APPARENT_AZIMUTH_NAME    = QObject::tr("Apparent Azimuth");
const QString& TRUE_AZIMUTH_NAME        = QObject::tr("True Azimuth");
const QString& STRIKE_NAME              = QObject::tr("Strikes");
const QString& PROPERTY_SHOW_LABEL      = QObject::tr("Show Label");
const QString& YES                      = QObject::tr("Yes");
const QString& NO                       = QObject::tr("No");

#define PIE_PADDING_RATIO 0.05f

RoseWidget::RoseWidget(QWidget *parent)
    : DipWidgetBase{parent, ROSE_WIDGET_NAME}, m_maxCount(0), m_title(ROSE_WIDGET_NAME){
}

RoseWidget::~RoseWidget(){}

void RoseWidget:: setData(DipDataAccess& data){
    m_buffer.clear();
    QVector<QString> typeList;
    data.getDipClassSet(typeList);
    for(int i = 0; i < typeList.size(); i++){
        AzimuthStatistic statistic;
        data.getDataByType(typeList[i], statistic.dips);
        m_buffer.insert(typeList[i], statistic);
    }
    calculateHistogram();
    update();
}

void RoseWidget::setStrikes(const QVector<int> &strikes)
{
    m_strikes = strikes;
    m_dataNum = strikes.size();
    calculateHistogram();
    update();
}

void RoseWidget::initProperties(){
    DipWidgetBase::initProperties();
    // Rose 相关属性
    if(!m_propertyGroup){
        m_propertyGroup = new PropertyGroup(ROSE_WIDGET_NAME, this);
    }

    // 使用的方位数据类型
    createProperty<SingleSelectPropertyItem>(
        PROPERTY_SHOW_AZI_TYPE,
        QStringList({APPARENT_AZIMUTH_NAME, TRUE_AZIMUTH_NAME, STRIKE_NAME}),
        0
    );

    createProperty<FloatPropertyItem>(
        PROPERTY_MAX_FREQ, 0, 10000, 20.0
    );

    createProperty<FloatPropertyItem>(
        PROPERTY_ANGLE_SIZE, 0, 20, 9
    );

    createProperty<FloatPropertyItem>(
        PROPERTY_FREQ_SIZE, 0, 20, 9
    );

    createProperty<SingleSelectPropertyItem>(
        PROPERTY_SHOW_LABEL, QStringList({YES, NO}), 0
    );

}

float RoseWidget::getMaxFreq() const{
    if(m_propertyGroup){
        PropertyItem* property = m_propertyGroup->property(PROPERTY_MAX_FREQ);
        return property->value().toFloat();
    }
    else{
        return 10.0f;
    }
}

float RoseWidget::getFreqDisplaySize() const{
    if(m_propertyGroup){
        PropertyItem* property = m_propertyGroup->property(PROPERTY_FREQ_SIZE);
        return property->value().toFloat();
    }
    else{
        return 9.0f;
    }
}

bool  RoseWidget::isShowTypeLabel() const{
    if(m_propertyGroup){
        PropertyItem* property = m_propertyGroup->property(PROPERTY_SHOW_LABEL);
        QString showLabel = property->value().toString();
        if(showLabel == YES){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return true;
    }
}

DipDataType RoseWidget::getDipDataType() const{
    if(m_propertyGroup){
        PropertyItem* property = m_propertyGroup->property(PROPERTY_SHOW_AZI_TYPE);
        QString type = property->value().toString();
        if(type.compare(APPARENT_AZIMUTH_NAME) == 0){
            return DipDataType::APP_AZI;
        }
        else if(type.compare(TRUE_AZIMUTH_NAME) == 0){
            return DipDataType::TRUE_AZI;
        }
        else if(type.compare(STRIKE_NAME) == 0){
            return DipDataType::STRIKE;
        }
        else{
            return DipDataType::INVALID;
        }
    }
    return DipDataType::INVALID;
}

float RoseWidget::getAngleValueSize() const{
    if(m_propertyGroup){
        PropertyItem* property = m_propertyGroup->property(PROPERTY_ANGLE_SIZE);
        return property->value().toFloat();
    }
    else{
        return 9.0f;
    }
}

void RoseWidget::onSetStrikes(const QVector<int>& strikes){
    setStrikes(strikes);
    // 设置数据长度
    setDataNum(strikes.size());
}

void RoseWidget::setTitle(const QString &title)
{
    m_title = title;
    update();
}

void RoseWidget::drawRoseDiagram(QPainter *painter, const QRect &rect){
    painter->save();
    int centerX = rect.center().x();
    int centerY = rect.center().y();
    int radius = qMin(rect.width(), rect.height()) / 2;

    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QBrush(QColor(200, 200, 200)));

    // 预留一些空间，与matplotlib的效果类似
    double padding = PIE_PADDING_RATIO; // 5%的额外空间
    int effectiveRadius = radius * (1 - padding);

    // 绘制玫瑰图的每个扇区
    auto iter = m_buffer.begin();
    while(iter != m_buffer.end()){
        for (int i = 0; i < 36; i++) {
            double startAngle = (i * 10) * M_PI / 180;
            double endAngle = ((i + 1) * 10) * M_PI / 180;

            // 计算扇区的半径比例
            double radiusRatio = m_maxCount > 0 ? (double)m_buffer[iter.key()].hist[i] / m_maxCount : 0;
            int sectorRadius = effectiveRadius * radiusRatio;

            // 绘制扇区
            painter->drawPie(centerX - sectorRadius, centerY - sectorRadius,
                             2 * sectorRadius, 2 * sectorRadius,
                             -startAngle * 180 / M_PI * 16,
                             -(endAngle - startAngle) * 180 / M_PI * 16);
        }
        iter++;
    }
    painter->restore();
}

void RoseWidget::drawGrid(QPainter *painter, const QRect &rect){
    painter->save();
    int centerX = rect.center().x();
    int centerY = rect.center().y();
    int radius = qMin(rect.width(), rect.height()) / 2;

    // 在最外面向外扩展一点空间，与matplotlib的效果类似
    double padding = PIE_PADDING_RATIO; // 5%的额外空间
    int extendedRadius = radius * (1 + padding);

    painter->setPen(QPen(Qt::lightGray, 1));

    // 绘制同心圆网格
    if (m_maxCount > 0) {
        // 计算合适的网格数量
        int numCircles = qMin(5, m_maxCount);
        if (numCircles < 2) numCircles = 2; // 至少2个网格
        
        // 确保最外面的圆对应m_maxCount
        for (int i = 1; i <= numCircles; i++) {
            double ratio = (double)i / numCircles;
            int circleRadius = radius * ratio;
            painter->drawEllipse(centerX - circleRadius, centerY - circleRadius,
                                 2 * circleRadius, 2 * circleRadius);
        }
    } else {
        // 没有数据时绘制固定网格
        int numCircles = 5;
        for (int i = 1; i <= numCircles; i++) {
            int circleRadius = radius * i / numCircles;
            painter->drawEllipse(centerX - circleRadius, centerY - circleRadius,
                                 2 * circleRadius, 2 * circleRadius);
        }
    }

    // 绘制最外面的圆（向外扩展）
    painter->setPen(QPen(Qt::black, 1));
    painter->drawEllipse(centerX - extendedRadius, centerY - extendedRadius,
                         2 * extendedRadius, 2 * extendedRadius);

    // 绘制轴线（使用扩展后的半径）
    painter->setPen(QPen(Qt::black, 1));

    // 绘制10度轴线（使用扩展后的半径）
    painter->setPen(QPen(Qt::gray, 1));
    for (int angle = 0; angle < 360; angle += 10) {
        double rad = qDegreesToRadians(angle);
        double x1 = centerX + extendedRadius * cos(rad);
        double y1 = centerY + extendedRadius * sin(rad);
        painter->drawLine(centerX, centerY, x1, y1);
    }
    painter->restore();
}

void RoseWidget::drawLabels(QPainter *painter, const QRect &rect){
    painter->save();
    int centerX = rect.center().x();
    int centerY = rect.center().y();
    int radius = qMin(rect.width(), rect.height()) / 2;
    painter->setPen(QPen(Qt::black, 1));
    painter->setFont(QFont("Arial", 10));

    double padding = 0.1;
    int extendRadius = radius * (1+padding);

    // 绘制刻度标签
    painter->setFont(QFont("Arial", 8));
    if (m_maxCount > 0) {
        int numCircles = qMin(5, m_maxCount);
        if (numCircles < 2) numCircles = 2;

        for (int i = 1; i <= numCircles; i++) {
            double ratio = (double)i / numCircles;
            int value = (int)(m_maxCount * ratio + 0.5); // 四舍五入
            int circleRadius = radius * ratio;
            painter->drawText(centerX + 10, centerY - circleRadius + 4, QString::number(value));
        }
    } else {
        // 没有数据时绘制固定标签
        int numCircles = 5;
        for (int i = 1; i <= numCircles; i++) {
            int circleRadius = radius * i / numCircles;
            painter->drawText(centerX + 10, centerY - circleRadius + 4, QString::number(i));
        }
    }

    // 绘制360度数值标签
    float angleSize = this->getAngleValueSize();
    qDebug() << "Using Angle value size: " << angleSize;
    painter->setFont(QFont("Arial", angleSize));
    for(int angle = 0; angle < 360; angle += 10){
        double rad = qDegreesToRadians(angle);
        int labelX = centerX + (extendRadius) * cos(rad);
        int labelY = centerY + (extendRadius) * sin(rad);
        QFontMetrics metrics(painter->font());
        QString angleText = QString::number(angle);
        int textWidth = metrics.horizontalAdvance(angleText);
        int textHeight = metrics.height();
        painter->drawText(labelX - textWidth / 2, labelY + textHeight / 4, angleText);
    }
    painter->restore();
}

void RoseWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect rect = this->rect();
    int margin = 100;
    // 将绘制区域分割为上下两个区域, 上区域为玫瑰图区域
    QRect diagramRectBoundingRect = rect.adjusted(margin / 2, margin / 2, -margin / 2, -margin / 2);
    QRect diagramRect = rect.adjusted(margin, margin, -margin, -margin);

    // 绘制边框
    painter.drawRect(diagramRect);
    painter.drawRect(diagramRectBoundingRect);
    painter.drawRect(rect);

    if(m_buffer.size() == 0){
        painter.drawText(diagramRect.center(), "No data");
        return;
    }
    qDebug() << "Drawing";
    drawGrid(&painter, diagramRect);
    drawRoseDiagram(&painter, diagramRect);
    drawLabels(&painter, diagramRect);
    drawInfo(&painter, rect);
}

QRect RoseWidget::calculateLabelRegion(const QRect& totalRegion){
    // 查看有多少种产状类型被加载
    QStringList keys = m_buffer.keys();
    // 找到最长的字符宽度
    float fontSize = getInfoFontSize();
    auto font = QFont("Arial", fontSize);
    QFontMetrics metrics(font);
    // label文本和color之间的空隙
    int space = 5;
    // label的高度
    int keyMaxHeight = metrics.height();
    // label的最大宽度
    int keyMaxWidth = -99999.0;
    foreach(auto key, keys){
        int keyWidth = metrics.boundingRect(key).width();
        if(keyWidth > keyMaxWidth){
            keyMaxWidth = keyWidth;
        }
    }
    // label颜色标记的宽度
    int labelColorRectSize = keyMaxHeight;

    // 单个label所占区域的宽度
    int labelRectWidth = keyMaxWidth + labelColorRectSize + space;
    int labelStartPosX = 0;
    int curRowIdx = 0;
    for(int i = 0; i < keys.size(); i++){
        if((totalRegion.left() + labelStartPosX * labelRectWidth) > totalRegion.right()){
            labelStartPosX = 0;
            curRowIdx++;
        }
        QRect labelRect = QRect(
            totalRegion.left() + labelStartPosX * labelRectWidth,
            totalRegion.top() + curRowIdx * labelColorRectSize, labelRectWidth, labelColorRectSize);
        labelStartPosX++;
    }
    return QRect();
}

void RoseWidget::calculateHistogram(AzimuthStatistic& dips, const DipDataType & dataType){
    // 初始化直方图数组，36个区间，每个10度
    dips.hist.resize(36, 0);
    std::fill(dips.hist.begin(), dips.hist.end(), 0);
    m_maxCount = 0;
    // 统计每个区间的数量
    for (DipData dipBuffer : dips.dips) {
        float data = -1;
        // 决定使用哪个数据, APP_AZI/TRUE_AZI/STRIKE
        switch (dataType) {
            case DipDataType::STRIKE:   { data = dipBuffer.strike; break; }
            case DipDataType::APP_AZI:  { data = dipBuffer.appAzi; break; }
            case DipDataType::TRUE_AZI: { data = dipBuffer.trueAzi;break; }
            default: {break;}
        }
        if(data < 0){ continue;}

        int index = data / 10;
        if (index >= 36) { index = 0;} // 0度和360度视为同一区间

        dips.hist[index]++;
        if (dips.hist[index] > m_maxCount) {
            m_maxCount = dips.hist[index];
        }
    }

    // 处理0度和180度对称的情况
    QVector<int> half(18, 0);
    for (int i = 0; i < 18; i++) {
        half[i] = dips.hist[i] + dips.hist[i + 18];
        if (half[i] > m_maxCount) {
            m_maxCount = half[i];
        }
    }

    // 复制到完整的36个区间
    for (int i = 0; i < 18; i++) {
        dips.hist[i] = half[i];
        dips.hist[i + 18] = half[i];
    }
}


void RoseWidget::calculateHistogram(){

    DipDataType showType = getDipDataType();
    auto iter = m_buffer.begin();
    while(iter != m_buffer.end()){
        calculateHistogram(iter.value(), showType);
        iter++;
    }
}
