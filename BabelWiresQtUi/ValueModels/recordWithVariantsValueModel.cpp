/**
 * Model for RecordValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/recordWithVariantsValueModel.hpp>

#include <BabelWiresQtUi/ModelBridge/ContextMenu/selectVariantAction.hpp>

#include <BabelWiresLib/ProjectExtra/projectDataLocation.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsValue.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>

#include <Common/Log/debugLogger.hpp>

void babelwires::RecordWithVariantsValueModel::getContextMenuActions(
    const DataLocation& location,
    std::vector<ContextMenuEntry>& entriesOut) const {
    ValueModel::getContextMenuActions(location, entriesOut);
    if (!m_isReadOnly) {
        if (const auto& projectDataLocation = location.as<ProjectDataLocation>()) {
            auto group = std::make_unique<ContextMenuGroup>("Variants");
            const RecordWithVariantsType& recordType = m_type->is<RecordWithVariantsType>();
            const RecordWithVariantsValue& recordValue = getValue()->is<RecordWithVariantsValue>();
            const ShortId currentTag = recordValue.getTag();
            for (auto tagId : recordType.getTags()) {
                auto selectVariant = std::make_unique<SelectVariantAction>(projectDataLocation->getPathToValue(), tagId);
                if (tagId == currentTag) {
                    selectVariant->setChecked(true);
                }
                // TODO tooltip.
                selectVariant->setEnabled(m_isStructureEditable);
                group->addContextMenuAction(std::move(selectVariant));
            }
            entriesOut.emplace_back(std::move(group));
        } else {
            logDebug() << "RecordWithVariants encountered outside the project. No context menu options available.";
        }
    }
}
