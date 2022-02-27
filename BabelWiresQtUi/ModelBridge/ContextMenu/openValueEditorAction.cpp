/**
 * QAction which opens an editor for a given value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/ContextMenu/openValueEditorAction.hpp"

#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"
#include "BabelWiresQtUi/ModelBridge/projectBridge.hpp"
#include "BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp"
#include "BabelWiresQtUi/mainWindow.hpp"


babelwires::OpenValueEditorAction::OpenValueEditorAction(const QString& text, babelwires::FeaturePath pathToArray)
    : FeatureContextMenuAction(text)
    , m_pathToMap(std::move(pathToArray)) {}

void babelwires::OpenValueEditorAction::actionTriggered(babelwires::FeatureModel& model, const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const ElementId elementId = model.getElementId();

    model.getProjectBridge().getMainWindow()->openEditorForValue(ComplexValueEditorData{ elementId, m_pathToMap });
}
