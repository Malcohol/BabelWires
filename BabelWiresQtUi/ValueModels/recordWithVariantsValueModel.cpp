/**
 * Model for RecordValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/recordWithVariantsValueModel.hpp>

#include <BabelWiresQtUi/ModelBridge/ContextMenu/selectVariantAction.hpp>

#include <BabelWiresLib/ProjectExtra/dataLocation.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsValue.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>

void babelwires::RecordWithVariantsValueModel::getContextMenuActions(
    const DataLocation& location,
    std::vector<FeatureContextMenuEntry>& entriesOut) const {
    ValueModel::getContextMenuActions(location, entriesOut);
    if (!m_isReadOnly) {
        auto group = std::make_unique<FeatureContextMenuGroup>("Variants");
        const RecordWithVariantsType& recordType = m_type->is<RecordWithVariantsType>();
        const RecordWithVariantsValue& recordValue = getValue()->is<RecordWithVariantsValue>();
        const ShortId currentTag = recordValue.getTag();
        for (auto tagId : recordType.getTags()) {
            auto selectVariant = std::make_unique<SelectVariantAction>(location.getPathToValue(), tagId);
            if (tagId == currentTag) {
                selectVariant->setChecked(true);
            }
            // TODO tooltip.
            selectVariant->setEnabled(m_isStructureEditable);
            group->addFeatureContextMenuAction(std::move(selectVariant));
        }
        entriesOut.emplace_back(std::move(group));
    }
}
