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
#include <BabelWiresLib/ProjectExtra/dataLocation.hpp>

void babelwires::ArrayValueModel::getContextMenuActions(
    const DataLocation& location,
    std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const {
    ValueModel::getContextMenuActions(location, actionsOut);
    if (!m_isReadOnly) {
        const ArrayType* const arrayType = m_type->as<ArrayType>();
        const ArrayValue* arrayValue = m_value->as<ArrayValue>();
        actionsOut.emplace_back(std::make_unique<SetArraySizeAction>(location.getPathToValue()));
    }
}
