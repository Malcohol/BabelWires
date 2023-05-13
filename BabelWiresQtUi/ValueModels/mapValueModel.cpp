/**
 * Model for MapValues.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ValueModels/mapValueModel.hpp>

#include <BabelWiresQtUi/ModelBridge/ContextMenu/openValueEditorAction.hpp>

#include <BabelWiresLib/Types/Map/mapValue.hpp>
#include <BabelWiresLib/Types/Map/mapType.hpp>

void babelwires::MapValueModel::getContextMenuActions(
    const DataLocation& location,
    std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const {
    ValueModel::getContextMenuActions(location, actionsOut);
    if (!m_isReadOnly) {
        const babelwires::MapType& mapFeature = getType()->is<MapType>();
        auto editAction = std::make_unique<OpenValueEditorAction>("Open map editor", location);
        actionsOut.emplace_back(std::move(editAction));
    }
}
