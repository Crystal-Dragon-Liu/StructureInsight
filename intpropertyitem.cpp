#include "intpropertyitem.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QSpinBox>
IntPropertyItem::IntPropertyItem(const QString& name, int min, int max, int defaultValue, QWidget* parent)
    : PropertyItem(name, Integer, parent)
{
    QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(this->layout());

    m_spinBox = new QSpinBox(this);
    m_spinBox->setRange(min, max);
    m_spinBox->setValue(defaultValue);

    layout->addWidget(m_spinBox);
    layout->addStretch();

    connect(m_spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int value) {
                emit valueChanged(value);
            });
}


QVariant IntPropertyItem::value() const
{
    return m_spinBox->value();
}

void IntPropertyItem::setValue(const QVariant& value)
{
    if (value.canConvert<int>()) {
        m_spinBox->setValue(value.toInt());
    }
}
