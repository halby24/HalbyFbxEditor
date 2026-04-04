#pragma once
#include <QWidget>
#include "core/FbxPropertyBridge.h"

class QDoubleSpinBox;

class Vec3EditWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Vec3 value READ value WRITE setValue NOTIFY valueChanged USER true)

public:
    explicit Vec3EditWidget(QWidget* parent = nullptr);

    Vec3 value() const;
    void setValue(const Vec3& v);

signals:
    void valueChanged(const Vec3& v);

private:
    QDoubleSpinBox* mX;
    QDoubleSpinBox* mY;
    QDoubleSpinBox* mZ;

    QDoubleSpinBox* makeSpinBox(const QString& prefix);
    void onSpinBoxChanged();
};
