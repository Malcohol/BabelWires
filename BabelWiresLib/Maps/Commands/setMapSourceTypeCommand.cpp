/**
 * The command which sets the source type of a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWiresLib/Maps/Commands/setMapSourceTypeCommand.hpp"

#include "BabelWiresLib/Maps/MapEntries/mapEntryData.hpp"
#include "BabelWiresLib/Maps/mapProject.hpp"
#include "BabelWiresLib/Maps/mapProjectEntry.hpp"
#include "BabelWiresLib/Project/projectContext.hpp"
#include "BabelWiresLib/TypeSystem/typeSystem.hpp"

babelwires::SetMapSourceTypeCommand::SetMapSourceTypeCommand(std::string commandName, LongIdentifier newSourceTypeId)
    : SimpleCommand(commandName)
    , m_newSourceTypeId(newSourceTypeId) {}

bool babelwires::SetMapSourceTypeCommand::initialize(const MapProject& map) {
    const LongIdentifier allowedSourceType = map.getAllowedSourceTypeId();
    const ProjectContext& context = map.getProjectContext();
    const TypeSystem& typeSystem = context.m_typeSystem;
    if (!typeSystem.isSubType(allowedSourceType, m_newSourceTypeId)) {
        return false;
    }
    m_oldSourceTypeId = map.getSourceTypeId();
    return true;
}

void babelwires::SetMapSourceTypeCommand::execute(MapProject& map) const {
    map.setSourceTypeId(m_newSourceTypeId);
}

void babelwires::SetMapSourceTypeCommand::undo(MapProject& map) const {
    map.setSourceTypeId(m_oldSourceTypeId);
}
