#include "propertyitem.h"
#include <QLabel>
#include <QHBoxLayout>

PropertyItem::PropertyItem(const QString& name, PropertyType type, QWidget *parent)
    : QWidget{parent}, m_name(name), m_type(type){
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 2, 5, 2);

    QLabel* nameLabel = new QLabel(name + ":", this);
    layout->addWidget(nameLabel);
}

PropertyItem::~PropertyItem(){}


