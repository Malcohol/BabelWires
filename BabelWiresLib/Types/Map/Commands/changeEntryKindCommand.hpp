
/**
 * The command which changes the kind of entry in a map.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>

namespace babelwires {
    class MapProject;

    /// Add an element to an array.
    class ChangeEntryKindCommand : public SimpleCommand<MapProject> {
      public:

        ChangeEntryKindCommand(std::string commandName, MapEntryData::Kind kind, unsigned int indexOfEntry);

        virtual bool initialize(const MapProject& map) override;
        virtual void execute(MapProject& map) const override;
        virtual void undo(MapProject& map) const override;

      private:
        MapEntryData::Kind m_kind;
        std::unique_ptr<MapEntryData> m_newEntry;
        std::unique_ptr<MapEntryData> m_replacedEntry;
        unsigned int m_indexOfEntry;
    };

} // namespace babelwires