#ifndef ROSEWIDGET_H
#define ROSEWIDGET_H

// #include <QWidget>
#include "dipwidgetbase.h"



class PropertyGroup;

class RoseWidget : public DipWidgetBase
{
    Q_OBJECT
public:

    explicit RoseWidget(QWidget *parent = nullptr);
    ~RoseWidget();

    void setData(DipDataAccess& data);

    void setTitle(const QString& title);

    PropertyGroup* getPropertyGroup() { return m_propertyGroup; }

    // 获取属性值
    float getMaxFreq() const;
    float getFreqDisplaySize() const;
    float getAngleValueSize() const;
    DipDataType getDipDataType() const;
    QString getDipDataTypeStr() const;
    bool  isShowTypeLabel() const;
    bool  isMirror() const;

    // 初始化属性
    void initProperties() override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVector<int> m_strikes;
    QVector<int> m_histogram;
    QString m_title;
    int m_maxCount;

public slots:
    void onUpdateWithPropertyChanged(const QString& propertyName, const QVariant& value) override;

private:

    void calculateHistogram(AzimuthStatistic& dips, const DipDataType & dataType);

    void calculateHistogram();

    /*@brief 计算绘制Label需要的rect大小*/
    QList<QRect> calculateLabelRegion(const QRect& totalRegion);

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

    QVector<double> calculateSmartGridValues(double maxValue);


};

#endif // ROSEWIDGET_H
