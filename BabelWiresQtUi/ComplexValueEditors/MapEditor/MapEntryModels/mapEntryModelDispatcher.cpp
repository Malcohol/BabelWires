/**
 * The MapEntryModelDispatcher provides access to an appropriate MapEntryModel for a row in the map editor.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModelDispatcher.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/oneToOneMapEntryModel.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/allToOneFallbackMapEntryModel.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/allToSameFallbackMapEntryModel.hpp>

#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToSameFallbackMapEntryData.hpp>

void babelwires::MapEntryModelDispatcher::init(const ValueModelRegistry& valueModelRegistry, const TypeSystem& typeSystem, const Type& sourceType, const Type& targetType,
                                               const MapProjectEntry& entry, unsigned int row, MapEntryModel::Column column, bool isLastRow) {
    m_rowModel = &m_rowModelStorage;
    const MapEntryData& entryData = entry.getData();
    if (entryData.as<OneToOneMapEntryData>()) {
        static_assert(sizeof(babelwires::MapEntryModel) == sizeof(babelwires::OneToOneMapEntryModel));
        new (m_rowModel) babelwires::OneToOneMapEntryModel();
    } else if (entryData.as<AllToOneFallbackMapEntryData>()) {
        static_assert(sizeof(babelwires::MapEntryModel) == sizeof(babelwires::AllToOneFallbackMapEntryModel));
        new (m_rowModel) babelwires::AllToOneFallbackMapEntryModel();
    } else if (entryData.as<AllToSameFallbackMapEntryData>()) {
        static_assert(sizeof(babelwires::MapEntryModel) == sizeof(babelwires::AllToSameFallbackMapEntryModel));
        new (m_rowModel) babelwires::AllToSameFallbackMapEntryModel();
    } else {
        // The base row model is used.
    }
    m_rowModel->m_sourceType = &sourceType;
    m_rowModel->m_targetType = &targetType;
    m_rowModel->m_mapProjectEntry = &entry;
    m_rowModel->m_row = row;
    m_rowModel->m_column = column;
    m_rowModel->m_isLastRow = isLastRow;
    m_rowModel->init(valueModelRegistry, typeSystem);
}
