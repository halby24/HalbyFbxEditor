#pragma once
#include <QAbstractTableModel>
#include <QVector>
#include <fbxsdk.h>

class UserPropertyModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit UserPropertyModel(QObject* parent = nullptr);

    void setNode(FbxNode* node);
    FbxNode* node() const { return mNode; }

    // Add/remove user properties
    bool addProperty(const QString& name, int fbxType);
    bool removeProperty(int row);

    // QAbstractTableModel interface
    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

signals:
    void userPropertyChanged();

private:
    struct PropEntry
    {
        QString name;
        FbxProperty prop;
    };

    FbxNode* mNode = nullptr;
    QVector<PropEntry> mEntries;

    void refresh();
    static QString fbxTypeName(EFbxType type);
    static QVariant readProperty(const FbxProperty& prop);
    static void writeProperty(FbxProperty& prop, const QVariant& value);
};
