#include "PropertyTreeModel.h"

PropertyTreeModel::PropertyTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

void PropertyTreeModel::setNode(FbxNode* node)
{
    beginResetModel();
    mNode = node;
    mProperties = FbxPropertyBridge::buildNodeProperties(node);
    rebuildCategories();
    endResetModel();
}

void PropertyTreeModel::rebuildCategories()
{
    mCategories.clear();
    QMap<QString, int> catMap;

    for (int i = 0; i < mProperties.size(); ++i)
    {
        const QString& cat = mProperties[i].category;
        if (!catMap.contains(cat))
        {
            catMap[cat] = mCategories.size();
            mCategories.append({cat, {}});
        }
        mCategories[catMap[cat]].propertyIndices.append(i);
    }
}

bool PropertyTreeModel::isCategoryIndex(const QModelIndex& index) const
{
    return index.isValid() && (index.internalId() & kCategoryFlag);
}

int PropertyTreeModel::categoryOfProperty(const QModelIndex& index) const
{
    return (int)((index.internalId() >> 16) & 0x7FFF);
}

int PropertyTreeModel::propertyLocalRow(const QModelIndex& index) const
{
    return (int)(index.internalId() & 0xFFFF);
}

QModelIndex PropertyTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (column < 0 || column > 1) return {};

    if (!parent.isValid())
    {
        // Top-level = categories
        if (row < 0 || row >= mCategories.size()) return {};
        quintptr id = kCategoryFlag | (quintptr)row;
        return createIndex(row, column, id);
    }

    if (isCategoryIndex(parent))
    {
        // Child of category = property
        int catIdx = parent.row();
        if (catIdx < 0 || catIdx >= mCategories.size()) return {};
        if (row < 0 || row >= mCategories[catIdx].propertyIndices.size()) return {};
        quintptr id = ((quintptr)(catIdx) << 16) | (quintptr)row;
        return createIndex(row, column, id);
    }

    return {};
}

QModelIndex PropertyTreeModel::parent(const QModelIndex& child) const
{
    if (!child.isValid()) return {};

    if (isCategoryIndex(child))
        return {};  // Categories are top-level

    // Property -> parent is category
    int catIdx = categoryOfProperty(child);
    quintptr id = kCategoryFlag | (quintptr)catIdx;
    return createIndex(catIdx, 0, id);
}

int PropertyTreeModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return mCategories.size();

    if (isCategoryIndex(parent))
    {
        int catIdx = parent.row();
        if (catIdx >= 0 && catIdx < mCategories.size())
            return mCategories[catIdx].propertyIndices.size();
    }

    return 0;
}

int PropertyTreeModel::columnCount(const QModelIndex&) const
{
    return 2;  // Name | Value
}

QVariant PropertyTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !mNode) return {};

    if (isCategoryIndex(index))
    {
        if (role == Qt::DisplayRole && index.column() == 0)
            return mCategories[index.row()].name;
        // Qt::UserRole+3 indicates this is a category row (for delegate styling)
        if (role == Qt::UserRole + 3)
            return true;
        return {};
    }

    // Property row
    int catIdx = categoryOfProperty(index);
    int localRow = propertyLocalRow(index);
    if (catIdx < 0 || catIdx >= mCategories.size()) return {};
    if (localRow < 0 || localRow >= mCategories[catIdx].propertyIndices.size()) return {};

    int propIdx = mCategories[catIdx].propertyIndices[localRow];
    const PropertyDescriptor& prop = mProperties[propIdx];

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == 0)
            return prop.name;
        if (index.column() == 1 && prop.getter)
        {
            QVariant val = prop.getter(mNode);

            // For display role, format Vec3 and enums nicely
            if (role == Qt::DisplayRole)
            {
                if (val.canConvert<Vec3>())
                {
                    Vec3 v = val.value<Vec3>();
                    return QStringLiteral("%1, %2, %3")
                        .arg(v.x, 0, 'f', 4)
                        .arg(v.y, 0, 'f', 4)
                        .arg(v.z, 0, 'f', 4);
                }
                if (prop.name == QStringLiteral("RotationOrder"))
                {
                    auto labels = FbxPropertyBridge::rotationOrderLabels();
                    int idx = val.toInt();
                    if (idx >= 0 && idx < labels.size()) return labels[idx];
                }
                if (prop.name == QStringLiteral("InheritType"))
                {
                    auto labels = FbxPropertyBridge::inheritTypeLabels();
                    int idx = val.toInt();
                    if (idx >= 0 && idx < labels.size()) return labels[idx];
                }
            }
            return val;
        }
    }

    // Store metatype and property index for delegate
    if (role == Qt::UserRole)
        return prop.metatype;
    if (role == Qt::UserRole + 1)
        return propIdx;
    if (role == Qt::UserRole + 2)
        return prop.readOnly;

    return {};
}

bool PropertyTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role != Qt::EditRole || !index.isValid() || !mNode) return false;
    if (index.column() != 1) return false;
    if (isCategoryIndex(index)) return false;

    int catIdx = categoryOfProperty(index);
    int localRow = propertyLocalRow(index);
    if (catIdx < 0 || catIdx >= mCategories.size()) return false;
    if (localRow < 0 || localRow >= mCategories[catIdx].propertyIndices.size()) return false;

    int propIdx = mCategories[catIdx].propertyIndices[localRow];
    PropertyDescriptor& prop = mProperties[propIdx];

    if (prop.readOnly || !prop.setter || !prop.getter) return false;

    QVariant oldValue = prop.getter(mNode);
    if (oldValue == value) return false;

    prop.setter(mNode, value);
    emit dataChanged(index, index, {role});
    emit propertyChanged(propIdx, oldValue, value);
    return true;
}

Qt::ItemFlags PropertyTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (!isCategoryIndex(index) && index.column() == 1)
    {
        int catIdx = categoryOfProperty(index);
        int localRow = propertyLocalRow(index);
        if (catIdx >= 0 && catIdx < mCategories.size() &&
            localRow >= 0 && localRow < mCategories[catIdx].propertyIndices.size())
        {
            int propIdx = mCategories[catIdx].propertyIndices[localRow];
            if (!mProperties[propIdx].readOnly)
                f |= Qt::ItemIsEditable;
        }
    }

    return f;
}

QVariant PropertyTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return {};
    if (section == 0) return tr("Property");
    if (section == 1) return tr("Value");
    return {};
}
