#pragma once
#include <QAbstractItemModel>
#include <fbxsdk.h>

class NodeTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit NodeTreeModel(QObject* parent = nullptr);

    void setScene(FbxScene* scene);
    FbxNode* nodeFromIndex(const QModelIndex& index) const;

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex& parent = {}) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    FbxScene* mScene = nullptr;

    FbxNode* rootNode() const;
    int childRow(FbxNode* node) const;
};
