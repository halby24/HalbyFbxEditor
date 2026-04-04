#include "NodeTreePanel.h"
#include "model/NodeTreeModel.h"

#include <QTreeView>
#include <QVBoxLayout>
#include <QHeaderView>

NodeTreePanel::NodeTreePanel(QWidget* parent)
    : QDockWidget(tr("Node Tree"), parent)
{
    setObjectName(QStringLiteral("NodeTreePanel"));

    auto* widget = new QWidget(this);
    auto* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    mModel = new NodeTreeModel(this);

    mTreeView = new QTreeView(widget);
    mTreeView->setModel(mModel);
    mTreeView->setHeaderHidden(true);
    mTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
    mTreeView->setAnimated(true);

    layout->addWidget(mTreeView);
    setWidget(widget);

    connect(mTreeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, [this]() { onSelectionChanged(); });
}

void NodeTreePanel::setScene(FbxScene* scene)
{
    mModel->setScene(scene);
    if (scene)
        mTreeView->expandToDepth(1);

    // Reconnect selection model (reset by setScene)
    disconnect(mTreeView->selectionModel(), nullptr, this, nullptr);
    connect(mTreeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, [this]() { onSelectionChanged(); });
}

void NodeTreePanel::onSelectionChanged()
{
    QModelIndex idx = mTreeView->currentIndex();
    FbxNode* node = mModel->nodeFromIndex(idx);
    emit currentNodeChanged(node);
}
