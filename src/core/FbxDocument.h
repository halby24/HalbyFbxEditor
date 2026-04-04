#pragma once
#include <fbxsdk.h>
#include <QString>
#include <string>

class FbxDocumentManager
{
public:
    FbxDocumentManager();
    ~FbxDocumentManager();

    FbxDocumentManager(const FbxDocumentManager&) = delete;
    FbxDocumentManager& operator=(const FbxDocumentManager&) = delete;

    bool open(const QString& path);
    bool save();
    bool saveAs(const QString& path);
    void close();

    bool isOpen() const { return mScene != nullptr; }
    bool isModified() const { return mModified; }
    void setModified(bool modified) { mModified = modified; }

    FbxScene* scene() const { return mScene; }
    FbxManager* manager() const { return mManager; }
    QString filePath() const { return mFilePath; }

    int nodeCount() const;
    QString lastError() const { return mLastError; }

private:
    FbxManager* mManager = nullptr;
    FbxScene* mScene = nullptr;
    QString mFilePath;
    QString mLastError;
    bool mModified = false;

    void initManager();
    void destroyAll();
};
