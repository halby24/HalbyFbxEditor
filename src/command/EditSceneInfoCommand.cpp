#include "EditSceneInfoCommand.h"

EditSceneInfoCommand::EditSceneInfoCommand(
    FbxScene* scene,
    const FbxSceneData& oldData,
    const FbxSceneData& newData,
    QUndoCommand* parent)
    : QUndoCommand(parent)
    , mScene(scene)
    , mOldData(oldData)
    , mNewData(newData)
{
    setText(QStringLiteral("Edit Scene Info"));
}

void EditSceneInfoCommand::undo()
{
    FbxSceneData::writeTo(mScene, mOldData);
}

void EditSceneInfoCommand::redo()
{
    // Skip first redo since dialog already applied changes
    if (mFirstRedo)
    {
        mFirstRedo = false;
        return;
    }
    FbxSceneData::writeTo(mScene, mNewData);
}
