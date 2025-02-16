/**
 * The AllToSameFallbackMapEntryModel provides the specific UI for a MapEntry with AllToSameFallbackMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/allToSameFallbackMapEntryModel.hpp>

#include <BabelWiresLib/Types/Map/standardMapIdentifiers.hpp>

QVariant babelwires::AllToSameFallbackMapEntryModel::getDisplayData(Column column) const {
    switch (column) {
        case Column::sourceValue:
            return c_wildcard;
        case Column::targetValue:
            // TODO Need better.
            return c_wildcardMatch;
        default:
            assert(false);
            return {};
    }
}

