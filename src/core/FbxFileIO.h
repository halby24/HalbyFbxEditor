#pragma once
#include <fbxsdk.h>
#include <string>
#include <stdexcept>

class FbxFileIO
{
public:
    static FbxScene* Import(FbxManager* manager, const std::string& path);
    static void Export(FbxManager* manager, FbxScene* scene, const std::string& path);

private:
    static std::string PathToUtf8(const std::string& path);
};
