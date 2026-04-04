#include "FbxSceneData.h"

FbxSceneData FbxSceneData::readFrom(FbxScene* scene)
{
    FbxSceneData d;
    if (!scene) return d;

    // Document info
    FbxDocumentInfo* info = scene->GetDocumentInfo();
    if (info)
    {
        d.title    = QString::fromUtf8(info->mTitle.Buffer());
        d.subject  = QString::fromUtf8(info->mSubject.Buffer());
        d.author   = QString::fromUtf8(info->mAuthor.Buffer());
        d.keywords = QString::fromUtf8(info->mKeywords.Buffer());
        d.revision = QString::fromUtf8(info->mRevision.Buffer());
        d.comment  = QString::fromUtf8(info->mComment.Buffer());

        d.originalAppVendor  = QString::fromUtf8(info->Original_ApplicationVendor.Get().Buffer());
        d.originalAppName    = QString::fromUtf8(info->Original_ApplicationName.Get().Buffer());
        d.originalAppVersion = QString::fromUtf8(info->Original_ApplicationVersion.Get().Buffer());
        d.lastSavedAppVendor  = QString::fromUtf8(info->LastSaved_ApplicationVendor.Get().Buffer());
        d.lastSavedAppName    = QString::fromUtf8(info->LastSaved_ApplicationName.Get().Buffer());
        d.lastSavedAppVersion = QString::fromUtf8(info->LastSaved_ApplicationVersion.Get().Buffer());
    }

    // Global settings
    FbxGlobalSettings& gs = scene->GetGlobalSettings();

    // System units
    FbxSystemUnit unit = gs.GetSystemUnit();
    d.systemUnitScale = unit.GetScaleFactor();
    d.systemUnitName  = QString::fromUtf8(unit.GetScaleFactorAsString().Buffer());

    // Axis system
    FbxAxisSystem axis = gs.GetAxisSystem();
    int upSign = 1;
    FbxAxisSystem::EUpVector upVec = axis.GetUpVector(upSign);
    d.upAxis = (int)upVec - 1;
    d.upAxisSign = upSign;

    int frontSign = 1;
    FbxAxisSystem::EFrontVector frontVec = axis.GetFrontVector(frontSign);
    // Compute actual front axis from parity
    int upIdx = d.upAxis;
    int minRemaining = (upIdx == 0) ? 1 : 0;
    int maxRemaining = (upIdx == 2) ? 1 : 2;
    d.frontAxis = (frontVec == FbxAxisSystem::eParityEven) ? minRemaining : maxRemaining;
    d.frontAxisSign = frontSign;

    // Coord (cross product of up x front)
    int coordIdx = 3 - upIdx - d.frontAxis;
    d.coordAxis = coordIdx;
    d.coordAxisSign = 1;

    // Timeline
    d.timeMode = (int)gs.GetTimeMode();
    d.customFrameRate = gs.GetCustomFrameRate();

    return d;
}

void FbxSceneData::writeTo(FbxScene* scene, const FbxSceneData& data)
{
    if (!scene) return;

    // Document info
    FbxDocumentInfo* info = scene->GetDocumentInfo();
    if (!info)
    {
        info = FbxDocumentInfo::Create(scene->GetFbxManager(), "DocInfo");
        scene->SetDocumentInfo(info);
    }

    info->mTitle    = data.title.toUtf8().constData();
    info->mSubject  = data.subject.toUtf8().constData();
    info->mAuthor   = data.author.toUtf8().constData();
    info->mKeywords = data.keywords.toUtf8().constData();
    info->mRevision = data.revision.toUtf8().constData();
    info->mComment  = data.comment.toUtf8().constData();

    // Timeline
    FbxGlobalSettings& gs = scene->GetGlobalSettings();
    gs.SetTimeMode((FbxTime::EMode)data.timeMode);
    if (data.timeMode == (int)FbxTime::eCustom)
        gs.SetCustomFrameRate(data.customFrameRate);
}

QString FbxSceneData::axisName(int axis)
{
    switch (axis)
    {
    case 0: return QStringLiteral("X");
    case 1: return QStringLiteral("Y");
    case 2: return QStringLiteral("Z");
    default: return QStringLiteral("?");
    }
}

QString FbxSceneData::timeModeLabel(int mode)
{
    switch ((FbxTime::EMode)mode)
    {
    case FbxTime::eDefaultMode:  return QStringLiteral("Default");
    case FbxTime::eFrames120:    return QStringLiteral("120 fps");
    case FbxTime::eFrames100:    return QStringLiteral("100 fps");
    case FbxTime::eFrames60:     return QStringLiteral("60 fps");
    case FbxTime::eFrames50:     return QStringLiteral("50 fps");
    case FbxTime::eFrames48:     return QStringLiteral("48 fps");
    case FbxTime::eFrames30:     return QStringLiteral("30 fps");
    case FbxTime::eFrames30Drop: return QStringLiteral("30 fps (Drop)");
    case FbxTime::eNTSCDropFrame:return QStringLiteral("NTSC Drop");
    case FbxTime::eNTSCFullFrame:return QStringLiteral("NTSC Full");
    case FbxTime::ePAL:          return QStringLiteral("PAL (25 fps)");
    case FbxTime::eFrames24:     return QStringLiteral("24 fps");
    case FbxTime::eFrames1000:   return QStringLiteral("1000 fps");
    case FbxTime::eFilmFullFrame:return QStringLiteral("Film Full Frame");
    case FbxTime::eCustom:       return QStringLiteral("Custom");
    case FbxTime::eFrames96:     return QStringLiteral("96 fps");
    case FbxTime::eFrames72:     return QStringLiteral("72 fps");
    case FbxTime::eFrames59dot94:return QStringLiteral("59.94 fps");
    default:                     return QStringLiteral("Unknown (%1)").arg(mode);
    }
}

QStringList FbxSceneData::timeModeLabels()
{
    QStringList labels;
    // Add in order of FbxTime::EMode enum values
    for (int i = 0; i <= (int)FbxTime::eFrames59dot94; ++i)
        labels.append(timeModeLabel(i));
    return labels;
}
