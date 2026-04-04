#pragma once
#include <QDockWidget>
#include <fbxsdk.h>

class QTreeView;
class QTableView;
class QTabWidget;
class QUndoStack;
class PropertyTreeModel;
class PropertyDelegate;
class UserPropertyModel;

class PropertyPanel : public QDockWidget
{
    Q_OBJECT

public:
    explicit PropertyPanel(QWidget* parent = nullptr);

    void setUndoStack(QUndoStack* stack) { mUndoStack = stack; }

public slots:
    void setCurrentNode(FbxNode* node);

private:
    QTabWidget* mTabWidget = nullptr;

    // Properties tab
    QTreeView* mTreeView = nullptr;
    PropertyTreeModel* mModel = nullptr;
    PropertyDelegate* mDelegate = nullptr;

    // User properties tab
    QTableView* mUserTableView = nullptr;
    UserPropertyModel* mUserModel = nullptr;

    QUndoStack* mUndoStack = nullptr;
    FbxNode* mCurrentNode = nullptr;

    void setupPropertiesTab();
    void setupUserPropertiesTab();
    void onAddUserProperty();
    void onRemoveUserProperty();
};
