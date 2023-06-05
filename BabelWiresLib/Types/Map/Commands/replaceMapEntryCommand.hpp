/**
 * The command which modifies the value in a map entries.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <BabelWiresLib/Commands/commands.hpp>

namespace babelwires {
    class MapProject;
    class MapEntryData;

    /// Add an element to an array feature.
    class ReplaceMapEntryCommand : public SimpleCommand<MapProject> {
      public:
        ReplaceMapEntryCommand(std::string commandName, std::unique_ptr<MapEntryData> newEntry, unsigned int indexOfReplacement);

        virtual bool initialize(const MapProject& map) override;
        virtual void execute(MapProject& map) const override;
        virtual void undo(MapProject& map) const override;

      private:
        std::unique_ptr<MapEntryData> m_newEntry;
        std::unique_ptr<MapEntryData> m_replacedEntry;
        unsigned int m_indexOfReplacement;
    };

} // namespace babelwires