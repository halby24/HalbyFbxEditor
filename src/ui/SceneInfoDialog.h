#pragma once
#include <QDialog>
#include "core/FbxSceneData.h"

class QLineEdit;
class QComboBox;
class QDoubleSpinBox;
class QLabel;

class SceneInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SceneInfoDialog(const FbxSceneData& data, QWidget* parent = nullptr);

    FbxSceneData result() const;

private:
    // Editable fields
    QLineEdit* mTitle;
    QLineEdit* mSubject;
    QLineEdit* mAuthor;
    QLineEdit* mKeywords;
    QLineEdit* mRevision;
    QLineEdit* mComment;
    QComboBox* mTimeMode;
    QDoubleSpinBox* mCustomFrameRate;

    // Read-only display
    FbxSceneData mOriginal;

    void setupUi(const FbxSceneData& data);
};
