/**
 * The AllToSameFallbackMapEntryModel provides the specific UI for a MapEntry with AllToSameFallbackMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/allToSameFallbackMapEntryModel.hpp>

QVariant babelwires::AllToSameFallbackMapEntryModel::getDisplayData(Column column) const {
    switch (column) {
        case Column::sourceValue:
            return "*";
        case Column::targetValue:
            // TODO Need better.
            return "= same";
        default:
            assert(false);
            return {};
    }
}

