/**
 * Model for RecordValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/recordValueModel.hpp>

#include <BabelWiresQtUi/ModelBridge/ContextMenu/optionalActivationAction.hpp>

#include <BabelWiresLib/ProjectExtra/dataLocation.hpp>
#include <BabelWiresLib/Types/Record/recordValue.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>

void babelwires::RecordValueModel::getContextMenuActions(
    const DataLocation& location,
    std::vector<FeatureContextMenuEntry>& actionsOut) const {
    ValueModel::getContextMenuActions(location, actionsOut);
    if (!m_isReadOnly) {
        const RecordType& recordType = m_type->is<RecordType>();
        const RecordValue& recordValue = getValue()->is<RecordValue>();
        for (auto opId : recordType.getOptionalFieldIds()) {
            auto activateOptional = std::make_unique<OptionalActivationAction>(location.getPathToValue(), opId, recordType.isActivated(recordValue, opId));
            // TODO tooltip.
            activateOptional->setEnabled(m_isStructureEditable);
            actionsOut.emplace_back(std::move(activateOptional));
        }
    }
}
