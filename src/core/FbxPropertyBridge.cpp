#include "FbxPropertyBridge.h"
#include "FbxEnums.h"

// Helper macros
#define PROP_VEC3(propName, category, fbxProp) \
    { propName, category, qMetaTypeId<Vec3>(), false, \
      [](FbxNode* n) -> QVariant { return QVariant::fromValue(Vec3(n->fbxProp.Get())); }, \
      [](FbxNode* n, const QVariant& v) { n->fbxProp.Set(v.value<Vec3>().toFbxDouble3()); } }

#define PROP_VEC3_RO(propName, category, fbxProp) \
    { propName, category, qMetaTypeId<Vec3>(), true, \
      [](FbxNode* n) -> QVariant { return QVariant::fromValue(Vec3(n->fbxProp.Get())); }, \
      nullptr }

#define PROP_DOUBLE(propName, category, fbxProp) \
    { propName, category, QMetaType::Double, false, \
      [](FbxNode* n) -> QVariant { return n->fbxProp.Get(); }, \
      [](FbxNode* n, const QVariant& v) { n->fbxProp.Set(v.toDouble()); } }

#define PROP_BOOL(propName, category, fbxProp) \
    { propName, category, QMetaType::Bool, false, \
      [](FbxNode* n) -> QVariant { return (bool)n->fbxProp.Get(); }, \
      [](FbxNode* n, const QVariant& v) { n->fbxProp.Set(v.toBool()); } }

#define PROP_ENUM(propName, category, fbxProp, enumType) \
    { propName, category, QMetaType::Int, false, \
      [](FbxNode* n) -> QVariant { return (int)n->fbxProp.Get(); }, \
      [](FbxNode* n, const QVariant& v) { n->fbxProp.Set((enumType)v.toInt()); } }

