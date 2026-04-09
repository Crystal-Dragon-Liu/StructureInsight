#ifndef ADVANCEDDIPPANE_H
#define ADVANCEDDIPPANE_H

#include <QWidget>
#include "propertygroup.h"

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

    // 通用属性组
    PropertyGroup* m_propertyGroup = nullptr;
private:
    void sltValueChangedWithName(const QString& propertyName, const QVariant& value);

protected:
    // 通用属性创建函数
    template<typename PropType, typename... Args>
    void createProperty(Args&&... args)
    {
        PropType* item = new PropType(std::forward<Args>(args)..., m_propertyPanel);
        connect(item, &PropType::valueChangedWithName, this, &AdvancedDipPane::sltValueChangedWithName);
        m_propertyGroup->addProperty(item);
    }
};

#endif // ADVANCEDDIPPANE_H
