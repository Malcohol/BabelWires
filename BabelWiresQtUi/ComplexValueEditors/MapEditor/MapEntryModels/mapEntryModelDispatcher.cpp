/**
 * The MapEntryModelDispatcher provides access to an appropriate MapEntryModel for a row in the map editor.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModelDispatcher.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/discreteMapEntryModel.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/allToOneFallbackMapEntryModel.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/identityFallbackMapEntryModel.hpp>

#include <BabelWiresLib/Maps/mapProjectEntry.hpp>
#include <BabelWiresLib/Maps/MapEntries/discreteMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/identityFallbackMapEntryData.hpp>

void babelwires::MapEntryModelDispatcher::init(const Type& sourceType, const Type& targetType,
                                               const MapProjectEntry& entry, unsigned int row, bool isLastRow) {
    m_rowModel = &m_rowModelStorage;
    const MapEntryData& entryData = entry.getData();
    if (entryData.as<DiscreteMapEntryData>()) {
        static_assert(sizeof(babelwires::MapEntryModel) == sizeof(babelwires::DiscreteMapEntryModel));
        new (m_rowModel) babelwires::DiscreteMapEntryModel();
    } else if (entryData.as<AllToOneFallbackMapEntryData>()) {
        static_assert(sizeof(babelwires::MapEntryModel) == sizeof(babelwires::AllToOneFallbackMapEntryModel));
        new (m_rowModel) babelwires::AllToOneFallbackMapEntryModel();
    } else if (entryData.as<IdentityFallbackMapEntryData>()) {
        static_assert(sizeof(babelwires::MapEntryModel) == sizeof(babelwires::IdentityFallbackMapEntryModel));
        new (m_rowModel) babelwires::IdentityFallbackMapEntryModel();
    } else {
        // The base row model is used.
    }
    m_rowModel->m_sourceType = &sourceType;
    m_rowModel->m_targetType = &targetType;
    m_rowModel->m_mapProjectEntry = &entry;
    m_rowModel->m_row = row;
    m_rowModel->m_isLastRow = isLastRow;
    m_rowModel->init();
}
