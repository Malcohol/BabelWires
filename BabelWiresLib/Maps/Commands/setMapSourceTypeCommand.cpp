/**
 * The command which sets the source type of a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Maps/Commands/setMapSourceTypeCommand.hpp>

#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProject.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

babelwires::SetMapSourceTypeCommand::SetMapSourceTypeCommand(std::string commandName, TypeRef newSourceTypeRef)
    : SimpleCommand(commandName)
    , m_newSourceTypeRef(std::move(newSourceTypeRef)) {}

bool babelwires::SetMapSourceTypeCommand::initialize(const MapProject& map) {
    const MapFeature::AllowedTypes& allowedTypes = map.getAllowedSourceTypeRefs();
    const ProjectContext& context = map.getProjectContext();
    const TypeSystem& typeSystem = context.m_typeSystem;
    if (!allowedTypes.isRelatedToSome(typeSystem, m_newSourceTypeRef)) {
        return false;
    }
    m_oldSourceTypeRef = map.getSourceTypeRef();
    return true;
}

void babelwires::SetMapSourceTypeCommand::execute(MapProject& map) const {
    map.setSourceTypeRef(m_newSourceTypeRef);
}

void babelwires::SetMapSourceTypeCommand::undo(MapProject& map) const {
    map.setSourceTypeRef(m_oldSourceTypeRef);
}
