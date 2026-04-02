// multiselectpropertyitem.cpp
#include "multiselectpropertyitem.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QListWidget>

MultiSelectPropertyItem::MultiSelectPropertyItem(const QString& name, const QStringList& options, QWidget* parent)
    : PropertyItem(name, MultiSelect, parent)
{
    // 移除原有的水平布局
    delete this->layout();
    QVBoxLayout* layout = new QVBoxLayout(this);
    this->setLayout(layout);
    layout->setContentsMargins(5, 2, 5, 2);

    QLabel* nameLabel = new QLabel(name + ":", this);
    layout->addWidget(nameLabel);

    m_listWidget = new QListWidget(this);
    m_listWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    for (const QString& option : options) {
        QListWidgetItem* item = new QListWidgetItem(option, m_listWidget);
        item->setCheckState(Qt::Unchecked);
        m_listWidget->addItem(item);
    }

    layout->addWidget(m_listWidget);

    connect(m_listWidget, &QListWidget::itemChanged,
            this, [this](QListWidgetItem* item) {
                QStringList selected;
                for (int i = 0; i < m_listWidget->count(); ++i) {
                    QListWidgetItem* currentItem = m_listWidget->item(i);
                    if (currentItem->checkState() == Qt::Checked) {
                        selected.append(currentItem->text());
                    }
                }
                emit valueChanged(selected);
            });
}

QVariant MultiSelectPropertyItem::value() const
{
    QStringList selected;
    for (int i = 0; i < m_listWidget->count(); ++i) {
        QListWidgetItem* item = m_listWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            selected.append(item->text());
        }
    }
    return selected;
}

void MultiSelectPropertyItem::setValue(const QVariant& value)
{
    if (value.canConvert<QStringList>()) {
        QStringList selected = value.toStringList();

        // 先取消所有选择
        for (int i = 0; i < m_listWidget->count(); ++i) {
            m_listWidget->item(i)->setCheckState(Qt::Unchecked);
        }

        // 设置新的选择
        for (const QString& option : selected) {
            for (int i = 0; i < m_listWidget->count(); ++i) {
                QListWidgetItem* item = m_listWidget->item(i);
                if (item->text() == option) {
                    item->setCheckState(Qt::Checked);
                    break;
                }
            }
        }
    }
}
