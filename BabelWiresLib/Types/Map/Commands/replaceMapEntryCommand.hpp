/**
 * The command which modifies the value in a map entries.
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
    class MapEntryData;

    /// Add an element to an array.
    class ReplaceMapEntryCommand : public SimpleCommand<MapProject> {
      public:
        CLONEABLE(ReplaceMapEntryCommand);
        ReplaceMapEntryCommand(std::string commandName, std::unique_ptr<MapEntryData> newEntry, unsigned int indexOfReplacement);
        ReplaceMapEntryCommand(const ReplaceMapEntryCommand& other);

        virtual bool initialize(const MapProject& map) override;
        virtual void execute(MapProject& map) const override;
        virtual void undo(MapProject& map) const override;

      private:
        std::unique_ptr<MapEntryData> m_newEntry;
        unsigned int m_indexOfReplacement;

        // Post initialization data

        std::unique_ptr<MapEntryData> m_replacedEntry;
    };

} // namespace babelwires