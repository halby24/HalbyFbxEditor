#include <QtTest/QtTest>
#include "model/PropertyTreeModel.h"

class TestPropertyTreeModel : public QObject
{
    Q_OBJECT

private:
    FbxManager* manager = nullptr;
    FbxScene* scene = nullptr;
    FbxNode* testNode = nullptr;

private slots:
    void initTestCase()
    {
        manager = FbxManager::Create();
        QVERIFY(manager);
        scene = FbxScene::Create(manager, "TestScene");
        QVERIFY(scene);

        testNode = FbxNode::Create(manager, "TestNode");
        scene->GetRootNode()->AddChild(testNode);
    }

    void cleanupTestCase()
    {
        if (manager) manager->Destroy();
    }

    void testEmptyModel()
    {
        PropertyTreeModel model;
        QCOMPARE(model.rowCount(), 0);
        QCOMPARE(model.columnCount(), 2);  // Name | Value
    }

    void testSetNode()
    {
        PropertyTreeModel model;
        model.setNode(testNode);
        // Should have at least one category (Transform, etc.)
        QVERIFY(model.rowCount() > 0);
    }

    void testHeaderData()
    {
        PropertyTreeModel model;
        QVariant col0 = model.headerData(0, Qt::Horizontal, Qt::DisplayRole);
        QVariant col1 = model.headerData(1, Qt::Horizontal, Qt::DisplayRole);
        QVERIFY(col0.isValid());
        QVERIFY(col1.isValid());
    }

    void testCategoryHasChildren()
    {
        PropertyTreeModel model;
        model.setNode(testNode);

        // First row should be a category with children
        QModelIndex category = model.index(0, 0);
        QVERIFY(category.isValid());
        QVERIFY(model.rowCount(category) > 0);
    }
};

QTEST_GUILESS_MAIN(TestPropertyTreeModel)
#include "test_PropertyTreeModel.moc"
