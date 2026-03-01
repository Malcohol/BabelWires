/**
 * Manages the open set of ComplexValueEditors.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/complexValueEditorManager.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>

#include <BaseLib/Log/userLogger.hpp>

void babelwires::ComplexValueEditorManager::openEditorForValue(QWidget* parent, ProjectGraphModel& projectGraphModel,
                                                               UserLogger& userLogger,
                                                               const ProjectDataLocation& data) {
    auto it = std::find_if(m_openValueEditors.begin(), m_openValueEditors.end(),
                           [&data](const ComplexValueEditor* editor) { return data == editor->getDataLocation(); });
    if (it == m_openValueEditors.end()) {
        if (ResultT<ComplexValueEditor*> result =
                m_valueEditorFactory.createEditor(parent, projectGraphModel, userLogger, data)) {
            ComplexValueEditor* newEditor = *result;
            m_openValueEditors.emplace_back(newEditor);
            it = m_openValueEditors.end() - 1;
            QObject::connect(newEditor, &ComplexValueEditor::editorClosing, this,
                             &ComplexValueEditorManager::onValueEditorClose);
        } else {
            userLogger.logWarning() << "Could not open an editor for " << data << ": " << result.error().toString();
            return;
        }
    }
    (*it)->raise();
}

void babelwires::ComplexValueEditorManager::onValueEditorClose() {
    ComplexValueEditor* editorWhichIsClosing = qobject_cast<ComplexValueEditor*>(sender());
    assert((editorWhichIsClosing != nullptr) && "Received an editorClosing signal with no appropriate sender");
    auto it = std::find(m_openValueEditors.begin(), m_openValueEditors.end(), editorWhichIsClosing);
    assert((it != m_openValueEditors.end()) && "Received an editorClosing signal from an unknown sender");
    m_openValueEditors.erase(it);
}

void babelwires::ComplexValueEditorManager::closeAllValueEditors() {
    auto openValueEditorsCopy = m_openValueEditors;
    for (auto* editor : openValueEditorsCopy) {
        editor->close();
    }
    assert(m_openValueEditors.empty() && "The editors should have removed themselves");
}
