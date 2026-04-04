#pragma once
#include <QString>
#include <QStringList>
#include <fbxsdk.h>

struct FbxSceneData
{
    // Document info
    QString title;
    QString subject;
    QString author;
    QString keywords;
    QString revision;
    QString comment;

    // System units
    double systemUnitScale = 1.0;
    QString systemUnitName;

    // Axis system (read-only display)
    int upAxis = 1;        // 0=X, 1=Y, 2=Z
    int upAxisSign = 1;
    int frontAxis = 2;
    int frontAxisSign = 1;
    int coordAxis = 0;
    int coordAxisSign = 1;

    // Timeline
    int timeMode = 0;
    double customFrameRate = 24.0;

    // Application info (read-only)
    QString originalAppVendor;
    QString originalAppName;
    QString originalAppVersion;
    QString lastSavedAppVendor;
    QString lastSavedAppName;
    QString lastSavedAppVersion;

    static FbxSceneData readFrom(FbxScene* scene);
    static void writeTo(FbxScene* scene, const FbxSceneData& data);

    static QString axisName(int axis);
    static QString timeModeLabel(int mode);
    static QStringList timeModeLabels();
};
