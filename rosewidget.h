#ifndef ROSEWIDGET_H
#define ROSEWIDGET_H

#include <QWidget>

class PropertyGroup;

class RoseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RoseWidget(QWidget *parent = nullptr);
    ~RoseWidget();

    void setStrikes(const QVector<int> &strikes);

    void setTitle(const QString& title);

    PropertyGroup* getPropertyGroup() { return m_propertyGroup; }

    // 获取属性值
    float getMaxFreq() const;
    float getFreqDisplaySize() const;
    float getAngleValueSize() const;


public slots:
    void onSetStrikes(const QVector<int>& strikes);

    void onUpdateSelfPaint();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVector<int> m_strikes;
    QVector<int> m_histogram;
    QString m_title;
    int m_maxCount;

private:

    void initProperties();

    void calculateHistogram();

    /*
     * @brief 绘制Rose图
     */
    void drawRoseDiagram(QPainter *painter, const QRect &rect);
    /*
     * @brief 绘制玫瑰图网格
     * @details TODO 目前网格数量是固定的, 等待优化完善
     */
    void drawGrid(QPainter *painter, const QRect &rect);
    /*
     * @brief 绘制网格旁边的label
     */
    void drawLabels(QPainter *painter, const QRect &rect);

private:
    // 属性组
    PropertyGroup* m_propertyGroup;
};

#endif // ROSEWIDGET_H
