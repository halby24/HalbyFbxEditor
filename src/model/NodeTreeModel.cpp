#include "NodeTreeModel.h"
#include "core/FbxEnums.h"

NodeTreeModel::NodeTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

void NodeTreeModel::setScene(FbxScene* scene)
{
    beginResetModel();
    mScene = scene;
    endResetModel();
}

FbxNode* NodeTreeModel::rootNode() const
{
    return mScene ? mScene->GetRootNode() : nullptr;
}

FbxNode* NodeTreeModel::nodeFromIndex(const QModelIndex& index) const
{
    if (!index.isValid()) return rootNode();
    return static_cast<FbxNode*>(index.internalPointer());
}

int NodeTreeModel::childRow(FbxNode* node) const
{
    if (!node) return 0;
    FbxNode* parent = node->GetParent();
    if (!parent) return 0;
    for (int i = 0; i < parent->GetChildCount(); ++i)
    {
        if (parent->GetChild(i) == node)
            return i;
    }
    return 0;
}

QModelIndex NodeTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!mScene || column != 0) return {};

    FbxNode* parentNode = nodeFromIndex(parent);
    if (!parentNode || row < 0 || row >= parentNode->GetChildCount())
        return {};

    FbxNode* child = parentNode->GetChild(row);
    return createIndex(row, column, child);
}

QModelIndex NodeTreeModel::parent(const QModelIndex& child) const
{
    if (!child.isValid()) return {};

    FbxNode* node = static_cast<FbxNode*>(child.internalPointer());
    FbxNode* parentNode = node->GetParent();

    if (!parentNode || parentNode == rootNode())
        return {};

    return createIndex(childRow(parentNode), 0, parentNode);
}

int NodeTreeModel::rowCount(const QModelIndex& parent) const
{
    FbxNode* node = nodeFromIndex(parent);
    return node ? node->GetChildCount() : 0;
}

int NodeTreeModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant NodeTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return {};

    FbxNode* node = static_cast<FbxNode*>(index.internalPointer());
    if (!node) return {};

    switch (role)
    {
    case Qt::DisplayRole:
    {
        QString name = QString::fromUtf8(node->GetName());
        auto type = FbxEnums::fromAttribute(node->GetNodeAttribute());
        QString typeName = FbxEnums::nodeTypeName(type);
        return QStringLiteral("%1 [%2]").arg(name, typeName);
    }
    case Qt::ToolTipRole:
    {
        QString name = QString::fromUtf8(node->GetName());
        int childCount = node->GetChildCount();
        return QStringLiteral("%1 (%2 children)").arg(name).arg(childCount);
    }
    default:
        return {};
    }
}

Qt::ItemFlags NodeTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
