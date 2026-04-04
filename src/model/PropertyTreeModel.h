#pragma once
#include <QAbstractItemModel>
#include <QVector>
#include "core/FbxPropertyBridge.h"

class PropertyTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit PropertyTreeModel(QObject* parent = nullptr);

    void setNode(FbxNode* node);

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex& parent = {}) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

signals:
    void propertyChanged(int propertyIndex, const QVariant& oldValue, const QVariant& newValue);

private:
    struct Category
    {
        QString name;
        QVector<int> propertyIndices;  // indices into mProperties
    };

    FbxNode* mNode = nullptr;
    QVector<PropertyDescriptor> mProperties;
    QVector<Category> mCategories;

    void rebuildCategories();

    // Internal pointer encoding:
    // Category rows: internalId = category index + 1 (shifted to avoid 0)
    // Property rows: internalId = (category index + 1) << 16 | property-within-category index
    static constexpr quintptr kCategoryFlag = 0x80000000;
    bool isCategoryIndex(const QModelIndex& index) const;
    int categoryOfProperty(const QModelIndex& index) const;
    int propertyLocalRow(const QModelIndex& index) const;
};
