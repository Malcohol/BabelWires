/**
 * Manages the open set of ComplexValueEditors.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditorManager.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>

#include <Common/Log/userLogger.hpp>

void babelwires::ComplexValueEditorManager::openEditorForValue(QWidget* parent, ProjectBridge& projectBridge, UserLogger& userLogger, const ComplexValueEditorData& data) {
    auto it = m_openValueEditors.find(data);
    if (it == m_openValueEditors.end()) {
        try {
            ComplexValueEditor* newEditor = m_valueEditorFactory.createEditor(parent, projectBridge, userLogger, data);
            auto [nit, _] = m_openValueEditors.insert({data, newEditor});
            it = nit;
            QObject::connect(newEditor, &ComplexValueEditor::editorClosing, this, &ComplexValueEditorManager::onValueEditorClose);
        }
        catch (ModelException& e) {
            // TODO: Depends on context. A failed user action should report an error.
            userLogger.logWarning() << "Could not open an editor for " << data << ": " << e.what();
            return;
        }
    }
    it->second->raise();
}

void babelwires::ComplexValueEditorManager::onValueEditorClose() {
    ComplexValueEditor* editorWhichIsClosing = qobject_cast<ComplexValueEditor*>(sender());
    assert((editorWhichIsClosing != nullptr) && "Received an editorClosing signal with no appropriate sender");
    auto it = m_openValueEditors.find(editorWhichIsClosing->getData());
    assert((it != m_openValueEditors.end())  && "Received an editorClosing signal from an unknown sender");
    m_openValueEditors.erase(it);
}

void babelwires::ComplexValueEditorManager::closeAllValueEditors() {
    auto openValueEditorsCopy = m_openValueEditors;
    for (auto& [_, editor] : openValueEditorsCopy) {
        editor->close();
    }
    assert(m_openValueEditors.empty() && "The editors should have removed themselves");
}
