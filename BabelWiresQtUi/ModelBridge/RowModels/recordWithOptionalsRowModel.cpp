/**
 * The row model for RecordWithOptionalsFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

/**
 * The row model for ArrayFeatures.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/RowModels/recordWithOptionalsRowModel.hpp"

//#include "BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp"
//#include "BabelWiresQtUi/ModelBridge/ContextMenu/insertArrayEntryAction.hpp"
#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"

#include "BabelWires/Features/recordWithOptionalsFeature.hpp"
//#include "BabelWires/Project/FeatureElements/contentsCache.hpp"

const babelwires::RecordWithOptionalsFeature& babelwires::RecordWithOptionalsRowModel::getRecordWithOptionalsFeature() const {
    assert(dynamic_cast<const babelwires::RecordWithOptionalsFeature*>(getInputThenOutputFeature()) &&
           "Wrong type of feature stored");
    return *static_cast<const babelwires::RecordWithOptionalsFeature*>(getInputThenOutputFeature());
}

QVariant babelwires::RecordWithOptionalsRowModel::getValueDisplayData() const {
    const babelwires::RecordWithOptionalsFeature& recordFeature = getRecordWithOptionalsFeature();
    const int numOptionals = recordFeature.getOptionalFields().size();
    if (numOptionals > 0) {
        const int numActiveOptionals = numOptionals - recordFeature.getNumInactiveFields();
        return QString("(%1/%2 optionals)").arg(numActiveOptionals).arg(numOptionals);
    } else {
        // Could log a debug message here.
        return QVariant();
    }
}

/*
void babelwires::RecordWithOptionalsRowModel::getContextMenuActions(
    std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const {
    RowModel::getContextMenuActions(actionsOut);
    if (hasInputFeature()) {
        const babelwires::ArrayFeature& arrayFeature = getArrayFeature();
        const auto sizeRange = arrayFeature.getSizeRange();
        const auto currentSize = arrayFeature.getNumFeatures();
        auto addElement = std::make_unique<InsertArrayEntryAction>(m_contentsCacheEntry->getPath(), -1);
        addElement->setEnabled(sizeRange.contains(currentSize + 1));
        actionsOut.emplace_back(std::move(addElement));
    }
}
*/
