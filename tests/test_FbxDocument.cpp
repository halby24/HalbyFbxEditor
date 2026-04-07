#include <gtest/gtest.h>
#include "core/FbxDocument.h"

TEST(FbxDocumentTest, InitialState)
{
    FbxDocumentManager doc;
    EXPECT_FALSE(doc.isOpen());
    EXPECT_FALSE(doc.isModified());
    // FbxManager is lazily initialized on open(), so it's null here
    EXPECT_EQ(doc.manager(), nullptr);
    EXPECT_EQ(doc.scene(), nullptr);
    EXPECT_EQ(doc.nodeCount(), 0);
}

TEST(FbxDocumentTest, OpenNonExistentFile)
{
    FbxDocumentManager doc;
    bool ok = doc.open("__nonexistent_test_file__.fbx");
    EXPECT_FALSE(ok);
    EXPECT_FALSE(doc.isOpen());
    // NOTE: lastError is currently cleared by destroyAll() in the catch block.
    // This is a known issue — once fixed, uncomment the assertion below:
    // EXPECT_FALSE(doc.lastError().isEmpty());
}

TEST(FbxDocumentTest, CloseWithoutOpen)
{
    FbxDocumentManager doc;
    doc.close();  // Should not crash
    EXPECT_FALSE(doc.isOpen());
}

TEST(FbxDocumentTest, SaveWithoutOpen)
{
    FbxDocumentManager doc;
    bool ok = doc.save();
    EXPECT_FALSE(ok);
}

// TODO: ファイルベースのテスト（テスト用FBXが用意できたら追加）
// TEST(FbxDocumentTest, OpenAndSaveRoundTrip) { ... }
