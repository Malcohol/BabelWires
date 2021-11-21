/**
 * A Map defines a how values between two types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/map.hpp>

#include <BabelWiresLib/Maps/mapEntry.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::LongIdentifier babelwires::getIntTypeId() {
    return REGISTERED_LONGID("int", "Integer", "76f26102-b217-45ac-82cd-20a8d0bf6ae7");
}

babelwires::Map::Map()
    : m_sourceId(getIntTypeId())
    , m_targetId(getIntTypeId()) {}

babelwires::Map::Map(Map&& other)
    : m_sourceId(other.m_sourceId)
    , m_targetId(other.m_targetId)
    , m_mapEntries(std::move(other.m_mapEntries)) {}

babelwires::Map::~Map() = default;

babelwires::LongIdentifier babelwires::Map::getSourceId() const {
    return m_sourceId;
}

babelwires::LongIdentifier babelwires::Map::getTargetId() const {
    return m_targetId;
}

void babelwires::Map::setSourceId(LongIdentifier sourceId) {
    m_sourceId = sourceId;
}

void babelwires::Map::setTargetId(LongIdentifier targetId) {
    m_targetId = targetId;
}

bool babelwires::Map::operator==(const Map& other) const {
    if ((m_sourceId != other.m_sourceId) || (m_targetId != other.m_targetId)) {
        return false;
    }
    return m_mapEntries == other.m_mapEntries;
}

bool babelwires::Map::operator!=(const Map& other) const {
    return m_mapEntries != other.m_mapEntries;
}

std::size_t babelwires::Map::getHash() const {
    std::size_t h = hash::mixtureOf(m_sourceId, m_targetId);
    for (const auto& e : m_mapEntries) {
        hash::mixInto(h, *e);
    }
    return h;
}
