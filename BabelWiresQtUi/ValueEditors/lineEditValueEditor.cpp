/**
 * A line editor QWidget which can be used for editing ValueFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueEditors/lineEditValueEditor.hpp>

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>

babelwires::LineEditValueEditor::LineEditValueEditor(QWidget* parent, const QModelIndex& index)
    : ValueEditorCommonBase(parent, index) {}

void babelwires::LineEditValueEditor::setFeatureIsModified(bool isModified) {
    QFont f = font();
    f.setBold(isModified);
    setFont(f);
}
