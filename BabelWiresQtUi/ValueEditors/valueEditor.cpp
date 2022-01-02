/**
 * ValueEditors are type-specific UIs that allow values to be edited.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueEditors/valueEditor.hpp>

#include "Common/Utilities/hash.hpp"

std::size_t babelwires::ValueEditorData::getHash() const {
    return hash::mixtureOf(m_elementId, m_pathToValue);
}

babelwires::ValueEditor::ValueEditor(QWidget *parent, ProjectBridge& projectBridge, UserLogger& userLogger, const ValueEditorData& data)
    : QWidget(parent), m_projectBridge(projectBridge), m_userLogger(userLogger), m_data(data) {}

const babelwires::ValueEditorData& babelwires::ValueEditor::getData() const {
    return m_data;
}

babelwires::ProjectBridge& babelwires::ValueEditor::getProjectBridge() {
    return m_projectBridge;
}
