#pragma once
#include <QString>
#include <fbxsdk.h>

namespace FbxEnums
{

enum class NodeType
{
    Unknown, Null, Marker, Skeleton, Mesh, Nurbs, Patch,
    Camera, CameraStereo, CameraSwitcher, Light,
    OpticalReference, OpticalMarker,
    NurbsCurve, TrimNurbsSurface, Boundary, NurbsSurface,
    Shape, LODGroup, SubDiv, CachedEffect, Line
};

inline NodeType fromAttribute(const FbxNodeAttribute* attr)
{
    if (!attr) return NodeType::Null;
    switch (attr->GetAttributeType())
    {
    case FbxNodeAttribute::eUnknown:           return NodeType::Unknown;
    case FbxNodeAttribute::eNull:              return NodeType::Null;
    case FbxNodeAttribute::eMarker:            return NodeType::Marker;
    case FbxNodeAttribute::eSkeleton:          return NodeType::Skeleton;
    case FbxNodeAttribute::eMesh:              return NodeType::Mesh;
    case FbxNodeAttribute::eNurbs:             return NodeType::Nurbs;
    case FbxNodeAttribute::ePatch:             return NodeType::Patch;
    case FbxNodeAttribute::eCamera:            return NodeType::Camera;
    case FbxNodeAttribute::eCameraStereo:      return NodeType::CameraStereo;
    case FbxNodeAttribute::eCameraSwitcher:    return NodeType::CameraSwitcher;
    case FbxNodeAttribute::eLight:             return NodeType::Light;
    case FbxNodeAttribute::eOpticalReference:  return NodeType::OpticalReference;
    case FbxNodeAttribute::eOpticalMarker:     return NodeType::OpticalMarker;
    case FbxNodeAttribute::eNurbsCurve:        return NodeType::NurbsCurve;
    case FbxNodeAttribute::eTrimNurbsSurface:  return NodeType::TrimNurbsSurface;
    case FbxNodeAttribute::eBoundary:          return NodeType::Boundary;
    case FbxNodeAttribute::eNurbsSurface:      return NodeType::NurbsSurface;
    case FbxNodeAttribute::eShape:             return NodeType::Shape;
    case FbxNodeAttribute::eLODGroup:          return NodeType::LODGroup;
    case FbxNodeAttribute::eSubDiv:            return NodeType::SubDiv;
    case FbxNodeAttribute::eCachedEffect:      return NodeType::CachedEffect;
    case FbxNodeAttribute::eLine:              return NodeType::Line;
    default:                                   return NodeType::Unknown;
    }
}

inline QString nodeTypeName(NodeType type)
{
    switch (type)
    {
    case NodeType::Unknown:          return QStringLiteral("Unknown");
    case NodeType::Null:             return QStringLiteral("Null");
    case NodeType::Marker:           return QStringLiteral("Marker");
    case NodeType::Skeleton:         return QStringLiteral("Skeleton");
    case NodeType::Mesh:             return QStringLiteral("Mesh");
    case NodeType::Nurbs:            return QStringLiteral("Nurbs");
    case NodeType::Patch:            return QStringLiteral("Patch");
    case NodeType::Camera:           return QStringLiteral("Camera");
    case NodeType::CameraStereo:     return QStringLiteral("CameraStereo");
    case NodeType::CameraSwitcher:   return QStringLiteral("CameraSwitcher");
    case NodeType::Light:            return QStringLiteral("Light");
    case NodeType::OpticalReference: return QStringLiteral("OpticalReference");
    case NodeType::OpticalMarker:    return QStringLiteral("OpticalMarker");
    case NodeType::NurbsCurve:       return QStringLiteral("NurbsCurve");
    case NodeType::TrimNurbsSurface: return QStringLiteral("TrimNurbsSurface");
    case NodeType::Boundary:         return QStringLiteral("Boundary");
    case NodeType::NurbsSurface:     return QStringLiteral("NurbsSurface");
    case NodeType::Shape:            return QStringLiteral("Shape");
    case NodeType::LODGroup:         return QStringLiteral("LODGroup");
    case NodeType::SubDiv:           return QStringLiteral("SubDiv");
    case NodeType::CachedEffect:     return QStringLiteral("CachedEffect");
    case NodeType::Line:             return QStringLiteral("Line");
    default:                         return QStringLiteral("Unknown");
    }
}

} // namespace FbxEnums
