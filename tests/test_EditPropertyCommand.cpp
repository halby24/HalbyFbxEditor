#include <QtTest/QtTest>
#include <QUndoStack>
#include <memory>
#include <fbxsdk.h>

#include "command/EditPropertyCommand.h"
#include "core/FbxPropertyBridge.h"

/// EditPropertyCommand のライフサイクルテスト。
/// FBX ドキュメントと QUndoStack の破棄順序が安全であることを検証する。
class TestEditPropertyCommand : public QObject
{
    Q_OBJECT

private slots:

    /// 正しい破棄順序の検証:
    ///   UndoStack を先にクリア → FBX ドキュメント破棄
    /// MainWindow::~MainWindow() の修正後の動作に対応。
    void testClearUndoStackBeforeDestroyDocument()
    {
        auto* manager = FbxManager::Create();
        QVERIFY(manager);
        auto* scene = FbxScene::Create(manager, "TestScene");
        auto* node = FbxNode::Create(manager, "TestNode");
        scene->GetRootNode()->AddChild(node);

        auto* undoStack = new QUndoStack(this);

        bool cleanChangedFiredAfterDestroy = false;
        bool documentAlive = true;
        connect(undoStack, &QUndoStack::cleanChanged, this, [&](bool) {
            if (!documentAlive)
                cleanChangedFiredAfterDestroy = true;
        });

        // プロパティ編集コマンドを積む
        auto props = FbxPropertyBridge::buildNodeProperties(node);
        QVERIFY(!props.isEmpty());

        int propIdx = -1;
        for (int i = 0; i < props.size(); ++i) {
            if (props[i].name == QStringLiteral("LclTranslation")) {
                propIdx = i;
                break;
            }
        }
        QVERIFY(propIdx >= 0);
        const auto& prop = props[propIdx];

        QVariant oldVal = prop.getter(node);
        QVariant newVal = QVariant::fromValue(Vec3(1.0, 2.0, 3.0));
        undoStack->push(new EditPropertyCommand(
            node, propIdx, oldVal, newVal, prop.setter, prop.name));

        QVERIFY(!undoStack->isClean());

        // 修正後の正しい順序: UndoStack クリア → FBX 破棄
        undoStack->clear();
        QVERIFY(undoStack->isClean());

        documentAlive = false;
        scene->Destroy();
        manager->Destroy();

        // UndoStack 破棄時に cleanChanged が飛んでも FBX は既にない
        // → clear() 済みなので undo() は呼ばれず安全
        delete undoStack;

        QVERIFY(!cleanChangedFiredAfterDestroy);
    }

    /// undo/redo が有効なうちは正常に動作することを確認
    void testUndoRedoOnLiveNode()
    {
        auto* manager = FbxManager::Create();
        QVERIFY(manager);
        auto* scene = FbxScene::Create(manager, "TestScene");
        auto* node = FbxNode::Create(manager, "TestNode");
        scene->GetRootNode()->AddChild(node);

        QUndoStack undoStack;

        auto props = FbxPropertyBridge::buildNodeProperties(node);
        int propIdx = -1;
        for (int i = 0; i < props.size(); ++i) {
            if (props[i].name == QStringLiteral("Visibility")) {
                propIdx = i;
                break;
            }
        }
        QVERIFY(propIdx >= 0);
        const auto& prop = props[propIdx];

        double original = node->Visibility.Get();
        QVariant oldVal = prop.getter(node);
        QVariant newVal = QVariant::fromValue(0.5);

        undoStack.push(new EditPropertyCommand(
            node, propIdx, oldVal, newVal, prop.setter, prop.name));

        // redo は初回スキップ（setData で既に適用済み前提）なので手動適用
        node->Visibility.Set(0.5);
        QCOMPARE(node->Visibility.Get(), 0.5);

        undoStack.undo();
        QCOMPARE(node->Visibility.Get(), original);

        undoStack.redo();
        QCOMPARE(node->Visibility.Get(), 0.5);

        undoStack.clear();
        manager->Destroy();
    }
};

QTEST_GUILESS_MAIN(TestEditPropertyCommand)
#include "test_EditPropertyCommand.moc"
