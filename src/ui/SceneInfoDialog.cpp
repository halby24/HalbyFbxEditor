#include "SceneInfoDialog.h"

#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDialogButtonBox>

SceneInfoDialog::SceneInfoDialog(const FbxSceneData& data, QWidget* parent)
    : QDialog(parent)
    , mOriginal(data)
{
    setWindowTitle(tr("Scene Information"));
    setMinimumWidth(500);
    setupUi(data);
}

void SceneInfoDialog::setupUi(const FbxSceneData& data)
{
    auto* mainLayout = new QVBoxLayout(this);

    // --- Document Info (Editable) ---
    {
        auto* group = new QGroupBox(tr("Document Info"), this);
        auto* form = new QFormLayout(group);

        mTitle = new QLineEdit(data.title, this);
        form->addRow(tr("Title:"), mTitle);

        mSubject = new QLineEdit(data.subject, this);
        form->addRow(tr("Subject:"), mSubject);

        mAuthor = new QLineEdit(data.author, this);
        form->addRow(tr("Author:"), mAuthor);

        mKeywords = new QLineEdit(data.keywords, this);
        form->addRow(tr("Keywords:"), mKeywords);

        mRevision = new QLineEdit(data.revision, this);
        form->addRow(tr("Revision:"), mRevision);

        mComment = new QLineEdit(data.comment, this);
        form->addRow(tr("Comment:"), mComment);

        mainLayout->addWidget(group);
    }

    // --- System Units & Axis (Read-only) ---
    {
        auto* group = new QGroupBox(tr("System"), this);
        auto* form = new QFormLayout(group);

        form->addRow(tr("Unit Scale:"),
            new QLabel(QStringLiteral("%1 (%2)")
                .arg(data.systemUnitScale).arg(data.systemUnitName), this));

        QString upStr = QStringLiteral("%1%2")
            .arg(data.upAxisSign > 0 ? "+" : "-")
            .arg(FbxSceneData::axisName(data.upAxis));
        QString frontStr = QStringLiteral("%1%2")
            .arg(data.frontAxisSign > 0 ? "+" : "-")
            .arg(FbxSceneData::axisName(data.frontAxis));
        form->addRow(tr("Up Axis:"), new QLabel(upStr, this));
        form->addRow(tr("Front Axis:"), new QLabel(frontStr, this));

        mainLayout->addWidget(group);
    }

    // --- Timeline (Editable) ---
    {
        auto* group = new QGroupBox(tr("Timeline"), this);
        auto* form = new QFormLayout(group);

        mTimeMode = new QComboBox(this);
        mTimeMode->addItems(FbxSceneData::timeModeLabels());
        mTimeMode->setCurrentIndex(data.timeMode);
        form->addRow(tr("Time Mode:"), mTimeMode);

        mCustomFrameRate = new QDoubleSpinBox(this);
        mCustomFrameRate->setRange(1.0, 1000.0);
        mCustomFrameRate->setDecimals(2);
        mCustomFrameRate->setValue(data.customFrameRate);
        form->addRow(tr("Custom Frame Rate:"), mCustomFrameRate);

        mainLayout->addWidget(group);
    }

    // --- Application Info (Read-only) ---
    {
        auto* group = new QGroupBox(tr("Application Info"), this);
        auto* form = new QFormLayout(group);

        if (!data.originalAppName.isEmpty())
        {
            form->addRow(tr("Created by:"),
                new QLabel(QStringLiteral("%1 %2 (%3)")
                    .arg(data.originalAppName, data.originalAppVersion, data.originalAppVendor), this));
        }
        if (!data.lastSavedAppName.isEmpty())
        {
            form->addRow(tr("Last saved by:"),
                new QLabel(QStringLiteral("%1 %2 (%3)")
                    .arg(data.lastSavedAppName, data.lastSavedAppVersion, data.lastSavedAppVendor), this));
        }
        if (data.originalAppName.isEmpty() && data.lastSavedAppName.isEmpty())
        {
            form->addRow(new QLabel(tr("No application info available."), this));
        }

        mainLayout->addWidget(group);
    }

    // --- Buttons ---
    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
}

FbxSceneData SceneInfoDialog::result() const
{
    FbxSceneData d = mOriginal;

    d.title    = mTitle->text();
    d.subject  = mSubject->text();
    d.author   = mAuthor->text();
    d.keywords = mKeywords->text();
    d.revision = mRevision->text();
    d.comment  = mComment->text();
    d.timeMode = mTimeMode->currentIndex();
    d.customFrameRate = mCustomFrameRate->value();

    return d;
}
