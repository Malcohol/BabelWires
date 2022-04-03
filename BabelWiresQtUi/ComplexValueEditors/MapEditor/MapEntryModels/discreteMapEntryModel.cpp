/**
 * The DiscreteMapEntryModel provides the specific UI for a MapEntry with DiscreteMapEntryData.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/discreteMapEntryModel.hpp>

#include <BabelWiresLib/Maps/MapEntries/discreteMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>

const babelwires::DiscreteMapEntryData& babelwires::DiscreteMapEntryModel::getDiscreteMapEntryData() const {
    assert(m_mapProjectEntry);
    assert(m_mapProjectEntry->getData().as<DiscreteMapEntryData>());
    return static_cast<const babelwires::DiscreteMapEntryData&>(m_mapProjectEntry->getData());
}

void babelwires::DiscreteMapEntryModel::init() {
    const DiscreteMapEntryData& discreteMapEntry = getDiscreteMapEntryData();
    m_sourceValueModel.init(*m_sourceType, *discreteMapEntry.getSourceValue());
    m_targetValueModel.init(*m_targetType, *discreteMapEntry.getTargetValue());
}

QVariant babelwires::DiscreteMapEntryModel::getDisplayData(unsigned int column) const {
    switch (column) {
        case 0:
            return m_sourceValueModel->getDisplayData();
        case 1:
            return m_targetValueModel->getDisplayData();
        default:
            assert(false);
    }
}

