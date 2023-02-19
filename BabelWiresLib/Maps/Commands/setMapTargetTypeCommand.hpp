/**
 * The command which sets the target type of a map.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <Common/Identifiers/identifier.hpp>

namespace babelwires {
    class MapProject;
    class MapData;

    /// Add an element to an array feature.
    class SetMapTargetTypeCommand : public SimpleCommand<MapProject> {
      public:
        SetMapTargetTypeCommand(std::string commandName, TypeRef newTargetTypeId);

        virtual bool initialize(const MapProject& map) override;
        virtual void execute(MapProject& map) const override;
        virtual void undo(MapProject& map) const override;

      private:
        TypeRef m_newTargetTypeId;
        TypeRef m_oldTargetTypeId;
    };

} // namespace babelwires
