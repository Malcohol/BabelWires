/**
 * The command which sets a map to its default state.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <BabelWiresLib/Types/Map/mapValue.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/Commands/commands.hpp>

namespace babelwires {
    class MapProject;

    /// Add an element to an array feature.
    class SetMapCommand : public SimpleCommand<MapProject> {
      public:
        SetMapCommand(std::string commandName, ValueHolderTemplate<MapValue> newData);

        virtual bool initialize(const MapProject& map) override;
        virtual void execute(MapProject& map) const override;
        virtual void undo(MapProject& map) const override;

      private:
        ValueHolderTemplate<MapValue> m_newContents;
        ValueHolderTemplate<MapValue> m_oldContents;
    };

} // namespace babelwires