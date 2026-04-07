#ifndef ROSEWIDGET_H
#define ROSEWIDGET_H

// #include <QWidget>
#include "dipwidgetbase.h"
#include "dipdataaccess.h"



class PropertyGroup;

class RoseWidget : public DipWidgetBase
{
    Q_OBJECT
public:

    struct AzimuthStatistic{
        QList<DipData> dips;
        QVector<float> hist;
    };

    explicit RoseWidget(QWidget *parent = nullptr);
    ~RoseWidget();

    void setData(DipDataAccess& data);

    void setStrikes(const QVector<int> &strikes);

    void setTitle(const QString& title);

    PropertyGroup* getPropertyGroup() { return m_propertyGroup; }

    // 获取属性值
    float getMaxFreq() const;
    float getFreqDisplaySize() const;
    float getAngleValueSize() const;
    DipDataType getDipDataType() const;

    // 初始化属性
    void initProperties() override;


public slots:
    void onSetStrikes(const QVector<int>& strikes);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVector<int> m_strikes;
    QVector<int> m_histogram;
    QString m_title;
    int m_maxCount;

    // 分组记录不同类型的histogram
    QMap<QString, AzimuthStatistic> m_buffer;


private:

    void calculateHistogram(AzimuthStatistic& dips, const DipDataType & dataType);

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

    /*@brief 绘制信息*/
    // void drawInfo(QPainter* painter, const QRect& rect);
};

#endif // ROSEWIDGET_H
