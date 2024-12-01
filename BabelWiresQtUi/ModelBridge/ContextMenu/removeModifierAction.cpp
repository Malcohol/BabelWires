/**
 * QAction for the remove modifier action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/removeModifierAction.hpp>

#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Commands/removeModifierCommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/contentsCache.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::RemoveModifierAction::RemoveModifierAction()
    : FeatureContextMenuAction(tr("Remove modifier")) {}

void babelwires::RemoveModifierAction::actionTriggered(babelwires::FeatureModel& model,
                                                       const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const ElementId elementId = model.getElementId();

    AccessModelScope scope(projectBridge);
    const Project& project = scope.getProject();
    const Node* const element = project.getFeatureElement(elementId);
    if (!element) {
        return;
    }

    const babelwires::ContentsCacheEntry* const entry = model.getEntry(scope, index);
    if (!entry) {
        return;
    }
    const ValueTreeNode* const valueTreeNode = entry->getInputThenOutput();
    assert(valueTreeNode && "No valueTreeNode for row model");
    const bool isModified = (valueTreeNode == entry->getInput()) && element->findModifier(Path(valueTreeNode));
    if (isModified) {
        projectBridge.scheduleCommand(
            std::make_unique<RemoveModifierCommand>("Remove modifier", elementId, entry->getPath()));
    }
}
