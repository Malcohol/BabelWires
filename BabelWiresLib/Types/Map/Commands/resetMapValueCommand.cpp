/**
 * The command which sets a value within a map back to its default value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Types/Map/Commands/resetMapValueCommand.hpp>

#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/valuePathUtils.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>

babelwires::ResetMapValueCommand::ResetMapValueCommand(std::string commandName, MapProjectDataLocation loc,
                                                       TypeRef type)
    : SimpleCommand(commandName)
    , m_location(std::move(loc))
    , m_type(std::move(type)) {}

bool babelwires::ResetMapValueCommand::initialize(const MapProject& map) {
    if (m_location.getEntryIndex() >= map.getNumMapEntries()) {
        return false;
    }

    const MapProjectEntry& mapEntry = map.getMapEntry(m_location.getEntryIndex());
    const MapEntryData& entryData = mapEntry.getData();

    const EditableValueHolder* entryHolder;
    const Type* typeInMap;
    if (m_location.getSide() == MapProjectDataLocation::Side::source) {
        entryHolder = entryData.tryGetSourceValue();
        typeInMap = map.getCurrentSourceType();
    } else {
        entryHolder = entryData.tryGetTargetValue();
        typeInMap = map.getCurrentTargetType();
    }

    if (!entryHolder) {
        return false;
    }

    // TODO: Paths within values
    assert (m_location.getPathToValue().getNumSteps() == 0);
    /*
    const TypeSystem& typeSystem = map.getProjectContext().m_typeSystem;
    std::optional<std::tuple<const babelwires::Type&, const babelwires::ValueHolder&>> optional =
        tryFollowPath(typeSystem, *typeInMap, m_location.getPathToValue(), *entryHolder);

    if (!optional.has_value()) {
        return false;
    }

    m_oldValue = std::get<1>(optional.value());
    */

    m_oldValue = *entryHolder;
    return true;
}

void babelwires::ResetMapValueCommand::execute(MapProject& map) const {
    assert (m_location.getEntryIndex() < map.getNumMapEntries());
    const MapProjectEntry& mapEntry = map.getMapEntry(m_location.getEntryIndex());
    std::unique_ptr<MapEntryData> entryData = mapEntry.getData().clone();
    const TypeSystem& typeSystem = map.getProjectContext().m_typeSystem;

    /*

    const EditableValueHolder* entryHolder;
    const Type* typeInMap;
    if (m_location.getSide() == MapProjectDataLocation::Side::source) {
        entryHolder = &entryData->getSourceValue();
        typeInMap = map.getCurrentSourceType();
    } else {
        entryHolder = &entryData->getTargetValue();
        typeInMap = map.getCurrentTargetType();
    }


    ValueHolder newEntry = *entryHolder;
    std::tuple<const babelwires::Type&, babelwires::ValueHolder> optional =
        followPathNonConst(typeSystem, *type, m_location.getPathToValue(), newEntry);

    // set value

    if (m_location.getSide() == MapProjectDataLocation::Side::source) {
        entryData.setSourceValue(newEntry);
    } else {
        entryData.setTargetValue(newEntry);
    } 
    */   

    const Type& type = m_type.assertResolve(typeSystem);
    EditableValueHolder newValue = type.createValue(typeSystem).is<EditableValue>();

    if (m_location.getSide() == MapProjectDataLocation::Side::source) {
        entryData->setSourceValue(newValue);
    } else {
        entryData->setTargetValue(newValue);
    }
    map.replaceMapEntry(std::move(entryData), m_location.getEntryIndex());
}

void babelwires::ResetMapValueCommand::undo(MapProject& map) const {
    assert (m_location.getEntryIndex() < map.getNumMapEntries());
    const MapProjectEntry& mapEntry = map.getMapEntry(m_location.getEntryIndex());
    std::unique_ptr<MapEntryData> entryData = mapEntry.getData().clone();

    const TypeSystem& typeSystem = map.getProjectContext().m_typeSystem;

    /*
    const EditableValueHolder* entryHolder;
    const Type* typeInMap;
    if (m_location.getSide() == MapProjectDataLocation::Side::source) {
        entryHolder = &entryData->getSourceValue();
        typeInMap = map.getCurrentSourceType();
    } else {
        entryHolder = &entryData->getTargetValue();
        typeInMap = map.getCurrentTargetType();
    }

    ValueHolder newEntry = *entryHolder;
    std::tuple<const babelwires::Type&, babelwires::ValueHolder> optional =
        followPathNonConst(typeSystem, *type, m_location.getPathToValue(), newEntry);

    // restore value

    if (m_location.getSide() == MapProjectDataLocation::Side::source) {
        entryData.setSourceValue(newEntry);
    } else {
        entryData.setTargetValue(newEntry);
    } 
    */   

    if (m_location.getSide() == MapProjectDataLocation::Side::source) {
        entryData->setSourceValue(m_oldValue);
    } else {
        entryData->setTargetValue(m_oldValue);
    }
    map.replaceMapEntry(std::move(entryData), m_location.getEntryIndex());
}
