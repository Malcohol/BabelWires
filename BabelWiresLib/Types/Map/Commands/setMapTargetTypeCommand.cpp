/**
 * The command which sets the target type of a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Types/Map/Commands/setMapTargetTypeCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapProject.hpp>
#include <BabelWiresLib/Types/Map/mapProjectEntry.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

babelwires::SetMapTargetTypeCommand::SetMapTargetTypeCommand(std::string commandName, TypeRef newTargetTypeRef)
    : SimpleCommand(commandName)
    , m_newTargetTypeRef(std::move(newTargetTypeRef)) {}

bool babelwires::SetMapTargetTypeCommand::initialize(const MapProject& map) {
    const MapFeature::AllowedTypes& allowedTypeRefs = map.getAllowedTargetTypeRefs();
    const ProjectContext& context = map.getProjectContext();
    const TypeSystem& typeSystem = context.m_typeSystem;
    if (!allowedTypeRefs.isSubtypeOfSome(typeSystem, m_newTargetTypeRef)) {
        return false;
    }
    m_oldTargetTypeRef = map.getTargetTypeRef();
    return true;
}

void babelwires::SetMapTargetTypeCommand::execute(MapProject& map) const {
    map.setTargetTypeRef(m_newTargetTypeRef);
}

void babelwires::SetMapTargetTypeCommand::undo(MapProject& map) const {
    map.setTargetTypeRef(m_oldTargetTypeRef);
}
