/**
 * MapEntryModels provide the UI specifics for MapEntries.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModel.hpp>

void babelwires::MapEntryModel::init() {}

QVariant babelwires::MapEntryModel::getDisplayData(unsigned int column) const {
    return {};
}

const babelwires::MapEntryData& babelwires::MapEntryModel::getMapEntry() const {
    return *m_mapEntry;
}
