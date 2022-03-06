/**
 * The command which adds entries to arrays.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include "BabelWiresLib/Commands/commands.hpp"

namespace babelwires {
    class Map;
    class MapEntryData;

    /// Add an element to an array feature.
    class AddEntryToMapCommand : public SimpleCommand<Map> {
      public:
        AddEntryToMapCommand(std::string commandName, std::unique_ptr<MapEntryData> newEntry, unsigned int indexOfNewEntry);

        virtual bool initialize(const Map& map) override;
        virtual void execute(Map& map) const override;
        virtual void undo(Map& map) const override;

      private:
        std::unique_ptr<MapEntryData> m_newEntry;
        unsigned int m_indexOfNewEntry;
    };

} // namespace babelwires