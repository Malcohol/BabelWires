/**
 * A line editor QWidget which can be used for editing Values.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueEditors/lineEditValueEditor.hpp>

#include <BabelWiresQtUi/ModelBridge/RowModels/rowModel.hpp>
#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>

babelwires::LineEditValueEditor::LineEditValueEditor(QWidget* parent)
    : ValueEditorCommonBase(parent) {}

void babelwires::LineEditValueEditor::setIsModified(bool isModified) {
    QFont f = font();
    f.setBold(isModified);
    setFont(f);
}
