#pragma once
#include <QString>
#include <QVariant>
#include <QVector>
#include <QMetaType>
#include <functional>
#include <fbxsdk.h>

// Custom Vec3 type for QVariant
struct Vec3
{
    double x = 0.0, y = 0.0, z = 0.0;

    Vec3() = default;
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
    explicit Vec3(const FbxDouble3& v) : x(v[0]), y(v[1]), z(v[2]) {}

    FbxDouble3 toFbxDouble3() const { return FbxDouble3(x, y, z); }

    bool operator==(const Vec3& o) const { return x == o.x && y == o.y && z == o.z; }
    bool operator!=(const Vec3& o) const { return !(*this == o); }
};
Q_DECLARE_METATYPE(Vec3)

struct PropertyDescriptor
{
    QString name;
    QString category;
    int metatype;       // QMetaType id for editor widget selection
    bool readOnly = false;
    std::function<QVariant(FbxNode*)> getter;
    std::function<void(FbxNode*, const QVariant&)> setter;
};

namespace FbxPropertyBridge
{
    QVector<PropertyDescriptor> buildNodeProperties(FbxNode* node);

    // Enum label helpers
    QStringList rotationOrderLabels();
    QStringList inheritTypeLabels();
    QStringList quatInterpLabels();
}
