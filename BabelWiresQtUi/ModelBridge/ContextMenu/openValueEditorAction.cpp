/**
 * QAction which opens an editor for a given value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/openValueEditorAction.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>
#include <BabelWiresQtUi/mainWindow.hpp>

#include <BabelWiresLib/ProjectExtra/dataLocation.hpp>

babelwires::OpenValueEditorAction::OpenValueEditorAction(const QString& text, const ProjectDataLocation& location)
    : NodeContentsContextMenuActionBase(text)
    , m_location(location.clone()) {}

babelwires::OpenValueEditorAction::~OpenValueEditorAction() = default;

void babelwires::OpenValueEditorAction::actionTriggered(babelwires::NodeContentsModel& model, const QModelIndex& index) const {
    ProjectGraphModel& projectGraphModel = model.getProjectGraphModel();
    const NodeId elementId = model.getNodeId();

    model.getProjectGraphModel().getMainWindow()->openEditorForValue(*m_location);
}
