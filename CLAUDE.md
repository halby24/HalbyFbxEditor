# HalbyFbxEditor

FBX SDK を使った FBX ファイルエディタ（Qt6 Widgets）。

## ビルド

### 前提条件
- Visual Studio 2022 (Build Tools でも可)
- FBX SDK（cmake/FindFBXSDK.cmake で検出）
- Qt6 — 以下のいずれかで導入:
  - **方法A**: Qt Online Installer で MSVC 2022 64-bit をインストール
  - **方法B**: vcpkg (`C:/vcpkg`) 経由でマニフェストモード自動取得

### ビルド手順

#### 方法A: Qt Online Installer を使う場合

```bash
# Configure（Qt のパスは環境に合わせて変更）
cmake -B build -S . -DCMAKE_PREFIX_PATH=C:/Qt/6.11.0/msvc2022_64

# Build
cmake --build build --config Debug
cmake --build build --config Release
```

#### 方法B: vcpkg を使う場合

```bash
# Configure（vcpkg.json で Qt6 を自動取得）
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build build --config Debug
cmake --build build --config Release
```

出力先: `build/src/<Config>/HalbyFbxEditor.exe`
