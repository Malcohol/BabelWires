/**
 * The command which sets the target type of a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Types/Map/Commands/setMapTargetTypeCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

babelwires::SetMapTargetTypeCommand::SetMapTargetTypeCommand(std::string commandName, TypeExp newTargetTypeExp)
    : SimpleCommand(commandName)
    , m_newTargetTypeExp(std::move(newTargetTypeExp)) {}

bool babelwires::SetMapTargetTypeCommand::initialize(const MapProject& map) {
    const MapProject::AllowedTypes& allowedTypeExps = map.getAllowedTargetTypeExps();
    const ProjectContext& context = map.getProjectContext();
    const TypeSystem& typeSystem = context.m_typeSystem;
    if (!allowedTypeExps.isSubtypeOfSome(typeSystem, m_newTargetTypeExp)) {
        return false;
    }
    m_oldTargetTypeExp = map.getCurrentTargetTypeExp();
    return true;
}

void babelwires::SetMapTargetTypeCommand::execute(MapProject& map) const {
    map.setCurrentTargetTypeExp(m_newTargetTypeExp);
}

void babelwires::SetMapTargetTypeCommand::undo(MapProject& map) const {
    map.setCurrentTargetTypeExp(m_oldTargetTypeExp);
}
