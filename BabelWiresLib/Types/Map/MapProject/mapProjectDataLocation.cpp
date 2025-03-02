/**
 * A MapProjectDataLocation identifies some data within the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Map/MapProject/mapProjectDataLocation.hpp>

#include <Common/Hash/hash.hpp>
#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

bool babelwires::MapProjectDataLocation::equals(const DataLocation& other) const {
    if (const auto* otherMapProjectDataLocation = other.as<MapProjectDataLocation>()) {
        return (m_entryIndex == otherMapProjectDataLocation->m_entryIndex) &&
               (m_side == otherMapProjectDataLocation->m_side) &&
               (m_pathToValue == otherMapProjectDataLocation->m_pathToValue);
    }
    return false;
}

void babelwires::MapProjectDataLocation::writeToStream(std::ostream& os) const {
    os << "\"" << m_pathToValue << " @ [" << m_entryIndex << "]("
       << ((m_side == Side::source) ? "source" : "target") << ")\"";
}

babelwires::MapProjectDataLocation::MapProjectDataLocation(unsigned int entryIndex, Side sourceOrTarget,
                                                           Path pathToValue)
    : m_entryIndex(entryIndex)
    , m_side(sourceOrTarget)
    , m_pathToValue(std::move(pathToValue)) {}

unsigned int babelwires::MapProjectDataLocation::getEntryIndex() const {
    return m_entryIndex;
}

babelwires::MapProjectDataLocation::Side babelwires::MapProjectDataLocation::getSide() const {
    return m_side;
}

const babelwires::Path& babelwires::MapProjectDataLocation::getPath() const {
    return m_pathToValue;
}

std::size_t babelwires::MapProjectDataLocation::getHash() const {
    return hash::mixtureOf(m_entryIndex, m_side, m_pathToValue);
}

void babelwires::MapProjectDataLocation::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("entryIndex", m_entryIndex);
    serializer.serializeValue("side", (m_side == Side::source) ? "source" : "target");
    serializer.serializeValue("path", m_pathToValue);
}

void babelwires::MapProjectDataLocation::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("entryIndex", m_entryIndex);
    std::string tmp;
    deserializer.deserializeValue("side", tmp);
    if (tmp == "source") {
        m_side = Side::source;
    } else if (tmp == "target") {
        m_side = Side::target;
    } else {
        throw ParseException() << "Unrecognized side \"" << tmp << "\" when parsing a MapDataLocation";
    }
    deserializer.deserializeValue("path", m_pathToValue);
}

void babelwires::MapProjectDataLocation::visitIdentifiers(IdentifierVisitor& visitor) {}

void babelwires::MapProjectDataLocation::visitFilePaths(FilePathVisitor& visitor) {}
