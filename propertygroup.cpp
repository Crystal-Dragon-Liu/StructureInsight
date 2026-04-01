#include "propertygroup.h"
#include <QMap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include "propertyitem.h"
PropertyGroup::PropertyGroup(const QString& title, QWidget *parent)
    : QWidget{parent}, m_expanded(true){
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Title
    QWidget* headerWidget = new QWidget(this);
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(5, 2, 5, 2);
    m_toggleButton = new QToolButton(this);
    m_toggleButton->setArrowType(Qt::DownArrow);
    m_toggleButton->setStyleSheet("QToolButton { border: none; }");
    m_toggleButton->setFixedSize(16, 16);
    m_titleLabel = new QLabel(title, this);
    m_titleLabel->setStyleSheet("font-weight: bold;");
    headerLayout->addWidget(m_toggleButton);
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();

    // 内容区域
    m_contentFrame = new QFrame(this);
    m_contentFrame->setFrameStyle(QFrame::Box);
    m_contentLayout = new QVBoxLayout(m_contentFrame);
    m_contentLayout->setContentsMargins(5, 5, 5, 5);
    m_contentLayout->setSpacing(2);

    m_mainLayout->addWidget(headerWidget);
    m_mainLayout->addWidget(m_contentFrame);

    connect(m_toggleButton, &QToolButton::clicked, this, &PropertyGroup::sltToggleExpansion);
}

void PropertyGroup::addProperty(PropertyItem* property){
    m_contentLayout->addWidget(property);
    m_properties[property->name()] = property;
    connect(property, &PropertyItem::valueChanged, this, [this, property](const QVariant& value) {
        emit propertyChanged(m_titleLabel->text(), property->name(), value);
    });
}

void PropertyGroup::removeProperty(const QString& name){
    if(m_properties.contains(name)){
        PropertyItem* property = m_properties.take(name);
        m_contentLayout->removeWidget(property);
        property->deleteLater();
    }
}

PropertyItem* PropertyGroup::property(const QString& name){
    return m_properties.value(name, nullptr);
}

void PropertyGroup::setExpanded(bool expanded){
    m_expanded = expanded;
    m_contentFrame->setVisible(expanded);
    m_toggleButton->setArrowType(expanded ? Qt::DownArrow : Qt::RightArrow);
}

void PropertyGroup::sltToggleExpansion(){
    setExpanded(!m_expanded);
}

QString PropertyGroup::title() const{
    return m_titleLabel->text();
}
