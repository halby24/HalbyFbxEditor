#include <gtest/gtest.h>
#include "core/FbxPropertyBridge.h"

TEST(Vec3Test, DefaultConstruction)
{
    Vec3 v;
    EXPECT_DOUBLE_EQ(v.x, 0.0);
    EXPECT_DOUBLE_EQ(v.y, 0.0);
    EXPECT_DOUBLE_EQ(v.z, 0.0);
}

TEST(Vec3Test, ValueConstruction)
{
    Vec3 v(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v.x, 1.0);
    EXPECT_DOUBLE_EQ(v.y, 2.0);
    EXPECT_DOUBLE_EQ(v.z, 3.0);
}

TEST(Vec3Test, FbxDouble3RoundTrip)
{
    FbxDouble3 fbx(4.0, 5.0, 6.0);
    Vec3 v(fbx);
    FbxDouble3 back = v.toFbxDouble3();
    EXPECT_DOUBLE_EQ(back[0], 4.0);
    EXPECT_DOUBLE_EQ(back[1], 5.0);
    EXPECT_DOUBLE_EQ(back[2], 6.0);
}

TEST(Vec3Test, Equality)
{
    Vec3 a(1.0, 2.0, 3.0);
    Vec3 b(1.0, 2.0, 3.0);
    Vec3 c(1.0, 2.0, 4.0);
    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
}

TEST(Vec3Test, QVariantStorage)
{
    Vec3 original(7.0, 8.0, 9.0);
    QVariant var = QVariant::fromValue(original);
    EXPECT_TRUE(var.canConvert<Vec3>());

    Vec3 restored = var.value<Vec3>();
    EXPECT_EQ(original, restored);
}
