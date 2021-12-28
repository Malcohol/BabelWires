/**
 * QAction which opens the map editor for a given map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/ContextMenu/editMapAction.hpp"

#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"
#include "BabelWiresQtUi/ModelBridge/modifyModelScope.hpp"
#include "BabelWiresQtUi/ModelBridge/projectBridge.hpp"
#include "BabelWiresQtUi/Utilities/fileDialogs.hpp"
#include "BabelWiresQtUi/uiProjectContext.hpp"
#include "BabelWiresQtUi/MapEditor/mapEditor.hpp"
#include "BabelWiresQtUi/mainWindow.hpp"

#include "BabelWiresLib/Features/recordFeature.hpp"
#include "BabelWiresLib/Features/mapFeature.hpp"
#include "BabelWiresLib/Project/Commands/changeFileCommand.hpp"
#include "BabelWiresLib/Commands/commandManager.hpp"
#include "BabelWiresLib/Project/FeatureElements/fileElement.hpp"
#include "BabelWiresLib/Project/project.hpp"

babelwires::EditMapAction::EditMapAction(babelwires::FeaturePath pathToArray)
    : FeatureContextMenuAction(tr("Edit map"))
    , m_pathToMap(std::move(pathToArray)) {}

void babelwires::EditMapAction::actionTriggered(babelwires::FeatureModel& model, const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const ElementId elementId = model.getElementId();

    model.getProjectBridge().getMainWindow()->createEditor<MapEditor>(elementId, m_pathToMap);
}
