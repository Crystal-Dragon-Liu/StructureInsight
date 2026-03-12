#include "rosewidget.h"
#include <QPainter>
#include <QFontMetrics>
RoseWidget::RoseWidget(QWidget *parent)
    : QWidget{parent}, m_maxCount(0), m_title(tr("Rose Diagram")){}

RoseWidget::~RoseWidget(){}


void RoseWidget::setStrikes(const QVector<int> &strikes)
{
    m_strikes = strikes;
    calculateHistogram();
    update();
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

    // 绘制玫瑰图的每个扇区
    for (int i = 0; i < 36; i++) {
        double startAngle = (i * 10) * M_PI / 180;
        double endAngle = ((i + 1) * 10) * M_PI / 180;

        // 计算扇区的半径比例
        double radiusRatio = m_maxCount > 0 ? (double)m_histogram[i] / m_maxCount : 0;
        int sectorRadius = radius * radiusRatio;

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
    int radius =qMin(rect.width(), rect.height()) / 2;

    painter->setPen(QPen(Qt::lightGray, 1));

    // 绘制同心圆网格

    int numCircles = 5;
    for(int i = 1; i <= numCircles; i++){
        int circleRadius = radius * i / numCircles;
        painter->drawEllipse(centerX - circleRadius, centerY - circleRadius,
                             2 * circleRadius, 2 * circleRadius);
    }

    // 绘制轴线
    painter->setPen(QPen(Qt::black, 1));
    painter->drawLine(centerX, centerY - radius, centerX, centerY + radius); // 南北轴线
    painter->drawLine(centerX - radius, centerY, centerX + radius, centerY); // 东西轴线

    // 绘制45度轴线
    painter->setPen(QPen(Qt::gray, 1, Qt::DotLine));
    double diagonalRadius = radius * M_SQRT2 / 2;
    painter->drawLine(centerX - diagonalRadius, centerY - diagonalRadius,
                      centerX + diagonalRadius, centerY + diagonalRadius);
    painter->drawLine(centerX - diagonalRadius, centerY + diagonalRadius,
                      centerX + diagonalRadius, centerY - diagonalRadius);
}

void RoseWidget::drawLabels(QPainter *painter, const QRect &rect){
    int centerX = rect.center().x();
    int centerY = rect.center().y();
    int radius = qMin(rect.width(), rect.height()) / 2;
    painter->setPen(QPen(Qt::black, 1));
    painter->setFont(QFont("Arial", 10));

    // 绘制方向标签
    // QString directions[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
    // for (int i = 0; i < 8; i++) {
    //     double angle = i * 45 * M_PI / 180;
    //     int labelX = centerX + (radius + 20) * cos(angle - M_PI / 2);
    //     int labelY = centerY + (radius + 20) * sin(angle - M_PI / 2);

    //     QFontMetrics metrics(painter->font());
    //     int textWidth = metrics.width(directions[i]);
    //     int textHeight = metrics.height();

    //     painter->drawText(labelX - textWidth / 2, labelY + textHeight / 4, directions[i]);
    // }

    // 绘制刻度标签
    painter->setFont(QFont("Arial", 8));
    int numCircles = 5;
    for (int i = 1; i <= numCircles; i++) {
        int circleRadius = radius * i / numCircles;
        int value = m_maxCount * i / numCircles;
        painter->drawText(centerX + 10, centerY - circleRadius + 4, QString::number(value));
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
