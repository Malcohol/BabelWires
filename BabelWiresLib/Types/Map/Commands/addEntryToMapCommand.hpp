/**
 * The command which adds entries to arrays.
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

    /// Add an element to an array.
    class AddEntryToMapCommand : public SimpleCommand<MapProject> {
      public:
        CLONEABLE(AddEntryToMapCommand);
        AddEntryToMapCommand(std::string commandName, std::unique_ptr<MapEntryData> newEntry,
                             unsigned int indexOfNewEntry);
        AddEntryToMapCommand(const AddEntryToMapCommand& other);

        virtual bool initialize(const MapProject& map) override;
        virtual void execute(MapProject& map) const override;
        virtual void undo(MapProject& map) const override;

      private:
        std::unique_ptr<MapEntryData> m_newEntry;
        unsigned int m_indexOfNewEntry;
    };

} // namespace babelwires