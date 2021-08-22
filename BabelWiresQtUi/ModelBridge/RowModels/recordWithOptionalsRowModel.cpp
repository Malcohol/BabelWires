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

#include "BabelWiresQtUi/ModelBridge/ContextMenu/featureContextMenu.hpp"
#include "BabelWiresQtUi/ModelBridge/ContextMenu/optionalActivationAction.hpp"
#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"

#include "BabelWires/Features/recordWithOptionalsFeature.hpp"
#include "BabelWires/Project/FeatureElements/contentsCache.hpp"

const babelwires::RecordWithOptionalsFeature& babelwires::RecordWithOptionalsRowModel::getRecordWithOptionalsFeature() const {
    assert(getInputThenOutputFeature()->as<const babelwires::RecordWithOptionalsFeature>() &&
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
        // This is useful if you have a base class you want to have optionals for one subclass but not
        // for others. In the latter case, it will just look like an ordinary record.
        return QVariant();
    }
}


void babelwires::RecordWithOptionalsRowModel::getContextMenuActions(
    std::vector<std::unique_ptr<FeatureContextMenuAction>>& actionsOut) const {
    RowModel::getContextMenuActions(actionsOut);
    if (hasInputFeature()) {
        const babelwires::RecordWithOptionalsFeature& recordFeature = getRecordWithOptionalsFeature();
        for (auto op : recordFeature.getOptionalFields()) {
            auto addElement = std::make_unique<OptionalActivationAction>(m_contentsCacheEntry->getPath(), op, recordFeature.isActivated(op));
            actionsOut.emplace_back(std::move(addElement));
        }
    }
}
