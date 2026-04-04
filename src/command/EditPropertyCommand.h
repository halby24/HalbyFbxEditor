#pragma once
#include <QUndoCommand>
#include <QVariant>
#include <functional>
#include <fbxsdk.h>

class PropertyTreeModel;

class EditPropertyCommand : public QUndoCommand
{
public:
    EditPropertyCommand(
        FbxNode* node,
        int propertyIndex,
        const QVariant& oldValue,
        const QVariant& newValue,
        std::function<void(FbxNode*, const QVariant&)> setter,
        const QString& propertyName,
        QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;
    int id() const override;
    bool mergeWith(const QUndoCommand* other) override;

private:
    FbxNode* mNode;
    int mPropertyIndex;
    QVariant mOldValue;
    QVariant mNewValue;
    std::function<void(FbxNode*, const QVariant&)> mSetter;
    bool mFirstRedo = true;
};
