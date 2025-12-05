/**
 * The command which sets a map to its default state.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>

namespace babelwires {
    class MapProject;

    /// Add an element to an array.
    class SetMapCommand : public SimpleCommand<MapProject> {
      public:
        CLONEABLE(SetMapCommand);
        SetMapCommand(std::string commandName, ValueHolder newData);

        virtual bool initialize(const MapProject& map) override;
        virtual void execute(MapProject& map) const override;
        virtual void undo(MapProject& map) const override;

      private:
        ValueHolder m_newContents;

        // Post initialization data

        ValueHolder m_oldContents;
    };

} // namespace babelwires