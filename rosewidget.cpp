#include "rosewidget.h"
#include <QPainter>
#include <QFontMetrics>
#include "propertygroup.h"
#include "floatpropertyitem.h"
#include "singleselectpropertyitem.h"
#include <QRandomGenerator> // 随机数核心类
#include <QDateTime>
#include <QColor>

#define USE_PROPERTY_WIDGET

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
const QString& PROPERTY_MIRROR_OR_NOT   = QObject::tr("Mirror");
const QString& NORTH_ITEM_NAME          = QObject::tr("N");
const QString& SOUTH_ITEM_NAME          = QObject::tr("S");
const QString& WEST_ITEM_NAME           = QObject::tr("W");
const QString& EAST_ITEM_NAME           = QObject::tr("E");


#define DEBUG_MODE
#define PIE_PADDING_RATIO 0.05f

RoseWidget::RoseWidget(QWidget *parent)
    : DipWidgetBase{parent, ROSE_WIDGET_NAME},
    m_maxCount(0),
    m_title(ROSE_WIDGET_NAME){}

RoseWidget::~RoseWidget(){}

void RoseWidget:: setData(DipDataAccess& data){
    m_buffer.clear();
    QVector<QString> typeList;
    data.getDipClassSet(typeList);
    float sdep = 99999;
    float edep = -99999;
    for(int i = 0; i < typeList.size(); i++){
        AzimuthStatistic statistic;
        data.getDataByType(typeList[i], statistic.dips);
        foreach(auto dip,  statistic.dips){
            if(sdep > dip.depth){
                sdep = dip.depth;
            }
            if(edep < dip.depth){
                edep = dip.depth;
            }
        }

        // 设置该类型对应的颜色
        // TODO 需要找到更好的办法来设置类型颜色
        int r = QRandomGenerator::global()->bounded(256);
        int g = QRandomGenerator::global()->bounded(256);
        int b = QRandomGenerator::global()->bounded(256);
        statistic.color = QColor(r, g, b);
        m_buffer.insert(typeList[i], statistic);
    }

    // 设置一下深度数据
    setTopDepth(sdep);
    setBottomDepth(edep);
    // 重新计算直方图数据
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
        1
    );

    // 是否进行镜像
    createProperty<SingleSelectPropertyItem>(
        PROPERTY_MIRROR_OR_NOT,
        QStringList({YES, NO}),
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


void RoseWidget::onUpdateWithPropertyChanged(const QString& propertyName, const QVariant& value){
    if(propertyName.compare(PROPERTY_SHOW_AZI_TYPE) == 0){
        calculateHistogram();
        update();
    }
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

bool  RoseWidget::isMirror() const{
    if(m_propertyGroup){
        PropertyItem* property = m_propertyGroup->property(PROPERTY_MIRROR_OR_NOT);
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

QString RoseWidget::getDipDataTypeStr() const{
    if(m_propertyGroup){
        PropertyItem* property = m_propertyGroup->property(PROPERTY_SHOW_AZI_TYPE);
        return property->value().toString();
    }
    else{
        return "UNKNOWN";
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


void RoseWidget::setTitle(const QString &title)
{
    m_title = title;
    update();
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
        QVector<double> gridValues = calculateSmartGridValues(m_maxCount);
        qDebug() << gridValues.size();
        int numCircles = gridValues.size();

        // 确保最外面的圆对应m_maxCount
        for (int i = 1; i < numCircles; i++) {
            double ratio = gridValues[i] / m_maxCount;
            ratio = sqrt(gridValues[i]) / sqrt(m_maxCount);
            int circleRadius = radius * ratio;
            painter->drawEllipse(centerX - circleRadius, centerY - circleRadius,
                                 2 * circleRadius, 2 * circleRadius);
        }
    } else {
        // 没有数据时绘制固定网格
        int numCircles = 5;
        for (int i = 1; i < numCircles; i++) {
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

QVector<double> RoseWidget::calculateSmartGridValues(double maxValue){
    QVector<double> gridValues;
    if(maxValue <= 0){
        // 没有数据时返回固定值
        gridValues << 0.2 << 0.4 << 0.6 << 0.8 << 1.0;
        return gridValues;
    }
    // 计算格线数量
    int numGrids = 5;
    // 计算合适的格线间距
    double range = maxValue;
    double exponent = floor(log10(range));
    double fraction = range / pow(10, exponent);
    double step;
    if (fraction <= 1.5) {
        step = 0.2 * pow(10, exponent);
    } else if (fraction <= 3) {
        step = 0.5 * pow(10, exponent);
    } else if (fraction <= 7) {
        step = 1.0 * pow(10, exponent);
    } else {
        step = 2.0 * pow(10, exponent);
    }
    // 调整步长以确保合适的格线数量
    double estimatedGrids = range / step;
    if (estimatedGrids > 8) {
        step *= 2;
    } else if (estimatedGrids < 3) {
        step /= 2;
    }
    // 生成格线值
    double currentValue = 0;
    while (currentValue <= maxValue * 1.05) { // 稍微超过最大值
        if (currentValue > 0) { // 跳过0值格线
            gridValues << currentValue;
        }
        currentValue += step;
    }
    // 确保至少有一条格线
    if (gridValues.isEmpty()) {
        gridValues << step;
    }
    // 限制格线数量在合理范围内
    if (gridValues.size() > 6) {
        gridValues = gridValues.mid(0, 6);
    }
    return gridValues;
}

void RoseWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect rect = this->rect();

    QRect labelTotalRect, diagramRectBoundingRect, diagramRect, abstractTotalRect;
    // 获取有多少种rect
    QStringList keys = m_buffer.keys();

    calculateDrawArea(rect, labelTotalRect, diagramRectBoundingRect, diagramRect, abstractTotalRect);


#ifdef DEBUG_MODE
    debug_drawRect(&painter, DEBUG_CANVAS_RECT_COLOR    , rect,                     "Rect");
    // debug_drawRect(&painter, DEBUG_PLOT_RECT_COLOR      , diagramRectBoundingRect,  "Outside Rect");
    // debug_drawRect(&painter, DEBUG_PLOT_RECT_COLOR      , diagramRect,              "Inside Rect");
    // debug_drawRect(&painter, DEBUG_INFO_RECT_COLOR      , labelTotalRect,           "Label Rect");
    // debug_drawRect(&painter, DEBUG_INFO_RECT_COLOR      , abstractTotalRect,        "Abstract Rect");
#endif

    if(m_buffer.size() == 0){
        painter.drawText(diagramRect.center(), "No data");
        return;
    }

    // TODO 绘制画布
    drawCanvas(&painter, diagramRectBoundingRect);
    // 绘制网格
    drawGrid(&painter, diagramRect);

    // 绘制玫瑰图
    drawRoseDiagram(&painter, diagramRect);

    // 绘制label(包括颜色等)
    drawLabels(&painter, diagramRect);

    // 整理绘图摘要的信息
    QStringList abstractList;
    // Plot name.
    abstractList << QString(tr("Stereonet : %1").arg("DIP TEST"));
    QString indexType = "M";
    // 深度信息
    abstractList << QString(tr("Reference(%1) : [%2 - %3]").arg(indexType).arg(getTopDepth()).arg(getBottomDepth()));
    // 绘图摘要信息
    abstractList << QString(tr("Rose - %1").arg(getDipDataTypeStr()));
    drawAbstractInfo(&painter, abstractTotalRect, abstractList);
    // 绘制label
    drawTypeLabel(&painter, labelTotalRect, keys);

}

void RoseWidget::drawLabels(QPainter* painter, const QRect& rect){
    painter->save();
    int centerX = rect.center().x();
    int centerY = rect.center().y();
    int radius = qMin(rect.width(), rect.height()) / 2;
    painter->setPen(QPen(Qt::black, 1));
    painter->setFont(QFont("Arial", 10));

    double padding = 0.1;
    int extendRadius = radius * (1+padding);

    // 绘制刻度标签(显示实际数据频率百分比)
    painter->setFont(QFont("Arial", 8));
    if (m_maxCount > 0) {

        QVector<double> gridValues = calculateSmartGridValues(m_maxCount);
        int numCircles = gridValues.size();

        // 计算总数据量
        int totalDataCount = 0;
        auto iter = m_buffer.begin();
        while(iter != m_buffer.end()){
            totalDataCount += std::accumulate(iter->hist.begin(), iter->hist.end(), 0);
            iter++;
        }

        for (int i = 1; i < numCircles; i++) {
            double ratio = gridValues[i] / m_maxCount;
            // 使用平方根归一化：与drawRoseDiagram2和drawGrid2保持一致
            ratio = sqrt(gridValues[i]) / sqrt(m_maxCount);
            int circleRadius = radius * ratio;
            // 计算该格线对应的实际数据百分比
            double actualPercentage = totalDataCount > 0 ? (gridValues[i] / totalDataCount * 100) : 0;
            // 格式化显示: 保留1位小数或整数
            QString percentageText;
            if (actualPercentage < 1) {
                percentageText = QString::number(actualPercentage, 'f', 1) + "%";
            } else {
                percentageText = QString::number(static_cast<int>(actualPercentage + 0.5)) + "%";
            }
            painter->drawText(centerX, centerY - circleRadius, percentageText);
        }
    } else {
        // 没有数据时绘制固定标签
        int numCircles = 5;
        for (int i = 1; i <= numCircles; i++) {
            int circleRadius = radius * i / numCircles;
            int percentage = (int)((double)i / numCircles * 100 + 0.5);
            painter->drawText(centerX + 10, centerY - circleRadius + 4, QString::number(percentage) + "%");
        }
    }

    // 绘制360度数值标签
    float angleSize = this->getAngleValueSize();
    painter->setFont(QFont("Arial", angleSize));
    for(int angle = 0; angle < 360; angle += 10){
        double rad = qDegreesToRadians(angle - 90);
        int labelX = centerX + (extendRadius) * cos(rad);
        int labelY = centerY + (extendRadius) * sin(rad);
        QFontMetrics metrics(painter->font());
        QString angleText = QString::number(angle) + "°";
        int textWidth = metrics.horizontalAdvance(angleText);
        int textHeight = metrics.height();
        int offsetX = labelX - textWidth / 2;
        int offsetY = labelY + textHeight / 2;
        painter->drawText(offsetX, offsetY, angleText);

        // 绘制东南西北标签
        QString directionItem = "";
        if(angle == 0){
            directionItem = NORTH_ITEM_NAME;
            offsetY -= textHeight * 1.2;
            painter->drawText(offsetX, offsetY, directionItem);
        }
        else if(angle == 90){
            directionItem = EAST_ITEM_NAME;
            offsetX += textWidth *1.2;
            painter->drawText(offsetX, offsetY, directionItem);
        }
        else if(angle == 180){
            directionItem = SOUTH_ITEM_NAME;
            offsetY += textHeight*1.2;
            painter->drawText(offsetX, offsetY, directionItem);
        }
        else if(angle == 270){
            directionItem = WEST_ITEM_NAME;
            offsetX -= textWidth *1.2;
            painter->drawText(offsetX, offsetY, directionItem);
        }
    }
    painter->restore();
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
    int effectiveRadius = radius * (1 + padding);

    // 绘制玫瑰图的每个扇区
    auto iter = m_buffer.begin();
    while(iter != m_buffer.end()){
        for (int i = 0; i < 36; i++) {
            double startAngle = (i * 10 - 90) * M_PI / 180;
            double endAngle = ((i + 1) * 10 -90) * M_PI / 180;

            // 计算扇区的半径比例(使用平方根归一化)
            double radiusRatio = 0;
            if (m_maxCount > 0) {
                // 平方根归一化：避免极端值导致图形变形
                radiusRatio = sqrt((double)m_buffer[iter.key()].hist[i]) / sqrt(m_maxCount);
                // 可选：对数归一化，更平滑但可能过度压缩
                // radiusRatio = log10(m_buffer[iter.key()].hist[i] + 1) / log10(m_maxCount + 1);
            }
            int sectorRadius = effectiveRadius * radiusRatio;
            // 绘制扇区, 使用当前type对应的color
            painter->setBrush(iter->color);
            painter->drawPie(centerX - sectorRadius, centerY - sectorRadius, 2 * sectorRadius, 2 * sectorRadius, -startAngle * 180 / M_PI * 16, -(endAngle - startAngle) * 180 / M_PI * 16);
        }
        iter++;
    }
    painter->restore();
}


QList<QRect> RoseWidget::calculateLabelRegion(const QRect& totalRegion){
    QList<QRect> returned;
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
        returned.append(labelRect);
    }
    return returned;
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
    // QVector<int> half(18, 0);
    // for (int i = 0; i < 18; i++) {
    //     half[i] = dips.hist[i] + dips.hist[i + 18];
    //     if (half[i] > m_maxCount) {
    //         m_maxCount = half[i];
    //     }
    // }

    // // 复制到完整的36个区间
    // for (int i = 0; i < 18; i++) {
    //     dips.hist[i] = half[i];
    //     dips.hist[i + 18] = half[i];
    // }
}


void RoseWidget::calculateHistogram(){

    DipDataType showType = getDipDataType();
    auto iter = m_buffer.begin();
    while(iter != m_buffer.end()){
        calculateHistogram(iter.value(), showType);
        iter++;
    }
}
