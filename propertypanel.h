#ifndef PROPERTYPANEL_H
#define PROPERTYPANEL_H

#include <QWidget>

class QVBoxLayout;
class QScrollArea;
class PropertyGroup;

class PropertyPanel : public QWidget
{
    Q_OBJECT
public:
    explicit PropertyPanel(QWidget *parent = nullptr);
    ~PropertyPanel();

    // 添加属性组
    void addGroup(PropertyGroup* group);
    void removeGroup(const QString& title);

    // 快捷方法：直接添加属性
    void addIntegerProperty(const QString& groupTitle, const QString& name,
                            int min, int max, int defaultValue);
    void addFloatProperty(const QString& groupTitle, const QString& name,
                          double min, double max, double defaultValue);
    void addMultiSelectProperty(const QString& groupTitle, const QString& name,
                                const QStringList& options);

    PropertyGroup* group(const QString& title) const;


signals:
    void propertyChanged(
        const QString& group,
        const QString& name,
        const QVariant& value);

private:
    QVBoxLayout* m_mainLayout;
    QScrollArea* m_scrollArea;
    QWidget* m_scrollWidget;
    QVBoxLayout* m_scrollLayout;
    QMap<QString, PropertyGroup*> m_groups;
};

#endif // PROPERTYPANEL_H
