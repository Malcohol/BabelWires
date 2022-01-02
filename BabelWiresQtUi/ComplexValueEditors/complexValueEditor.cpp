/**
 * ComplexValueEditors are type-specific UIs that allow values to be edited.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditor.hpp>

#include "Common/Utilities/hash.hpp"

std::size_t babelwires::ComplexValueEditorData::getHash() const {
    return hash::mixtureOf(m_elementId, m_pathToValue);
}

babelwires::ComplexValueEditor::ComplexValueEditor(QWidget *parent, ProjectBridge& projectBridge, UserLogger& userLogger, const ComplexValueEditorData& data)
    : QWidget(parent), m_projectBridge(projectBridge), m_userLogger(userLogger), m_data(data) {}

const babelwires::ComplexValueEditorData& babelwires::ComplexValueEditor::getData() const {
    return m_data;
}

babelwires::ProjectBridge& babelwires::ComplexValueEditor::getProjectBridge() {
    return m_projectBridge;
}
