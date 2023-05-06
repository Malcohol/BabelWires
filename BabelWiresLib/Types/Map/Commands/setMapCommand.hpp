/**
 * The command which sets a map to its default state.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <BabelWiresLib/Commands/commands.hpp>

namespace babelwires {
    class MapProject;
    class MapValue;

    /// Add an element to an array feature.
    class SetMapCommand : public SimpleCommand<MapProject> {
      public:
        SetMapCommand(std::string commandName, std::unique_ptr<MapValue> newData);

        virtual bool initialize(const MapProject& map) override;
        virtual void execute(MapProject& map) const override;
        virtual void undo(MapProject& map) const override;

      private:
        std::unique_ptr<MapValue> m_newContents;
        std::unique_ptr<MapValue> m_oldContents;
    };

} // namespace babelwires