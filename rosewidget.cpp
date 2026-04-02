#include "rosewidget.h"
#include <QPainter>
#include <QFontMetrics>
#include "propertygroup.h"
#include "floatpropertyitem.h"

const QString& PROPERTY_MAX_FREQ = QObject::tr("Max Frequency");
const QString& PROPERTY_ANGLE_SIZE = QObject::tr("Angle Size");
const QString& PROPERTY_FREQ_SIZE = QObject::tr("Freq Size");

RoseWidget::RoseWidget(QWidget *parent)
    : QWidget{parent}, m_maxCount(0), m_title(tr("Rose Diagram")), m_propertyGroup(nullptr){

    // 初始化属性
    initProperties();

}

RoseWidget::~RoseWidget(){}


void RoseWidget::setStrikes(const QVector<int> &strikes)
{
    m_strikes = strikes;
    calculateHistogram();
    update();
}

void RoseWidget::initProperties(){

    // Rose 相关属性
    if(m_propertyGroup != nullptr){
        delete m_propertyGroup;
        m_propertyGroup = nullptr;
    }
    m_propertyGroup = new PropertyGroup(tr("Rose Plot"), this);

    // 刻度最大值
    FloatPropertyItem* maxFreqItem  = new FloatPropertyItem(PROPERTY_MAX_FREQ, 0, 10000, 20.0, m_propertyGroup);
    m_propertyGroup->addProperty(maxFreqItem);

    // 角度数值大小
    FloatPropertyItem* angleValueSize = new FloatPropertyItem(PROPERTY_ANGLE_SIZE, 0, 20, 9, m_propertyGroup);
    connect(angleValueSize, SIGNAL(valueChangedNoArgs()), this, SLOT(onUpdateSelfPaint()));
    m_propertyGroup->addProperty(angleValueSize);

    // 频率数值显示尺寸
    FloatPropertyItem* freqDisplaySizeItem = new FloatPropertyItem(PROPERTY_FREQ_SIZE, 0, 20, 9, m_propertyGroup);
    m_propertyGroup->addProperty(freqDisplaySizeItem);

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

float RoseWidget::getAngleValueSize() const{
    if(m_propertyGroup){
        PropertyItem* property = m_propertyGroup->property(PROPERTY_ANGLE_SIZE);
        return property->value().toFloat();
    }
    else{
        return 9.0f;
    }
}

void RoseWidget::onUpdateSelfPaint(){
    update();
}

void RoseWidget::onSetStrikes(const QVector<int>& strikes){
    setStrikes(strikes);
}

void RoseWidget::setTitle(const QString &title)
{
    m_title = title;
    update();
}

void RoseWidget::drawRoseDiagram(QPainter *painter, const QRect &rect){
    int centerX = rect.center().x();
    int centerY = rect.center().y();
    int radius = qMin(rect.width(), rect.height()) / 2;

    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QBrush(QColor(200, 200, 200)));

    // 预留一些空间，与matplotlib的效果类似
    double padding = 0.05; // 5%的额外空间
    int effectiveRadius = radius * (1 - padding);

    // 绘制玫瑰图的每个扇区
    for (int i = 0; i < 36; i++) {
        double startAngle = (i * 10) * M_PI / 180;
        double endAngle = ((i + 1) * 10) * M_PI / 180;

        // 计算扇区的半径比例
        double radiusRatio = m_maxCount > 0 ? (double)m_histogram[i] / m_maxCount : 0;
        int sectorRadius = effectiveRadius * radiusRatio;

        // 绘制扇区
        painter->drawPie(centerX - sectorRadius, centerY - sectorRadius,
                         2 * sectorRadius, 2 * sectorRadius,
                         -startAngle * 180 / M_PI * 16,
                         -(endAngle - startAngle) * 180 / M_PI * 16);
    }

}

void RoseWidget::drawGrid(QPainter *painter, const QRect &rect){
    int centerX = rect.center().x();
    int centerY = rect.center().y();
    int radius = qMin(rect.width(), rect.height()) / 2;

    // 在最外面向外扩展一点空间，与matplotlib的效果类似
    double padding = 0.05; // 5%的额外空间
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
}

void RoseWidget::drawLabels(QPainter *painter, const QRect &rect){
    int centerX = rect.center().x();
    int centerY = rect.center().y();
    int radius = qMin(rect.width(), rect.height()) / 2;
    painter->setPen(QPen(Qt::black, 1));
    painter->setFont(QFont("Arial", 10));

    // // 绘制方向标签
    // QString directions[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
    // for (int i = 0; i < 8; i++) {
    //     double angle = i * 45 * M_PI / 180;
    //     int labelX = centerX + (radius + 20) * cos(angle - M_PI / 2);
    //     int labelY = centerY + (radius + 20) * sin(angle - M_PI / 2);

    //     QFontMetrics metrics(painter->font());
    //     int textWidth = metrics.horizontalAdvance(directions[i]);
    //     int textHeight = metrics.height();
    //     painter->drawText(labelX - textWidth / 2, labelY + textHeight / 4, directions[i]);
    // }

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
}

void RoseWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect rect = this->rect();
    int margin = 50;
    QRect diagramRect = rect.adjusted(margin, margin, -margin, -margin);

    // 绘制标题
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(rect.center().x(), margin / 2, m_title);

    if (m_strikes.isEmpty()) {
        painter.drawText(diagramRect.center(), "No data");
        return;
    }

    drawGrid(&painter, diagramRect);
    drawRoseDiagram(&painter, diagramRect);
    drawLabels(&painter, diagramRect);
}


void RoseWidget::calculateHistogram()
{
    // 初始化直方图数组，36个区间，每个10度
    m_histogram.resize(36, 0);
    std::fill(this->m_histogram.begin(), this->m_histogram.end(), 0);
    m_maxCount = 0;

    // 统计每个区间的数量
    for (int strike : m_strikes) {
        int index = strike / 10;
        if (index >= 36) {
            index = 0; // 0度和360度视为同一区间
        }
        m_histogram[index]++;
        if (m_histogram[index] > m_maxCount) {
            m_maxCount = m_histogram[index];
        }
    }

    // 处理0度和180度对称的情况
    QVector<int> half(18, 0);
    for (int i = 0; i < 18; i++) {
        half[i] = m_histogram[i] + m_histogram[i + 18];
        if (half[i] > m_maxCount) {
            m_maxCount = half[i];
        }
    }

    // 复制到完整的36个区间
    for (int i = 0; i < 18; i++) {
        m_histogram[i] = half[i];
        m_histogram[i + 18] = half[i];
    }
}
