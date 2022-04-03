/**
 * The AllToOneFallbackMapEntryModel provides the specific UI for a MapEntry with AllToOneFallbackMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/allToOneFallbackMapEntryModel.hpp>

#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>

const babelwires::AllToOneFallbackMapEntryData& babelwires::AllToOneFallbackMapEntryModel::getAllToOneFallbackMapEntryData() const {
    assert(m_mapEntry);
    assert(m_mapEntry->as<AllToOneFallbackMapEntryData>());
    return static_cast<const babelwires::AllToOneFallbackMapEntryData&>(*m_mapEntry);
}

void babelwires::AllToOneFallbackMapEntryModel::init() {
    const AllToOneFallbackMapEntryData& allToOneFallbackMapEntry = getAllToOneFallbackMapEntryData();
    m_targetValueModel.init(*m_targetType, *allToOneFallbackMapEntry.getTargetValue());
}

QVariant babelwires::AllToOneFallbackMapEntryModel::getDisplayData(unsigned int column) const {
    switch (column) {
        case 0:
            return "*";
        case 1:
            return m_targetValueModel->getDisplayData();
        default:
            assert(false);
    }
}
