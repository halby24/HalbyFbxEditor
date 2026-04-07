#include "PropertyDelegate.h"
#include "Vec3EditWidget.h"
#include "core/FbxPropertyBridge.h"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

PropertyDelegate::PropertyDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

QWidget* PropertyDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() != 1) return nullptr;

    bool readOnly = index.data(Qt::UserRole + 2).toBool();
    if (readOnly) return nullptr;

    int metatype = index.data(Qt::UserRole).toInt();

    if (metatype == qMetaTypeId<Vec3>())
    {
        auto* w = new Vec3EditWidget(parent);
        connect(w, &Vec3EditWidget::valueChanged, this, [this, w]() {
            const_cast<PropertyDelegate*>(this)->commitData(w);
        });
        return w;
    }

    if (metatype == QMetaType::Double)
    {
        auto* spin = new QDoubleSpinBox(parent);
        spin->setRange(-1e9, 1e9);
        spin->setDecimals(6);
        spin->setSingleStep(0.1);
        return spin;
    }

    if (metatype == QMetaType::Bool)
    {
        // Bool properties are handled via paint() + editorEvent() for single-click toggle.
        // No editor widget needed.
        return nullptr;
    }

    if (metatype == QMetaType::Int)
    {
        // Check if this is an enum property by name
        QVariant val = index.data(Qt::EditRole);
        // Determine which enum labels to use
        QString propName;
        QModelIndex nameIdx = index.sibling(index.row(), 0);
        if (nameIdx.isValid())
            propName = nameIdx.data(Qt::DisplayRole).toString();

        QStringList labels;
        if (propName == QStringLiteral("RotationOrder"))
            labels = FbxPropertyBridge::rotationOrderLabels();
        else if (propName == QStringLiteral("InheritType"))
            labels = FbxPropertyBridge::inheritTypeLabels();
        else if (propName == QStringLiteral("QuaternionInterpolate"))
            labels = FbxPropertyBridge::quatInterpLabels();

        if (!labels.isEmpty())
        {
            auto* combo = new QComboBox(parent);
            combo->addItems(labels);
            return combo;
        }

        auto* spin = new QSpinBox(parent);
        spin->setRange(-999999, 999999);
        return spin;
    }

    if (metatype == QMetaType::QString)
    {
        return new QLineEdit(parent);
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void PropertyDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QVariant val = index.data(Qt::EditRole);
    int metatype = index.data(Qt::UserRole).toInt();

    if (metatype == qMetaTypeId<Vec3>())
    {
        if (auto* w = qobject_cast<Vec3EditWidget*>(editor))
            w->setValue(val.value<Vec3>());
        return;
    }

    if (auto* spin = qobject_cast<QDoubleSpinBox*>(editor))
    {
        spin->setValue(val.toDouble());
        return;
    }

    if (auto* cb = qobject_cast<QCheckBox*>(editor))
    {
        cb->setChecked(val.toBool());
        return;
    }

    if (auto* combo = qobject_cast<QComboBox*>(editor))
    {
        combo->setCurrentIndex(val.toInt());
        return;
    }

    if (auto* spin = qobject_cast<QSpinBox*>(editor))
    {
        spin->setValue(val.toInt());
        return;
    }

    if (auto* line = qobject_cast<QLineEdit*>(editor))
    {
        line->setText(val.toString());
        return;
    }

    QStyledItemDelegate::setEditorData(editor, index);
}

void PropertyDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
    const QModelIndex& index) const
{
    int metatype = index.data(Qt::UserRole).toInt();

    if (metatype == qMetaTypeId<Vec3>())
    {
        if (auto* w = qobject_cast<Vec3EditWidget*>(editor))
            model->setData(index, QVariant::fromValue(w->value()), Qt::EditRole);
        return;
    }

    if (auto* spin = qobject_cast<QDoubleSpinBox*>(editor))
    {
        model->setData(index, spin->value(), Qt::EditRole);
        return;
    }

    if (auto* cb = qobject_cast<QCheckBox*>(editor))
    {
        model->setData(index, cb->isChecked(), Qt::EditRole);
        return;
    }

    if (auto* combo = qobject_cast<QComboBox*>(editor))
    {
        model->setData(index, combo->currentIndex(), Qt::EditRole);
        return;
    }

    if (auto* spin = qobject_cast<QSpinBox*>(editor))
    {
        model->setData(index, spin->value(), Qt::EditRole);
        return;
    }

    if (auto* line = qobject_cast<QLineEdit*>(editor))
    {
        model->setData(index, line->text(), Qt::EditRole);
        return;
    }

    QStyledItemDelegate::setModelData(editor, model, index);
}

void PropertyDelegate::updateEditorGeometry(QWidget* editor,
    const QStyleOptionViewItem& option, const QModelIndex&) const
{
    editor->setGeometry(option.rect);
}

bool PropertyDelegate::isBoolProperty(const QModelIndex& index) const
{
    if (!index.isValid() || index.column() != 1) return false;
    return index.data(Qt::UserRole).toInt() == QMetaType::Bool;
}

QRect PropertyDelegate::checkBoxRect(const QStyleOptionViewItem& option) const
{
    QStyleOptionButton checkOpt;
    QRect cbRect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkOpt);
    // Center vertically, left-align with small margin
    QPoint topLeft(option.rect.left() + 4,
                   option.rect.top() + (option.rect.height() - cbRect.height()) / 2);
    return QRect(topLeft, cbRect.size());
}

void PropertyDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    if (isBoolProperty(index))
    {
        // Draw background (selection highlight, etc.)
        QStyleOptionViewItem bgOption = option;
        initStyleOption(&bgOption, index);
        bgOption.text.clear();
        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &bgOption, painter);

        // Draw checkbox
        QStyleOptionButton checkOpt;
        checkOpt.rect = checkBoxRect(option);
        checkOpt.state = QStyle::State_Enabled;
        if (index.data(Qt::EditRole).toBool())
            checkOpt.state |= QStyle::State_On;
        else
            checkOpt.state |= QStyle::State_Off;

        QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkOpt, painter);
        return;
    }

    QStyledItemDelegate::paint(painter, option, index);
}

bool PropertyDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
    const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (!isBoolProperty(index))
        return QStyledItemDelegate::editorEvent(event, model, option, index);

    // Toggle on single click
    if (event->type() == QEvent::MouseButtonRelease)
    {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        if (option.rect.contains(mouseEvent->pos()))
        {
            bool readOnly = index.data(Qt::UserRole + 2).toBool();
            if (readOnly) return false;

            bool current = index.data(Qt::EditRole).toBool();
            model->setData(index, !current, Qt::EditRole);
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
