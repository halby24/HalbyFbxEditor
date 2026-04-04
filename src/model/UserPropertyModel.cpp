#include "UserPropertyModel.h"

UserPropertyModel::UserPropertyModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

void UserPropertyModel::setNode(FbxNode* node)
{
    beginResetModel();
    mNode = node;
    refresh();
    endResetModel();
}

void UserPropertyModel::refresh()
{
    mEntries.clear();
    if (!mNode) return;

    FbxProperty prop = mNode->GetFirstProperty();
    while (prop.IsValid())
    {
        if (prop.GetFlag(FbxPropertyFlags::eUserDefined))
        {
            PropEntry entry;
            entry.name = QString::fromUtf8(prop.GetName().Buffer());
            entry.prop = prop;
            mEntries.append(entry);
        }
        prop = mNode->GetNextProperty(prop);
    }
}

int UserPropertyModel::rowCount(const QModelIndex&) const
{
    return mEntries.size();
}

int UserPropertyModel::columnCount(const QModelIndex&) const
{
    return 3;  // Name | Type | Value
}

QVariant UserPropertyModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= mEntries.size())
        return {};

    const PropEntry& entry = mEntries[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
        case 0: return entry.name;
        case 1: return fbxTypeName(entry.prop.GetPropertyDataType().GetType());
        case 2: return readProperty(entry.prop);
        }
    }

    return {};
}

bool UserPropertyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role != Qt::EditRole || !index.isValid() || index.row() >= mEntries.size())
        return false;

    if (index.column() == 2)
    {
        PropEntry& entry = mEntries[index.row()];
        writeProperty(entry.prop, value);
        emit dataChanged(index, index, {role});
        emit userPropertyChanged();
        return true;
    }

    return false;
}

Qt::ItemFlags UserPropertyModel::flags(const QModelIndex& index) const
{
    auto f = QAbstractTableModel::flags(index);
    if (index.isValid() && index.column() == 2)
        f |= Qt::ItemIsEditable;
    return f;
}

QVariant UserPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return {};
    switch (section)
    {
    case 0: return tr("Name");
    case 1: return tr("Type");
    case 2: return tr("Value");
    }
    return {};
}

bool UserPropertyModel::addProperty(const QString& name, int fbxType)
{
    if (!mNode || name.isEmpty()) return false;

    FbxProperty existing = mNode->FindProperty(name.toUtf8().constData());
    if (existing.IsValid()) return false;

    FbxDataType dt;
    switch ((EFbxType)fbxType)
    {
    case eFbxBool:   dt = FbxBoolDT;   break;
    case eFbxInt:    dt = FbxIntDT;    break;
    case eFbxFloat:  dt = FbxFloatDT;  break;
    case eFbxDouble: dt = FbxDoubleDT; break;
    case eFbxString: dt = FbxStringDT; break;
    default:         dt = FbxStringDT; break;
    }

    FbxProperty newProp = FbxProperty::Create(mNode,
        dt, name.toUtf8().constData());
    newProp.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
    newProp.ModifyFlag(FbxPropertyFlags::eAnimatable, true);

    beginResetModel();
    refresh();
    endResetModel();

    emit userPropertyChanged();
    return true;
}

bool UserPropertyModel::removeProperty(int row)
{
    if (!mNode || row < 0 || row >= mEntries.size()) return false;

    FbxProperty& prop = mEntries[row].prop;
    prop.DestroyRecursively();

    beginResetModel();
    refresh();
    endResetModel();

    emit userPropertyChanged();
    return true;
}

QString UserPropertyModel::fbxTypeName(EFbxType type)
{
    switch (type)
    {
    case eFbxBool:     return QStringLiteral("Bool");
    case eFbxInt:      return QStringLiteral("Int");
    case eFbxFloat:    return QStringLiteral("Float");
    case eFbxDouble:   return QStringLiteral("Double");
    case eFbxString:   return QStringLiteral("String");
    case eFbxDouble3:  return QStringLiteral("Double3");
    case eFbxDouble4:  return QStringLiteral("Double4");
    case eFbxEnum:     return QStringLiteral("Enum");
    default:           return QStringLiteral("Unknown");
    }
}

QVariant UserPropertyModel::readProperty(const FbxProperty& prop)
{
    EFbxType type = prop.GetPropertyDataType().GetType();
    switch (type)
    {
    case eFbxBool:    return prop.Get<FbxBool>();
    case eFbxInt:     return prop.Get<FbxInt>();
    case eFbxFloat:   return (double)prop.Get<FbxFloat>();
    case eFbxDouble:  return prop.Get<FbxDouble>();
    case eFbxString:  return QString::fromUtf8(prop.Get<FbxString>().Buffer());
    case eFbxDouble3:
    {
        FbxDouble3 v = prop.Get<FbxDouble3>();
        return QStringLiteral("%1, %2, %3").arg(v[0]).arg(v[1]).arg(v[2]);
    }
    default:
        return QStringLiteral("(unsupported)");
    }
}

void UserPropertyModel::writeProperty(FbxProperty& prop, const QVariant& value)
{
    EFbxType type = prop.GetPropertyDataType().GetType();
    switch (type)
    {
    case eFbxBool:    prop.Set(value.toBool()); break;
    case eFbxInt:     prop.Set(value.toInt()); break;
    case eFbxFloat:   prop.Set((float)value.toDouble()); break;
    case eFbxDouble:  prop.Set(value.toDouble()); break;
    case eFbxString:  prop.Set(FbxString(value.toString().toUtf8().constData())); break;
    default: break;
    }
}
