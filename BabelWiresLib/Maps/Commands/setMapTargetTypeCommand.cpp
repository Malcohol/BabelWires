/**
 * The command which sets the target type of a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWiresLib/Maps/Commands/setMapTargetTypeCommand.hpp"

#include "BabelWiresLib/Maps/MapEntries/mapEntryData.hpp"
#include "BabelWiresLib/Maps/mapProject.hpp"
#include "BabelWiresLib/Maps/mapProjectEntry.hpp"
#include "BabelWiresLib/Project/projectContext.hpp"
#include "BabelWiresLib/TypeSystem/typeSystem.hpp"

babelwires::SetMapTargetTypeCommand::SetMapTargetTypeCommand(std::string commandName, LongIdentifier newTargetTypeId)
    : SimpleCommand(commandName)
    , m_newTargetTypeId(newTargetTypeId) {}

bool babelwires::SetMapTargetTypeCommand::initialize(const MapProject& map) {
    const LongIdentifier allowedTargetType = map.getAllowedTargetTypeId();
    const ProjectContext& context = map.getProjectContext();
    const TypeSystem& typeSystem = context.m_typeSystem;
    if (!typeSystem.isSubType(m_newTargetTypeId, allowedTargetType)) {
        return false;
    }
    m_oldTargetTypeId = map.getTargetTypeId();
    return true;
}

void babelwires::SetMapTargetTypeCommand::execute(MapProject& map) const {
    map.setTargetTypeId(m_newTargetTypeId);
}

void babelwires::SetMapTargetTypeCommand::undo(MapProject& map) const {
    map.setTargetTypeId(m_oldTargetTypeId);
}
