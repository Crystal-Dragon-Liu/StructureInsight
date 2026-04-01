#ifndef MULTISELECTPROPERTYITEM_H
#define MULTISELECTPROPERTYITEM_H

#include <QObject>
#include "propertyitem.h"
class QListWidget;
class MultiSelectPropertyItem : public PropertyItem
{
    Q_OBJECT
public:
    MultiSelectPropertyItem(const QString& name, const QStringList& options, QWidget* parent = nullptr);

    QVariant value() const override;
    void setValue(const QVariant& value) override;


private:
    QListWidget* m_listWidget;
};

#endif // MULTISELECTPROPERTYITEM_H
