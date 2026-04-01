#ifndef ADVANCEDDIPPANE_H
#define ADVANCEDDIPPANE_H

#include <QWidget>

namespace Ui {
class AdvancedDipPane;
}

class StereonetWidget;
class StereonetControlPane;
class RoseControlPane;
class RoseWidget;
class QListWidgetItem;
class DipDataAccess;
class PropertyPanel;

class AdvancedDipPane : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedDipPane(QWidget *parent = nullptr);
    ~AdvancedDipPane();
    // 新增属性栏
    void initPropertyPanel();

private slots:
    void onStackedWidgetChanged(int index);
    void onAddPlaneClicked();
    void onClearClicked();
    void onProjectionTypeChanged(int index);
    void onRadioButtonToggled(bool checked);
    void onDataBrowserBtnClicked();
    void onListWidgetItemClicked(QListWidgetItem *item);


private:
    void setupStereonetWrapper();
    void setupRoseWrapper();
    void setupControlPane();

    Ui::AdvancedDipPane *ui;
    
    // Stereonet 相关
    StereonetWidget *m_stereonetWidget = nullptr;
    StereonetControlPane *m_stereonetControlPane = nullptr;
    
    // Rose plot 相关
    RoseWidget* m_roseWidget = nullptr;
    RoseControlPane *m_roseControlPane = nullptr;

    // 数据接口
    DipDataAccess* m_dataInterface = nullptr;

    // 属性栏(可选)
    PropertyPanel* m_propertyPanel = nullptr;
};

#endif // ADVANCEDDIPPANE_H
