// propertypanel.cpp
#include "propertypanel.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include "intpropertyitem.h"
#include "floatpropertyitem.h"
#include "multiselectpropertyitem.h"
#include "propertygroup.h"

PropertyPanel::PropertyPanel(QWidget* parent)
    : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_scrollWidget = new QWidget(this);
    m_scrollLayout = new QVBoxLayout(m_scrollWidget);
    m_scrollLayout->setContentsMargins(5, 5, 5, 5);
    m_scrollLayout->setSpacing(5);
    m_scrollLayout->addStretch();

    m_scrollArea->setWidget(m_scrollWidget);
    m_mainLayout->addWidget(m_scrollArea);
}

PropertyPanel::~PropertyPanel()
{
}

void PropertyPanel::addGroup(PropertyGroup* group)
{
    m_scrollLayout->insertWidget(m_scrollLayout->count() - 1, group);
    m_groups[group->title()] = group;

    // 修正信号连接：使用lambda函数转发信号
    connect(group, &PropertyGroup::propertyChanged, this,
            [this](const QString& groupName, const QString& propertyName, const QVariant& value) {
                emit this->propertyChanged(groupName, propertyName, value);
            }
    );
}

void PropertyPanel::removeGroup(const QString& title)
{
    if (m_groups.contains(title)) {
        PropertyGroup* group = m_groups.take(title);
        m_scrollLayout->removeWidget(group);
        group->deleteLater();
    }
}

PropertyGroup* PropertyPanel::group(const QString& title) const{
    return m_groups.value(title, nullptr);
}

void PropertyPanel::addIntegerProperty(const QString& groupTitle, const QString& name,
                                       int min, int max, int defaultValue){
    PropertyGroup* group = this->group(groupTitle);
    if (!group) {
        group = new PropertyGroup(groupTitle, this);
        addGroup(group);
    }

    IntPropertyItem* property = new IntPropertyItem(name, min, max, defaultValue, this);
    group->addProperty(property);
}

void PropertyPanel::addFloatProperty(const QString& groupTitle, const QString& name,
                                     double min, double max, double defaultValue){
    PropertyGroup* group = this->group(groupTitle);
    if (!group) {
        group = new PropertyGroup(groupTitle, this);
        addGroup(group);
    }

    FloatPropertyItem* property = new FloatPropertyItem(name, min, max, defaultValue, this);
    group->addProperty(property);
}

void PropertyPanel::addMultiSelectProperty(const QString& groupTitle, const QString& name,
                                           const QStringList& options){
    PropertyGroup* group = this->group(groupTitle);
    if (!group) {
        group = new PropertyGroup(groupTitle, this);
        addGroup(group);
    }

    MultiSelectPropertyItem* property = new MultiSelectPropertyItem(name, options, this);
    group->addProperty(property);
}
