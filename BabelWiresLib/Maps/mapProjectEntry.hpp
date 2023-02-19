/**
 * A MapProjectEntry wraps MapEntryData and augments it with features relevant for editing.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectVisitable.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Cloning/cloneable.hpp>
#include <Common/Utilities/result.hpp>
#include <Common/types.hpp>

namespace babelwires {
    class MapEntryData;

    /// A MapProjectEntry wraps MapEntryData and augments it with features relevant for editing.
    class MapProjectEntry : public Cloneable {
      public:
        CLONEABLE(MapProjectEntry);
        MapProjectEntry(std::unique_ptr<MapEntryData> data);
        MapProjectEntry(const MapProjectEntry& other);
        virtual ~MapProjectEntry();

        const MapEntryData& getData() const;

        /// Get the validity of the entry.
        Result getValidity() const;

        void validate(const TypeSystem& typeSystem, const TypeRef& sourceTypeId, const TypeRef& targetTypeId, bool isLastEntry);

      private:
        std::unique_ptr<MapEntryData> m_data;
        /// This is empty if the entry is valid.
        Result m_validityOfEntry;
    };
} // namespace babelwires
