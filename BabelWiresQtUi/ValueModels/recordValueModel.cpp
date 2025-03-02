/**
 * Model for RecordValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/recordValueModel.hpp>

#include <BabelWiresQtUi/ModelBridge/ContextMenu/optionalActivationAction.hpp>

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
                const RecordValue& recordValue = getValue()->is<RecordValue>();
                auto group = std::make_unique<ContextMenuGroup>("Optional fields", QActionGroup::ExclusionPolicy::None);
                for (auto opId : recordType.getOptionalFieldIds()) {
                    auto activateOptional = std::make_unique<OptionalActivationAction>(
                        projectDataLocation->getPathToValue(), opId, recordType.isActivated(recordValue, opId));
                    // TODO tooltip.
                    activateOptional->setEnabled(m_isStructureEditable);
                    group->addContextMenuAction(std::move(activateOptional));
                }
                entriesOut.emplace_back(std::move(group));
            } else {
                logDebug() << "Record with optionals encountered outside the project. No context menu options available.";
            }
        }
    }
}
