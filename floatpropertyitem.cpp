// floatpropertyitem.cpp
#include "floatpropertyitem.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QDoubleSpinBox>

FloatPropertyItem::FloatPropertyItem(const QString& name, double min, double max, double defaultValue, QWidget* parent)
    : PropertyItem(name, Float, parent){
    QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(this->layout());

    m_spinBox = new QDoubleSpinBox(this);
    m_spinBox->setRange(min, max);
    m_spinBox->setValue(defaultValue);
    m_spinBox->setDecimals(2);

    layout->addWidget(m_spinBox);
    layout->addStretch();

    connect(m_spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double value) {
                emit valueChanged(value);
            });
}

QVariant FloatPropertyItem::value() const{
    return m_spinBox->value();
}

void FloatPropertyItem::setValue(const QVariant& value){
    if (value.canConvert<double>()) {
        m_spinBox->setValue(value.toDouble());
    }
}
