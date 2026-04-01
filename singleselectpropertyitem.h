#ifndef SINGLESELECTPROPERTYITEM_H
#define SINGLESELECTPROPERTYITEM_H

#include <QObject>
#include "propertyitem.h"

class QComboBox;
class SingleSelectPropertyItem : public PropertyItem
{
    Q_OBJECT
public:
    SingleSelectPropertyItem(const QString& name, const QStringList& options, int defaultIndex = 0, QWidget* parent = nullptr);
    QVariant value() const override;
    void setValue(const QVariant& value) override;

signals:

private:
    QComboBox* m_comboBox;
};

#endif // SINGLESELECTPROPERTYITEM_H
