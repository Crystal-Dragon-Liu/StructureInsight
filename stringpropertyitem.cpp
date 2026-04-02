#include "stringpropertyitem.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>

StringPropertyItem::StringPropertyItem(const QString& name, const QString& defaultText, QWidget* parent)
    : PropertyItem(name, String, parent){
    QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(this->layout());

    QLabel* itemName = new QLabel(this);
    itemName->setText(name + ": ");
    m_text = new QLineEdit(this);
    m_text->setText(defaultText);
    layout->addWidget(itemName);
    layout->addWidget(m_text);
    layout->addStretch();

    connect(m_text, &QLineEdit::textChanged,
            this, [this](const QString& text) {
                emit valueChanged(text);
            });
}

QVariant StringPropertyItem::value() const{
    return m_text->text();
}

void StringPropertyItem::setValue(const QVariant& value){
    if(value.canConvert<QString>()){
        m_text->setText(value.toString());
    }
}
