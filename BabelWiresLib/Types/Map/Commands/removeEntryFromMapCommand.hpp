/**
 * The command to remove entries from arrays.
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
    class RemoveEntryFromMapCommand : public SimpleCommand<MapProject> {
      public:
        CLONEABLE(RemoveEntryFromMapCommand);
        RemoveEntryFromMapCommand(std::string commandName, unsigned int indexOfEntryToRemove);
        RemoveEntryFromMapCommand(const RemoveEntryFromMapCommand& other);
        ~RemoveEntryFromMapCommand();

        virtual bool initialize(const MapProject& map) override;
        virtual void execute(MapProject& map) const override;
        virtual void undo(MapProject& map) const override;

      private:
        unsigned int m_indexOfEntryToRemove;

        // Post initialization data

        std::unique_ptr<MapEntryData> m_removedEntry;
    };

} // namespace babelwires