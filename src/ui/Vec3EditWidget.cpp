#include "Vec3EditWidget.h"
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QLabel>

Vec3EditWidget::Vec3EditWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    mX = makeSpinBox(QStringLiteral("X"));
    mY = makeSpinBox(QStringLiteral("Y"));
    mZ = makeSpinBox(QStringLiteral("Z"));

    layout->addWidget(new QLabel(QStringLiteral("X:")));
    layout->addWidget(mX);
    layout->addWidget(new QLabel(QStringLiteral("Y:")));
    layout->addWidget(mY);
    layout->addWidget(new QLabel(QStringLiteral("Z:")));
    layout->addWidget(mZ);

    setFocusProxy(mX);
}

QDoubleSpinBox* Vec3EditWidget::makeSpinBox(const QString&)
{
    auto* spin = new QDoubleSpinBox(this);
    spin->setRange(-1e9, 1e9);
    spin->setDecimals(6);
    spin->setSingleStep(0.1);
    spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    connect(spin, &QDoubleSpinBox::valueChanged,
            this, &Vec3EditWidget::onSpinBoxChanged);
    return spin;
}

Vec3 Vec3EditWidget::value() const
{
    return Vec3(mX->value(), mY->value(), mZ->value());
}

void Vec3EditWidget::setValue(const Vec3& v)
{
    QSignalBlocker bx(mX), by(mY), bz(mZ);
    mX->setValue(v.x);
    mY->setValue(v.y);
    mZ->setValue(v.z);
}

void Vec3EditWidget::onSpinBoxChanged()
{
    emit valueChanged(value());
}