QVector<PropertyDescriptor> FbxPropertyBridge::buildNodeProperties(FbxNode* node)
{
    if (!node) return {};

    QVector<PropertyDescriptor> props;

    // --- Basic ---
    props.append({
        QStringLiteral("Name"), QStringLiteral("Basic"),
        QMetaType::QString, false,
        [](FbxNode* n) -> QVariant { return QString::fromUtf8(n->GetName()); },
        [](FbxNode* n, const QVariant& v) { n->SetName(v.toString().toUtf8().constData()); }
    });

    props.append({
        QStringLiteral("Type"), QStringLiteral("Basic"),
        QMetaType::QString, true,
        [](FbxNode* n) -> QVariant {
            return FbxEnums::nodeTypeName(FbxEnums::fromAttribute(n->GetNodeAttribute()));
        },
        nullptr
    });

    props.append(PROP_DOUBLE("Visibility", "Basic", Visibility));
    props.append(PROP_BOOL("VisibilityInheritance", "Basic", VisibilityInheritance));

    // --- Transform ---
    props.append(PROP_VEC3("LclTranslation", "Transform", LclTranslation));
    props.append(PROP_VEC3("LclRotation", "Transform", LclRotation));
    props.append(PROP_VEC3("LclScaling", "Transform", LclScaling));

    // --- Rotation ---
    props.append(PROP_ENUM("RotationOrder", "Rotation",
                           RotationOrder, FbxEuler::EOrder));
    props.append(PROP_BOOL("RotationActive", "Rotation", RotationActive));
    props.append(PROP_VEC3("PreRotation", "Rotation", PreRotation));
    props.append(PROP_VEC3("PostRotation", "Rotation", PostRotation));
    props.append(PROP_VEC3("RotationOffset", "Rotation", RotationOffset));
    props.append(PROP_VEC3("RotationPivot", "Rotation", RotationPivot));
    props.append(PROP_VEC3("RotationMin", "Rotation", RotationMin));
    props.append(PROP_VEC3("RotationMax", "Rotation", RotationMax));
    props.append(PROP_BOOL("RotationMinX", "Rotation", RotationMinX));
    props.append(PROP_BOOL("RotationMinY", "Rotation", RotationMinY));
    props.append(PROP_BOOL("RotationMinZ", "Rotation", RotationMinZ));
    props.append(PROP_BOOL("RotationMaxX", "Rotation", RotationMaxX));
    props.append(PROP_BOOL("RotationMaxY", "Rotation", RotationMaxY));
    props.append(PROP_BOOL("RotationMaxZ", "Rotation", RotationMaxZ));
    props.append(PROP_ENUM("QuaternionInterpolate", "Rotation",
                           QuaternionInterpolate, EFbxQuatInterpMode));
    props.append(PROP_BOOL("RotationSpaceForLimitOnly", "Rotation",
                           RotationSpaceForLimitOnly));

    // --- Scaling ---
    props.append(PROP_VEC3("ScalingOffset", "Scaling", ScalingOffset));
    props.append(PROP_VEC3("ScalingPivot", "Scaling", ScalingPivot));
    props.append(PROP_BOOL("ScalingActive", "Scaling", ScalingActive));
    props.append(PROP_VEC3("ScalingMin", "Scaling", ScalingMin));
    props.append(PROP_VEC3("ScalingMax", "Scaling", ScalingMax));
    props.append(PROP_BOOL("ScalingMinX", "Scaling", ScalingMinX));
    props.append(PROP_BOOL("ScalingMinY", "Scaling", ScalingMinY));
    props.append(PROP_BOOL("ScalingMinZ", "Scaling", ScalingMinZ));
    props.append(PROP_BOOL("ScalingMaxX", "Scaling", ScalingMaxX));
    props.append(PROP_BOOL("ScalingMaxY", "Scaling", ScalingMaxY));
    props.append(PROP_BOOL("ScalingMaxZ", "Scaling", ScalingMaxZ));

    // --- Translation ---
    props.append(PROP_BOOL("TranslationActive", "Translation", TranslationActive));
    props.append(PROP_VEC3("TranslationMin", "Translation", TranslationMin));
    props.append(PROP_VEC3("TranslationMax", "Translation", TranslationMax));
    props.append(PROP_BOOL("TranslationMinX", "Translation", TranslationMinX));
    props.append(PROP_BOOL("TranslationMinY", "Translation", TranslationMinY));
    props.append(PROP_BOOL("TranslationMinZ", "Translation", TranslationMinZ));
    props.append(PROP_BOOL("TranslationMaxX", "Translation", TranslationMaxX));
    props.append(PROP_BOOL("TranslationMaxY", "Translation", TranslationMaxY));
    props.append(PROP_BOOL("TranslationMaxZ", "Translation", TranslationMaxZ));

    // --- Geometric Transform ---
    props.append(PROP_VEC3("GeometricTranslation", "Geometry", GeometricTranslation));
    props.append(PROP_VEC3("GeometricRotation", "Geometry", GeometricRotation));
    props.append(PROP_VEC3("GeometricScaling", "Geometry", GeometricScaling));

    // --- Misc ---
    props.append(PROP_ENUM("InheritType", "Misc",
                           InheritType, FbxTransform::EInheritType));
    props.append(PROP_DOUBLE("AxisLen", "Misc", AxisLen));
    props.append(PROP_BOOL("Show", "Misc", Show));
    props.append(PROP_BOOL("NegativePercentShapeSupport", "Misc",
                           NegativePercentShapeSupport));
    props.append(PROP_BOOL("Freeze", "Misc", Freeze));
    props.append(PROP_BOOL("LODBox", "Misc", LODBox));

    return props;
}

QStringList FbxPropertyBridge::rotationOrderLabels()
{
    return {
        QStringLiteral("XYZ"), QStringLiteral("XZY"),
        QStringLiteral("YZX"), QStringLiteral("YXZ"),
        QStringLiteral("ZXY"), QStringLiteral("ZYX"),
        QStringLiteral("SphericXYZ")
    };
}

QStringList FbxPropertyBridge::inheritTypeLabels()
{
    return {
        QStringLiteral("RrSs"), QStringLiteral("RSrs"), QStringLiteral("Rrs")
    };
}

QStringList FbxPropertyBridge::quatInterpLabels()
{
    return {
        QStringLiteral("Off"), QStringLiteral("Classic"),
        QStringLiteral("Slerp"), QStringLiteral("Cubic"),
        QStringLiteral("TangentDependent")
    };
}
