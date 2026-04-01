#ifndef FLOATPROPERTYITEM_H
#define FLOATPROPERTYITEM_H

#include <QObject>
#include "propertyitem.h"
class QDoubleSpinBox;
class FloatPropertyItem : public PropertyItem
{
    Q_OBJECT
public:
    FloatPropertyItem(const QString& name, double min, double max, double defaultValue, QWidget* parent = nullptr);

    QVariant value() const override;
    void setValue(const QVariant& value) override;
private:
    QDoubleSpinBox* m_spinBox;
};

#endif // FLOATPROPERTYITEM_H
