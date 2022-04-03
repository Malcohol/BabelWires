/**
 * The IdentityFallbackMapEntryModel provides the specific UI for a MapEntry with IdentityFallbackMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/identityFallbackMapEntryModel.hpp>

QVariant babelwires::IdentityFallbackMapEntryModel::getDisplayData(unsigned int column) const {
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
