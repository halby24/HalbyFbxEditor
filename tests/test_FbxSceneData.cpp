#include <gtest/gtest.h>
#include "core/FbxSceneData.h"

// --- Static helper tests (no FBX scene needed) ---

TEST(FbxSceneDataTest, AxisNameMapping)
{
    // Standard axis values: 0=X, 1=Y, 2=Z
    EXPECT_FALSE(FbxSceneData::axisName(0).isEmpty());
    EXPECT_FALSE(FbxSceneData::axisName(1).isEmpty());
    EXPECT_FALSE(FbxSceneData::axisName(2).isEmpty());
}

TEST(FbxSceneDataTest, TimeModeLabelsNotEmpty)
{
    QStringList labels = FbxSceneData::timeModeLabels();
    EXPECT_FALSE(labels.isEmpty());
}

// --- Read/Write round-trip with programmatic FBX scene ---

class FbxSceneDataRoundTrip : public ::testing::Test
{
protected:
    FbxManager* manager = nullptr;
    FbxScene* scene = nullptr;

    void SetUp() override
    {
        manager = FbxManager::Create();
        ASSERT_NE(manager, nullptr);
        scene = FbxScene::Create(manager, "TestScene");
        ASSERT_NE(scene, nullptr);
    }

    void TearDown() override
    {
        if (manager) manager->Destroy();
    }
};

TEST_F(FbxSceneDataRoundTrip, WriteAndReadBack)
{
    FbxSceneData data;
    data.title = "Test Title";
    data.author = "Test Author";
    data.comment = "Test Comment";

    FbxSceneData::writeTo(scene, data);
    FbxSceneData readBack = FbxSceneData::readFrom(scene);

    EXPECT_EQ(readBack.title, "Test Title");
    EXPECT_EQ(readBack.author, "Test Author");
    EXPECT_EQ(readBack.comment, "Test Comment");
}
