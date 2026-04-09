#include "singleselectpropertyitem.h"
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>

SingleSelectPropertyItem::SingleSelectPropertyItem(const QString& name, const QStringList& options, int defaultIndex, QWidget* parent)
    : PropertyItem(name, PropertyType::SingleSelect, parent){
    QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(this->layout());

    m_comboBox = new QComboBox(this);
    m_comboBox->addItems(options);
    if (defaultIndex >= 0 && defaultIndex < options.size()) {
        m_comboBox->setCurrentIndex(defaultIndex);
    }

    QLabel* nameLabel = new QLabel(name + ":", this);
    layout->addWidget(nameLabel);
    layout->addWidget(m_comboBox);
    layout->addStretch();

    connect(m_comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                emit valueChanged(m_comboBox->currentText());
                emit valueChangedNoArgs();
                emit valueChangedWithName(m_name, m_comboBox->currentText());
    });
}


QVariant SingleSelectPropertyItem::value() const{
    return m_comboBox->currentText();
}

void SingleSelectPropertyItem::setValue(const QVariant& value){
    if (value.canConvert<QString>()) {
        QString text = value.toString();
        int index = m_comboBox->findText(text);
        if (index >= 0) {
            m_comboBox->setCurrentIndex(index);
        }
    }
}
