# HalbyFbxEditor

[![Build](https://github.com/halby24/HalbyFbxEditor/actions/workflows/build.yml/badge.svg)](https://github.com/halby24/HalbyFbxEditor/actions/workflows/build.yml)

FBX SDK を使った FBX ファイルエディタ。Qt6 Widgets ベースの GUI でノードツリーの閲覧やプロパティの編集ができます。

## 主な機能

- FBX ファイルの読み込み・保存
- ノードツリーの表示・選択
- プロパティ（Translation, Rotation, Scale, ユーザープロパティなど）の編集
- シーン情報の表示・編集
- Undo / Redo 対応

## スクリーンショット

<!-- TODO: スクリーンショットを追加 -->

## ビルド

### 前提条件

- **C++17** 対応コンパイラ
- **CMake** 3.20 以上
- **Qt6** (Widgets)
- **FBX SDK** (cmake/FindFBXSDK.cmake で検出)

#### Windows

- Visual Studio 2025 (v18)
- [vcpkg](https://github.com/microsoft/vcpkg) (`C:/vcpkg`)

#### Linux

- Qt6: `sudo apt-get install qt6-base-dev libgl1-mesa-dev`

### ビルド手順

```bash
# Configure
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build build --config Release
```

FBX SDK を標準外の場所に置いている場合は `-DFBXSDK_ROOT=<path>` を指定してください。

出力先: `build/src/Release/HalbyFbxEditor.exe` (Windows) / `build/src/HalbyFbxEditor` (Linux)

## アーキテクチャ

```
src/
├── core/       # FBX SDK ラッパー (FbxDocument, FbxFileIO, FbxPropertyBridge, FbxSceneData)
├── model/      # Qt Model/View アダプタ (NodeTreeModel, PropertyTreeModel, UserPropertyModel)
├── command/    # Undo/Redo コマンド (EditPropertyCommand, EditSceneInfoCommand)
├── ui/         # Qt Widgets UI (MainWindow, NodeTreePanel, PropertyPanel, etc.)
└── main.cpp
```

## ライセンス

[MIT License](LICENSE)
