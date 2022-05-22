/**
 * The AllToSameFallbackMapEntryModel provides the specific UI for a MapEntry with AllToSameFallbackMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/allToSameFallbackMapEntryModel.hpp>

QVariant babelwires::AllToSameFallbackMapEntryModel::getDisplayData(unsigned int column) const {
    switch (column) {
        case 0:
            return "*";
        case 1:
            // TODO Need better.
            return "= same";
        default:
            assert(false);
    }
}

