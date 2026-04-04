#pragma once
#include <QDockWidget>
#include <fbxsdk.h>

class QTreeView;
class NodeTreeModel;

class NodeTreePanel : public QDockWidget
{
    Q_OBJECT

public:
    explicit NodeTreePanel(QWidget* parent = nullptr);

    void setScene(FbxScene* scene);

signals:
    void currentNodeChanged(FbxNode* node);

private:
    QTreeView* mTreeView = nullptr;
    NodeTreeModel* mModel = nullptr;

    void onSelectionChanged();
};
