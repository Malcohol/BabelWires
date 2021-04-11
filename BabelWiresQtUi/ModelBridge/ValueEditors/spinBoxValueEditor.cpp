/**
 * A spinBox QWidget which can be used for editing ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/ValueEditors/spinBoxValueEditor.hpp"

#include "BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp"
#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"

#include <QLineEdit>

babelwires::SpinBoxValueEditor::SpinBoxValueEditor(QWidget* parent, const QModelIndex& index)
    : ValueEditorCommonBase(parent, index) {
    // Commit on select.
    QObject::connect(this, QOverload<int>::of(&QSpinBox::valueChanged), RowModel::getDelegateFromParentWidget(parent),
                     &FeatureModelDelegate::commitEditorValue);
}

void babelwires::SpinBoxValueEditor::setFeatureIsModified(bool isModified) {
    QLineEdit* lineEditor = lineEdit();
    QFont font = lineEditor->font();
    font.setBold(isModified);
    lineEditor->setFont(font);
}
