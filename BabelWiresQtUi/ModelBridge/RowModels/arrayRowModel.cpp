/**
 * The row model for ArrayFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/RowModels/arrayRowModel.hpp>

#include <BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp>
#include <BabelWiresQtUi/ModelBridge/ContextMenu/setArraySizeAction.hpp>
#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>

#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/contentsCache.hpp>

const babelwires::ArrayFeature& babelwires::ArrayRowModel::getArrayFeature() const {
    assert(getInputThenOutputFeature()->as<const babelwires::ArrayFeature>() &&
           "Wrong type of feature stored");
    return *static_cast<const babelwires::ArrayFeature*>(getInputThenOutputFeature());
}

QVariant babelwires::ArrayRowModel::getValueDisplayData() const {
    const babelwires::ArrayFeature& arrayFeature = getArrayFeature();
    const int numFeatures = arrayFeature.getNumFeatures();
    return QString("(count: %1)").arg(numFeatures);
}

void babelwires::ArrayRowModel::getContextMenuActions(
    std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const {
    RowModel::getContextMenuActions(actionsOut);
    if (hasInputFeature()) {
        const babelwires::ArrayFeature& arrayFeature = getArrayFeature();
        const auto sizeRange = arrayFeature.getSizeRange();
        const auto currentSize = arrayFeature.getNumFeatures();
        auto addElement = std::make_unique<SetArraySizeAction>(m_contentsCacheEntry->getPath());
        addElement->setEnabled(sizeRange.contains(currentSize + 1));
        actionsOut.emplace_back(std::move(addElement));
    }
}
