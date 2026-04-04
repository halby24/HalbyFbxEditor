#include "PropertyPanel.h"
#include "PropertyDelegate.h"
#include "model/PropertyTreeModel.h"
#include "model/UserPropertyModel.h"
#include "command/EditPropertyCommand.h"

#include <QTreeView>
#include <QTableView>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QUndoStack>
#include <QInputDialog>
#include <QMessageBox>

PropertyPanel::PropertyPanel(QWidget* parent)
    : QDockWidget(tr("Properties"), parent)
{
    setObjectName(QStringLiteral("PropertyPanel"));

    mTabWidget = new QTabWidget(this);
    setWidget(mTabWidget);

    setupPropertiesTab();
    setupUserPropertiesTab();
}

void PropertyPanel::setupPropertiesTab()
{
    auto* page = new QWidget(mTabWidget);
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);

    mModel = new PropertyTreeModel(this);
    mDelegate = new PropertyDelegate(this);

    mTreeView = new QTreeView(page);
    mTreeView->setModel(mModel);
    mTreeView->setItemDelegateForColumn(1, mDelegate);
    mTreeView->setAlternatingRowColors(true);
    mTreeView->setRootIsDecorated(true);
    mTreeView->header()->setStretchLastSection(true);
    mTreeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    layout->addWidget(mTreeView);

    mTabWidget->addTab(page, tr("Properties"));

    connect(mModel, &PropertyTreeModel::propertyChanged,
            this, [this](int propIdx, const QVariant& oldVal, const QVariant& newVal) {
                if (!mUndoStack || !mCurrentNode) return;
                auto props = FbxPropertyBridge::buildNodeProperties(mCurrentNode);
                if (propIdx < 0 || propIdx >= props.size()) return;
                const auto& prop = props[propIdx];
                mUndoStack->push(new EditPropertyCommand(
                    mCurrentNode, propIdx, oldVal, newVal,
                    prop.setter, prop.name));
            });
}

void PropertyPanel::setupUserPropertiesTab()
{
    auto* page = new QWidget(mTabWidget);
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);

    mUserModel = new UserPropertyModel(this);

    mUserTableView = new QTableView(page);
    mUserTableView->setModel(mUserModel);
    mUserTableView->setAlternatingRowColors(true);
    mUserTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mUserTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    mUserTableView->horizontalHeader()->setStretchLastSection(true);
    mUserTableView->verticalHeader()->hide();

    layout->addWidget(mUserTableView);

    // Add/Remove buttons
    auto* btnLayout = new QHBoxLayout();
    auto* addBtn = new QPushButton(tr("Add"), page);
    auto* removeBtn = new QPushButton(tr("Remove"), page);
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(removeBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    mTabWidget->addTab(page, tr("User Properties"));

    connect(addBtn, &QPushButton::clicked, this, &PropertyPanel::onAddUserProperty);
    connect(removeBtn, &QPushButton::clicked, this, &PropertyPanel::onRemoveUserProperty);

    connect(mUserModel, &UserPropertyModel::userPropertyChanged,
            this, [this]() {
                if (mUndoStack)
                    mUndoStack->resetClean();
            });
}

void PropertyPanel::setCurrentNode(FbxNode* node)
{
    mCurrentNode = node;
    mModel->setNode(node);
    mUserModel->setNode(node);
    if (node)
        mTreeView->expandAll();
}

void PropertyPanel::onAddUserProperty()
{
    if (!mCurrentNode) return;

    bool ok;
    QString name = QInputDialog::getText(this, tr("Add Property"),
        tr("Property name:"), QLineEdit::Normal, {}, &ok);
    if (!ok || name.isEmpty()) return;

    QStringList types = {
        QStringLiteral("Bool"),
        QStringLiteral("Int"),
        QStringLiteral("Float"),
        QStringLiteral("Double"),
        QStringLiteral("String")
    };
    QString typeName = QInputDialog::getItem(this, tr("Add Property"),
        tr("Property type:"), types, 3, false, &ok);
    if (!ok) return;

    int fbxType;
    if (typeName == QStringLiteral("Bool"))        fbxType = (int)eFbxBool;
    else if (typeName == QStringLiteral("Int"))     fbxType = (int)eFbxInt;
    else if (typeName == QStringLiteral("Float"))   fbxType = (int)eFbxFloat;
    else if (typeName == QStringLiteral("Double"))  fbxType = (int)eFbxDouble;
    else                                            fbxType = (int)eFbxString;

    if (!mUserModel->addProperty(name, fbxType))
    {
        QMessageBox::warning(this, tr("Error"),
            tr("Failed to add property. It may already exist."));
    }
}

void PropertyPanel::onRemoveUserProperty()
{
    if (!mCurrentNode) return;

    auto idx = mUserTableView->currentIndex();
    if (!idx.isValid()) return;

    auto result = QMessageBox::question(this, tr("Remove Property"),
        tr("Remove property '%1'?").arg(mUserModel->data(
            mUserModel->index(idx.row(), 0)).toString()));

    if (result == QMessageBox::Yes)
        mUserModel->removeProperty(idx.row());
}
