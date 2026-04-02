#ifndef PROPERTYITEM_H
#define PROPERTYITEM_H

#include <QWidget>

class PropertyItem : public QWidget
{
    Q_OBJECT
public:
    enum PropertyType{
        Integer,
        Float,
        MultiSelect,
        SingleSelect,
        String
    };
    explicit PropertyItem(const QString& name, PropertyType type, QWidget *parent = nullptr);
    virtual ~PropertyItem();

    QString name() const { return m_name;}

    PropertyType type() const {return m_type;}

    virtual QVariant value() const = 0;
    virtual void setValue(const QVariant& value) = 0;

signals:
    void valueChanged(const QVariant& value);

    void valueChangedNoArgs();

protected:
    QString m_name;
    PropertyType m_type;

signals:
};

#endif // PROPERTYITEM_H
