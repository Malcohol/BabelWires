/**
 * The command which sets the target type of a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Types/Map/Commands/resetMapValueCommand.hpp>

#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>

babelwires::ResetMapValueCommand::ResetMapValueCommand(std::string commandName, MapProjectDataLocation loc,
                                                   TypeRef type)
    : SimpleCommand(commandName)
    , m_location(std::move(loc))
    , m_type(std::move(type)) {}

bool babelwires::ResetMapValueCommand::initialize(const MapProject& map) {
    // TODO
}

void babelwires::ResetMapValueCommand::execute(MapProject& map) const {
    // TODO
}

void babelwires::ResetMapValueCommand::undo(MapProject& map) const {
    // TODO
}
