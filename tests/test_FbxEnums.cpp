#include <gtest/gtest.h>
#include "core/FbxEnums.h"

using namespace FbxEnums;

TEST(FbxEnumsTest, NodeTypeNameKnownTypes)
{
    EXPECT_EQ(nodeTypeName(NodeType::Null),     "Null");
    EXPECT_EQ(nodeTypeName(NodeType::Mesh),     "Mesh");
    EXPECT_EQ(nodeTypeName(NodeType::Skeleton), "Skeleton");
    EXPECT_EQ(nodeTypeName(NodeType::Camera),   "Camera");
    EXPECT_EQ(nodeTypeName(NodeType::Light),    "Light");
    EXPECT_EQ(nodeTypeName(NodeType::LODGroup), "LODGroup");
}

TEST(FbxEnumsTest, FromAttributeNull)
{
    // nullptr attribute should map to Null
    EXPECT_EQ(fromAttribute(nullptr), NodeType::Null);
}
