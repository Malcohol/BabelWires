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
        AddEntryToMapCommand(std::string commandName, unsigned int indexOfNewEntry);

        virtual bool initialize(const MapProject& map) override;
        virtual void execute(MapProject& map) const override;
        virtual void undo(MapProject& map) const override;

      private:
        unsigned int m_indexOfNewEntry;
    };

} // namespace babelwires