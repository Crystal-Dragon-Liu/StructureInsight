#ifndef INTPROPERTYITEM_H
#define INTPROPERTYITEM_H

#include <QObject>
#include "propertyitem.h"
class QSpinBox;

class IntPropertyItem : public PropertyItem
{
    Q_OBJECT
public:
    IntPropertyItem(const QString& name, int min, int max, int defaultValue, QWidget* parent = nullptr);
    QVariant value() const override;
    void setValue(const QVariant& value) override;
private:
    QSpinBox* m_spinBox;
};

#endif // INTPROPERTYITEM_H
