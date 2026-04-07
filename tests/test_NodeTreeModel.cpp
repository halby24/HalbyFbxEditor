#include <QtTest/QtTest>
#include "model/NodeTreeModel.h"

class TestNodeTreeModel : public QObject
{
    Q_OBJECT

private:
    FbxManager* manager = nullptr;
    FbxScene* scene = nullptr;

private slots:
    void initTestCase()
    {
        manager = FbxManager::Create();
        QVERIFY(manager);
        scene = FbxScene::Create(manager, "TestScene");
        QVERIFY(scene);

        // Build a small node hierarchy:
        //   RootNode
        //     +-- Child_A
        //     |     +-- Grandchild
        //     +-- Child_B
        FbxNode* root = scene->GetRootNode();
        FbxNode* childA = FbxNode::Create(manager, "Child_A");
        FbxNode* childB = FbxNode::Create(manager, "Child_B");
        FbxNode* grandchild = FbxNode::Create(manager, "Grandchild");
        root->AddChild(childA);
        root->AddChild(childB);
        childA->AddChild(grandchild);
    }

    void cleanupTestCase()
    {
        if (manager) manager->Destroy();
    }

    void testEmptyModel()
    {
        NodeTreeModel model;
        QCOMPARE(model.rowCount(), 0);
    }

    void testRowCountAfterSetScene()
    {
        NodeTreeModel model;
        model.setScene(scene);
        // Root node has 2 children
        QCOMPARE(model.rowCount(), 2);
    }

    void testChildNode()
    {
        NodeTreeModel model;
        model.setScene(scene);

        QModelIndex childA = model.index(0, 0);
        QVERIFY(childA.isValid());
        // DisplayRole format: "Name [Type]"
        QString display = model.data(childA, Qt::DisplayRole).toString();
        QVERIFY(display.startsWith("Child_A"));

        // Child_A has 1 child
        QCOMPARE(model.rowCount(childA), 1);

        QModelIndex grandchild = model.index(0, 0, childA);
        QVERIFY(grandchild.isValid());
        QString gcDisplay = model.data(grandchild, Qt::DisplayRole).toString();
        QVERIFY(gcDisplay.startsWith("Grandchild"));
    }

    void testNodeFromIndex()
    {
        NodeTreeModel model;
        model.setScene(scene);

        QModelIndex childB = model.index(1, 0);
        FbxNode* node = model.nodeFromIndex(childB);
        QVERIFY(node);
        QCOMPARE(QString(node->GetName()), QString("Child_B"));
    }

    void testInvalidIndexReturnsRoot()
    {
        NodeTreeModel model;
        model.setScene(scene);
        // Invalid QModelIndex maps to root node (by design)
        FbxNode* node = model.nodeFromIndex(QModelIndex());
        QVERIFY(node);
        QCOMPARE(node, scene->GetRootNode());
    }
};

QTEST_GUILESS_MAIN(TestNodeTreeModel)
#include "test_NodeTreeModel.moc"
