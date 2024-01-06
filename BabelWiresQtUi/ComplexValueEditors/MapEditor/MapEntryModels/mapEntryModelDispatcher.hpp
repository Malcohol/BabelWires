/**
 * The MapEntryModelDispatcher provides access to an appropriate MapEntryModel for a row in the map editor.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapEntryModels/mapEntryModel.hpp>

namespace babelwires {

    /// Dispatches to an appropriate MapEntryModel.
    /// A bit of a hack to avoid an unnecessary allocation:
    /// Allocate the appropriate MapEntryModel on top of its same-sized base class.
    /// Neither require destruction, so it should be safe.
    class MapEntryModelDispatcher {
      public:
        void init(const ValueModelRegistry& valueModelRegistry, const TypeSystem& typeSystem, const Type& sourceType, const Type& targetType, const MapProjectEntry& entry, unsigned int row, bool isLastRow);

        const MapEntryModel* operator->() const { return m_rowModel; }

      private:
        MapEntryModel* m_rowModel;
        MapEntryModel m_rowModelStorage;
    };

} // namespace babelwires
