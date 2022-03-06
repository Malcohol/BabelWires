/**
 * The command to remove entries from arrays.
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
    class RemoveEntryFromMapCommand : public SimpleCommand<Map> {
      public:
        RemoveEntryFromMapCommand(std::string commandName, unsigned int indexOfEntryToRemove);

        virtual bool initialize(const Map& map) override;
        virtual void execute(Map& map) const override;
        virtual void undo(Map& map) const override;

      private:
        unsigned int m_indexOfEntryToRemove;
        std::unique_ptr<MapEntryData> m_removedEntry;
    };

} // namespace babelwires