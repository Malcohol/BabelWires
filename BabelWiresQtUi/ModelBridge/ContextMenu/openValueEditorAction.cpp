/**
 * QAction which opens an editor for a given value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/openValueEditorAction.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>
#include <BabelWiresQtUi/mainWindow.hpp>


babelwires::OpenValueEditorAction::OpenValueEditorAction(const QString& text, const DataLocation& location)
    : NodeContentsContextMenuActionBase(text)
    , m_location(location.clone()) {}

void babelwires::OpenValueEditorAction::actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const NodeId elementId = model.getNodeId();

    model.getProjectBridge().getMainWindow()->openEditorForValue(*m_location);
}
