#include "FbxDocument.h"
#include "FbxFileIO.h"

FbxDocumentManager::FbxDocumentManager() = default;

FbxDocumentManager::~FbxDocumentManager()
{
    destroyAll();
}

void FbxDocumentManager::initManager()
{
    if (mManager) return;
    mManager = FbxManager::Create();
    if (!mManager)
        throw std::runtime_error("Failed to create FbxManager.");
    FbxIOSettings* ios = FbxIOSettings::Create(mManager, IOSROOT);
    mManager->SetIOSettings(ios);
}

void FbxDocumentManager::destroyAll()
{
    if (mScene)   { mScene->Destroy();   mScene = nullptr; }
    if (mManager) { mManager->Destroy(); mManager = nullptr; }
    mFilePath.clear();
    mModified = false;
    mLastError.clear();
}

bool FbxDocumentManager::open(const QString& path)
{
    close();
    try
    {
        initManager();
        QByteArray utf8 = path.toUtf8();
        mScene = FbxFileIO::Import(mManager, std::string(utf8.constData(), utf8.size()));
        mFilePath = path;
        mModified = false;
        return true;
    }
    catch (const std::exception& e)
    {
        mLastError = QString::fromUtf8(e.what());
        destroyAll();
        return false;
    }
}

bool FbxDocumentManager::save()
{
    return saveAs(mFilePath);
}

bool FbxDocumentManager::saveAs(const QString& path)
{
    if (!mScene || !mManager)
    {
        mLastError = QStringLiteral("No scene to save.");
        return false;
    }
    try
    {
        QByteArray utf8 = path.toUtf8();
        FbxFileIO::Export(mManager, mScene, std::string(utf8.constData(), utf8.size()));
        mFilePath = path;
        mModified = false;
        return true;
    }
    catch (const std::exception& e)
    {
        mLastError = QString::fromUtf8(e.what());
        return false;
    }
}

void FbxDocumentManager::close()
{
    destroyAll();
}

int FbxDocumentManager::nodeCount() const
{
    if (!mScene) return 0;
    return mScene->GetNodeCount();
}
