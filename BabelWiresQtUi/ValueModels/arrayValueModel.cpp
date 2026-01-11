/**
 * Model for ArrayValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/arrayValueModel.hpp>

#include <BabelWiresQtUi/ModelBridge/ContextMenu/setArraySizeAction.hpp>

#include <BabelWiresLib/Types/Array/arrayValue.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/ProjectExtra/projectDataLocation.hpp>

#include <Common/Log/debugLogger.hpp>

void babelwires::ArrayValueModel::getContextMenuActions(
    const DataLocation& location,
    std::vector<ContextMenuEntry>& actionsOut) const {
    ValueModel::getContextMenuActions(location, actionsOut);
    if (!m_isReadOnly) {
        if (const auto& projectDataLocation = location.tryAs<ProjectDataLocation>()) {
            const ArrayType* const arrayType = m_type->tryAs<ArrayType>();
            const ArrayValue* arrayValue = getValue()->tryAs<ArrayValue>();
            auto setArraySize = std::make_unique<SetArraySizeAction>(projectDataLocation->getPathToValue());
            // TODO tooltip.
            setArraySize->setEnabled(m_isStructureEditable);
            actionsOut.emplace_back(std::move(setArraySize));
        } else {
            logDebug() << "Array encountered outside the project. No context menu options available.";
        }
    }
}
