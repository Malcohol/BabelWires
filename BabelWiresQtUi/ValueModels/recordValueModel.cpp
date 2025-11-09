/**
 * Model for RecordValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/recordValueModel.hpp>

#include <BabelWiresQtUi/ModelBridge/ContextMenu/projectCommandContextMenuAction.hpp>

#include <BabelWiresLib/Project/Commands/activateOptionalCommand.hpp>
#include <BabelWiresLib/Project/Commands/deactivateOptionalCommand.hpp>
#include <BabelWiresLib/ProjectExtra/projectDataLocation.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Types/Record/recordValue.hpp>

#include <Common/Log/debugLogger.hpp>

void babelwires::RecordValueModel::getContextMenuActions(const DataLocation& location,
                                                         std::vector<ContextMenuEntry>& entriesOut) const {
    ValueModel::getContextMenuActions(location, entriesOut);
    if (!m_isReadOnly) {
        const RecordType& recordType = m_type->is<RecordType>();
        if (recordType.getOptionalFieldIds().size() > 0) {
            if (const auto& projectDataLocation = location.as<ProjectDataLocation>()) {
                auto group = std::make_unique<ContextMenuGroup>("Optional fields", QActionGroup::ExclusionPolicy::None);
                for (auto opId : recordType.getOptionalFieldIds()) {
                    const std::string fieldName = IdentifierRegistry::read()->getName(opId).c_str();
                    std::unique_ptr<ContextMenuAction> action;
                    const bool isActivated = recordType.isActivated(getValue(), opId);
                    if (!isActivated) {
                        action = std::make_unique<ProjectCommandContextMenuAction>(
                            fieldName.c_str(), std::make_unique<ActivateOptionalCommand>(
                                           "Activate optional field " + fieldName, projectDataLocation->getNodeId(),
                                           projectDataLocation->getPathToValue(), opId));
                    } else {
                        action = std::make_unique<ProjectCommandContextMenuAction>(
                            fieldName.c_str(), std::make_unique<DeactivateOptionalCommand>(
                                           "Deactivate optional field " + fieldName, projectDataLocation->getNodeId(),
                                           projectDataLocation->getPathToValue(), opId));
                    }
                    // TODO tooltip.
                    action->setEnabled(m_isStructureEditable);
                    action->setCheckable(true);
                    action->setChecked(isActivated);
                    group->addContextMenuAction(std::move(action));
                }
                entriesOut.emplace_back(std::move(group));
            } else {
                logDebug()
                    << "Record with optionals encountered outside the project. No context menu options available.";
            }
        }
    }
}
