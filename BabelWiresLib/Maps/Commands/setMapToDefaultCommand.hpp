/**
 * The command which modifies the value in a map entries.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include "BabelWiresLib/Commands/commands.hpp"

namespace babelwires {
    class MapProject;
    class MapData;

    /// Add an element to an array feature.
    class SetMapToDefaultCommand : public SimpleCommand<MapProject> {
      public:
        SetMapToDefaultCommand(std::string commandName);

        virtual bool initialize(const MapProject& map) override;
        virtual void execute(MapProject& map) const override;
        virtual void undo(MapProject& map) const override;

      private:
        std::unique_ptr<MapData> m_oldContents;
    };

} // namespace babelwires