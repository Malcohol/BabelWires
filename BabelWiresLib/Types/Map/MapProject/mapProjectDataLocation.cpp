/**
 * A MapProjectDataLocation identifies some data within the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Map/MapProject/mapProjectDataLocation.hpp>

#include <BaseLib/Hash/hash.hpp>
#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>

bool babelwires::MapProjectDataLocation::equals(const DataLocation& other) const {
    if (DataLocation::equals(other)) {
        if (const auto* otherMapProjectDataLocation = other.tryAs<MapProjectDataLocation>()) {
            return (m_entryIndex == otherMapProjectDataLocation->m_entryIndex) &&
                   (m_side == otherMapProjectDataLocation->m_side);
        }
    }
    return false;
}

void babelwires::MapProjectDataLocation::writeToStream(std::ostream& os) const {
    DataLocation::writeToStream(os);
    os << " @ [" << m_entryIndex << "](" << ((m_side == Side::source) ? "source" : "target") << ")";
}

babelwires::MapProjectDataLocation::MapProjectDataLocation(unsigned int entryIndex, Side sourceOrTarget,
                                                           Path pathToValue)
    : DataLocation(std::move(pathToValue))
    , m_entryIndex(entryIndex)
    , m_side(sourceOrTarget) {}

unsigned int babelwires::MapProjectDataLocation::getEntryIndex() const {
    return m_entryIndex;
}

babelwires::MapProjectDataLocation::Side babelwires::MapProjectDataLocation::getSide() const {
    return m_side;
}

std::size_t babelwires::MapProjectDataLocation::getHash() const {
    return hash::mixtureOf(DataLocation::getHash(), m_entryIndex, m_side);
}

void babelwires::MapProjectDataLocation::serializeContents(Serializer& serializer) const {
    DataLocation::serializeContents(serializer);
    serializer.serializeValue("entryIndex", m_entryIndex);
    serializer.serializeValue("side", (m_side == Side::source) ? "source" : "target");
}

void babelwires::MapProjectDataLocation::deserializeContents(Deserializer& deserializer) {
    DataLocation::deserializeContents(deserializer);
    THROW_ON_ERROR(deserializer.deserializeValue("entryIndex", m_entryIndex), ParseException);
    std::string tmp;
    THROW_ON_ERROR(deserializer.deserializeValue("side", tmp), ParseException);
    if (tmp == "source") {
        m_side = Side::source;
    } else if (tmp == "target") {
        m_side = Side::target;
    } else {
        throw ParseException() << "Unrecognized side \"" << tmp << "\" when parsing a MapDataLocation";
    }
}
