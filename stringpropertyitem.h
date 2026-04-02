#ifndef STRINGPROPERTYITEM_H
#define STRINGPROPERTYITEM_H

#include <QWidget>
#include "propertyitem.h"

class QLineEdit;

class StringPropertyItem : public PropertyItem
{
    Q_OBJECT
public:
    StringPropertyItem(const QString& name, const QString& defaultText, QWidget* parent = nullptr);
    QVariant value() const override;
    void setValue(const QVariant& value) override;
signals:
private:
    QLineEdit* m_text;
};

#endif // STRINGPROPERTYITEM_H
