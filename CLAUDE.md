# HalbyFbxEditor

FBX SDK を使った FBX ファイルエディタ（Qt6 Widgets）。

## ビルド

### 前提条件
- Visual Studio 18 (2025)
- vcpkg (`C:/vcpkg`)
- Qt6 (vcpkg経由: `vcpkg install qt6:x64-windows`)
- FBX SDK（cmake/FindFBXSDK.cmake で検出）

### ビルド手順

```bash
# CMake は VS 同梱版を使用
CMAKE="C:/Program Files/Microsoft Visual Studio/18/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"

# Configure（初回のみ）
"$CMAKE" -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build
"$CMAKE" --build build --config Debug
"$CMAKE" --build build --config Release
```

出力先: `build/src/<Config>/HalbyFbxEditor.exe`
