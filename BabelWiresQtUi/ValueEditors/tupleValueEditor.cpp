/**
 * 
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueEditors/tupleValueEditor.hpp>

babelwires::TupleValueEditor::TupleValueEditor(QWidget* parent)
    : ValueEditorCommonBase(parent) {
    //setContextMenuPolicy(Qt::NoContextMenu);
    // Commit on select.
    //QObject::connect(this, QOverload<int>::of(&QSpinBox::valueChanged), 
    //                    this, [this]() { emit m_signals->editorHasChanged(this); });
}

void babelwires::TupleValueEditor::setFeatureIsModified(bool isModified) {
    
}