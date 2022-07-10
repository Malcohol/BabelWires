/**
 * The row model for MapFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/RowModels/mapRowModel.hpp"

#include "BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp"
#include "BabelWiresQtUi/ModelBridge/ContextMenu/openValueEditorAction.hpp"
#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"

#include "BabelWiresLib/Features/mapFeature.hpp"
#include "BabelWiresLib/Project/FeatureElements/contentsCache.hpp"

const babelwires::MapFeature& babelwires::MapRowModel::getMapFeature() const {
    assert(getInputThenOutputFeature()->as<const babelwires::MapFeature>() &&
           "Wrong type of feature stored");
    return *static_cast<const babelwires::MapFeature*>(getInputThenOutputFeature());
}

QVariant babelwires::MapRowModel::getValueDisplayData() const {
    if (m_contentsCacheEntry->hasLocalModifier()) {
        return QString("(Local map value)");
    }
    return {};
}

void babelwires::MapRowModel::getContextMenuActions(
    std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const {
    RowModel::getContextMenuActions(actionsOut);
    if (hasInputFeature()) {
        const babelwires::MapFeature& mapFeature = getMapFeature();
        auto editAction = std::make_unique<OpenValueEditorAction>("Open map editor", m_contentsCacheEntry->getPath());
        actionsOut.emplace_back(std::move(editAction));
    }
}
