/**
 * A MapProjectEntry wraps MapEntryData and augments it with features relevant for editing.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Project/projectVisitable.hpp"
#include "BabelWiresLib/TypeSystem/typeSystem.hpp"

#include "Common/Cloning/cloneable.hpp"

#include <Common/types.hpp>

namespace babelwires {
    class MapEntryData;

    /// A MapProjectEntry wraps MapEntryData and augments it with features relevant for editing.
    class MapProjectEntry : public Cloneable {
      public:
        CLONEABLE(MapProjectEntry);
        MapProjectEntry(std::unique_ptr<MapEntryData> data);
        MapProjectEntry(std::unique_ptr<MapEntryData> data, std::string reasonForFailure);
        MapProjectEntry(const MapProjectEntry& other);
        virtual ~MapProjectEntry();
        
        const MapEntryData& getData() const;
        
        /// Get a description of the failure.
        /// Returns the empty string if the entry is valid.
        const std::string& getReasonForFailure() const;

      private:
        std::unique_ptr<MapEntryData> m_data;
        /// This is empty if the entry is valid.
        std::string m_reasonForFailure;
    };
} // namespace babelwires

