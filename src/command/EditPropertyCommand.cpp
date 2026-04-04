#include "EditPropertyCommand.h"

EditPropertyCommand::EditPropertyCommand(
    FbxNode* node,
    int propertyIndex,
    const QVariant& oldValue,
    const QVariant& newValue,
    std::function<void(FbxNode*, const QVariant&)> setter,
    const QString& propertyName,
    QUndoCommand* parent)
    : QUndoCommand(parent)
    , mNode(node)
    , mPropertyIndex(propertyIndex)
    , mOldValue(oldValue)
    , mNewValue(newValue)
    , mSetter(std::move(setter))
{
    setText(QStringLiteral("Edit %1").arg(propertyName));
}

void EditPropertyCommand::undo()
{
    if (mSetter && mNode)
        mSetter(mNode, mOldValue);
}

void EditPropertyCommand::redo()
{
    // Skip first redo since PropertyTreeModel::setData already applied it
    if (mFirstRedo)
    {
        mFirstRedo = false;
        return;
    }
    if (mSetter && mNode)
        mSetter(mNode, mNewValue);
}

int EditPropertyCommand::id() const
{
    // Unique per (node, property) for merge support
    return 1000 + mPropertyIndex;
}

bool EditPropertyCommand::mergeWith(const QUndoCommand* other)
{
    auto* cmd = dynamic_cast<const EditPropertyCommand*>(other);
    if (!cmd) return false;
    if (cmd->mNode != mNode || cmd->mPropertyIndex != mPropertyIndex)
        return false;
    mNewValue = cmd->mNewValue;
    return true;
}
