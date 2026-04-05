#pragma once
#include <QMainWindow>
#include <memory>

class FbxDocumentManager;
class NodeTreePanel;
class PropertyPanel;
class QLabel;
class QUndoStack;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    std::unique_ptr<FbxDocumentManager> mDocument;
    NodeTreePanel* mNodeTreePanel = nullptr;
    PropertyPanel* mPropertyPanel = nullptr;
    QUndoStack* mUndoStack = nullptr;
    QLabel* mStatusLabel = nullptr;

    void setupMenuBar();
    void setupStatusBar();
    void setupDockWidgets();

    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileClose();
    void showSceneInfo();

    void updateTitle();
    void updateStatusBar();

    bool confirmClose();
};
