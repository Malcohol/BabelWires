/**
 * The command which sets the source type of a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Types/Map/Commands/setMapSourceTypeCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

babelwires::SetMapSourceTypeCommand::SetMapSourceTypeCommand(std::string commandName, TypeExp newSourceTypeExp)
    : SimpleCommand(commandName)
    , m_newSourceTypeExp(std::move(newSourceTypeExp)) {}

bool babelwires::SetMapSourceTypeCommand::initialize(const MapProject& map) {
    const MapProject::AllowedTypes& allowedTypes = map.getAllowedSourceTypeExps();
    const ProjectContext& context = map.getProjectContext();
    const TypeSystem& typeSystem = context.m_typeSystem;
    if (!allowedTypes.isRelatedToSome(typeSystem, m_newSourceTypeExp)) {
        return false;
    }
    m_oldSourceTypeExp = map.getCurrentSourceTypeExp();
    return true;
}

void babelwires::SetMapSourceTypeCommand::execute(MapProject& map) const {
    map.setCurrentSourceTypeExp(m_newSourceTypeExp);
}

void babelwires::SetMapSourceTypeCommand::undo(MapProject& map) const {
    map.setCurrentSourceTypeExp(m_oldSourceTypeExp);
}
