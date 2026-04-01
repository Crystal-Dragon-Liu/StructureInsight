#ifndef PROPERTYGROUP_H
#define PROPERTYGROUP_H

#include <QObject>
#include <QWidget>
class PropertyItem;
class QToolButton;
class QLabel;
class QVBoxLayout;
class QFrame;

class PropertyGroup : public QWidget
{
    Q_OBJECT
public:
    explicit PropertyGroup(const QString& title, QWidget *parent = nullptr);
    ~PropertyGroup() = default;

    // 添加属性组
    void addProperty(PropertyItem* property);
    void removeProperty(const QString& name);
    PropertyItem* property(const QString& name);

    void setExpanded(bool expanded);
    bool isExpanded() const {return m_expanded;}

    QString title() const;

private slots:
    void sltToggleExpansion();

signals:
    void propertyChanged(const QString& group, const QString& name, const QVariant& value);
private:
    bool m_expanded;
    QToolButton* m_toggleButton;
    QLabel* m_titleLabel;
    QFrame* m_contentFrame;
    QVBoxLayout* m_contentLayout;
    QVBoxLayout* m_mainLayout;
    QMap<QString, PropertyItem*> m_properties;

signals:
};

#endif // PROPERTYGROUP_H
