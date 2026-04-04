#include "MainWindow.h"
#include "NodeTreePanel.h"
#include "PropertyPanel.h"
#include "SceneInfoDialog.h"
#include "core/FbxDocument.h"
#include "core/FbxSceneData.h"
#include "command/EditSceneInfoCommand.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QCloseEvent>
#include <QUndoStack>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , mDocument(std::make_unique<FbxDocumentManager>())
    , mUndoStack(new QUndoStack(this))
{
    setWindowTitle(QStringLiteral("HalbyFbxEditor"));
    resize(1200, 800);

    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupDockWidgets();

    connect(mUndoStack, &QUndoStack::cleanChanged, this, [this](bool clean) {
        mDocument->setModified(!clean);
        updateTitle();
    });
}

MainWindow::~MainWindow() = default;

void MainWindow::setupMenuBar()
{
    auto* fileMenu = menuBar()->addMenu(tr("&File"));

    auto* openAction = fileMenu->addAction(tr("&Open..."), this, &MainWindow::fileOpen);
    openAction->setShortcut(QKeySequence::Open);

    auto* saveAction = fileMenu->addAction(tr("&Save"), this, &MainWindow::fileSave);
    saveAction->setShortcut(QKeySequence::Save);

    auto* saveAsAction = fileMenu->addAction(tr("Save &As..."), this, &MainWindow::fileSaveAs);
    saveAsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));

    fileMenu->addSeparator();

    fileMenu->addAction(tr("&Close"), this, &MainWindow::fileClose);

    fileMenu->addSeparator();

    auto* quitAction = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
    quitAction->setShortcut(QKeySequence::Quit);

    // Edit menu with undo/redo
    auto* editMenu = menuBar()->addMenu(tr("&Edit"));
    auto* undoAction = mUndoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    editMenu->addAction(undoAction);

    auto* redoAction = mUndoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    editMenu->addAction(redoAction);
}

void MainWindow::setupToolBar()
{
    auto* toolbar = addToolBar(tr("Main"));
    toolbar->setObjectName(QStringLiteral("MainToolBar"));

    toolbar->addAction(tr("Open"), this, &MainWindow::fileOpen);
    toolbar->addAction(tr("Save"), this, &MainWindow::fileSave);
    toolbar->addAction(tr("Save As"), this, &MainWindow::fileSaveAs);
    toolbar->addSeparator();
    toolbar->addAction(mUndoStack->createUndoAction(this, tr("Undo")));
    toolbar->addAction(mUndoStack->createRedoAction(this, tr("Redo")));
    toolbar->addSeparator();
    toolbar->addAction(tr("Scene Info"), this, &MainWindow::showSceneInfo);
}

void MainWindow::setupStatusBar()
{
    mStatusLabel = new QLabel(this);
    statusBar()->addWidget(mStatusLabel, 1);
    updateStatusBar();
}

void MainWindow::setupDockWidgets()
{
    mNodeTreePanel = new NodeTreePanel(this);
    addDockWidget(Qt::LeftDockWidgetArea, mNodeTreePanel);

    mPropertyPanel = new PropertyPanel(this);
    mPropertyPanel->setUndoStack(mUndoStack);
    addDockWidget(Qt::RightDockWidgetArea, mPropertyPanel);

    connect(mNodeTreePanel, &NodeTreePanel::currentNodeChanged,
            mPropertyPanel, &PropertyPanel::setCurrentNode);
}

// --- File Operations --------------------------------------------------------

void MainWindow::fileOpen()
{
    if (!confirmClose()) return;

    QString path = QFileDialog::getOpenFileName(
        this, tr("Open FBX File"), {},
        tr("FBX Files (*.fbx);;All Files (*)"));

    if (path.isEmpty()) return;

    if (!mDocument->open(path))
    {
        QMessageBox::critical(this, tr("Error"),
            tr("Failed to open file:\n%1").arg(mDocument->lastError()));
        return;
    }

    mUndoStack->clear();
    mNodeTreePanel->setScene(mDocument->scene());
    updateTitle();
    updateStatusBar();
}

void MainWindow::fileSave()
{
    if (!mDocument->isOpen()) return;

    if (mDocument->filePath().isEmpty())
    {
        fileSaveAs();
        return;
    }

    if (!mDocument->save())
    {
        QMessageBox::critical(this, tr("Error"),
            tr("Failed to save file:\n%1").arg(mDocument->lastError()));
        return;
    }

    mUndoStack->setClean();
    updateTitle();
}

void MainWindow::fileSaveAs()
{
    if (!mDocument->isOpen()) return;

    QString path = QFileDialog::getSaveFileName(
        this, tr("Save FBX File"), mDocument->filePath(),
        tr("FBX Files (*.fbx);;All Files (*)"));

    if (path.isEmpty()) return;

    if (!mDocument->saveAs(path))
    {
        QMessageBox::critical(this, tr("Error"),
            tr("Failed to save file:\n%1").arg(mDocument->lastError()));
        return;
    }

    mUndoStack->setClean();
    updateTitle();
    updateStatusBar();
}

void MainWindow::fileClose()
{
    if (!confirmClose()) return;

    mPropertyPanel->setCurrentNode(nullptr);
    mNodeTreePanel->setScene(nullptr);
    mUndoStack->clear();
    mDocument->close();
    updateTitle();
    updateStatusBar();
}

// --- UI Updates -------------------------------------------------------------

void MainWindow::updateTitle()
{
    QString title = QStringLiteral("HalbyFbxEditor");
    if (mDocument->isOpen())
    {
        title += QStringLiteral(" - ") + mDocument->filePath();
        if (mDocument->isModified())
            title += QStringLiteral(" *");
    }
    setWindowTitle(title);
}

void MainWindow::updateStatusBar()
{
    if (!mDocument->isOpen())
    {
        mStatusLabel->setText(tr("No file loaded"));
        return;
    }

    mStatusLabel->setText(
        tr("%1 | %2 nodes")
            .arg(mDocument->filePath())
            .arg(mDocument->nodeCount()));
}

bool MainWindow::confirmClose()
{
    if (!mDocument->isOpen() || !mDocument->isModified())
        return true;

    auto result = QMessageBox::question(
        this, tr("Unsaved Changes"),
        tr("The file has been modified. Save changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (result == QMessageBox::Save)
    {
        fileSave();
        return !mDocument->isModified();
    }
    return result == QMessageBox::Discard;
}

void MainWindow::showSceneInfo()
{
    if (!mDocument->isOpen()) return;

    FbxSceneData oldData = FbxSceneData::readFrom(mDocument->scene());
    SceneInfoDialog dlg(oldData, this);

    if (dlg.exec() == QDialog::Accepted)
    {
        FbxSceneData newData = dlg.result();
        FbxSceneData::writeTo(mDocument->scene(), newData);
        mUndoStack->push(new EditSceneInfoCommand(
            mDocument->scene(), oldData, newData));
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (confirmClose())
        event->accept();
    else
        event->ignore();
}
