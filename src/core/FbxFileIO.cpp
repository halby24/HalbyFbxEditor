#include "FbxFileIO.h"

FbxScene* FbxFileIO::Import(FbxManager* manager, const std::string& path)
{
    FbxScene* scene = FbxScene::Create(manager, "Scene");
    FbxImporter* importer = FbxImporter::Create(manager, "");
    if (!importer->Initialize(path.c_str(), -1, manager->GetIOSettings()))
    {
        std::string err = importer->GetStatus().GetErrorString();
        importer->Destroy();
        scene->Destroy();
        throw std::runtime_error(
            std::string("Failed to open input file: ") + path + "\n" + err);
    }
    importer->Import(scene);
    importer->Destroy();
    return scene;
}

void FbxFileIO::Export(FbxManager* manager, FbxScene* scene, const std::string& path)
{
    FbxExporter* exporter = FbxExporter::Create(manager, "");
    if (!exporter->Initialize(path.c_str(), -1, manager->GetIOSettings()))
    {
        exporter->Destroy();
        throw std::runtime_error(
            std::string("Failed to open output file: ") + path);
    }
    exporter->Export(scene);
    exporter->Destroy();
}

