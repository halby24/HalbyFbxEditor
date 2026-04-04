#pragma once
#include <QUndoCommand>
#include "core/FbxSceneData.h"

class EditSceneInfoCommand : public QUndoCommand
{
public:
    EditSceneInfoCommand(
        FbxScene* scene,
        const FbxSceneData& oldData,
        const FbxSceneData& newData,
        QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    FbxScene* mScene;
    FbxSceneData mOldData;
    FbxSceneData mNewData;
    bool mFirstRedo = true;
};
