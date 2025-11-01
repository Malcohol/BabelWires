/**
 * A ListProjectDataLocation identifies some data within the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/List/ListProject/listProjectDataLocation.hpp>

#include <Common/Hash/hash.hpp>
#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

bool babelwires::ListProjectDataLocation::equals(const DataLocation& other) const {
    if (DataLocation::equals(other)) {
        if (const auto* otherListProjectDataLocation = other.as<ListProjectDataLocation>()) {
            return (m_entryIndex == otherListProjectDataLocation->m_entryIndex);
        }
    }
    return false;
}

void babelwires::ListProjectDataLocation::writeToStream(std::ostream& os) const {
    DataLocation::writeToStream(os);
    os << " @ [" << m_entryIndex << "]";
}

babelwires::ListProjectDataLocation::ListProjectDataLocation(unsigned int entryIndex, Path pathToValue)
    : DataLocation(std::move(pathToValue))
    , m_entryIndex(entryIndex) {}

unsigned int babelwires::ListProjectDataLocation::getEntryIndex() const {
    return m_entryIndex;
}

std::size_t babelwires::ListProjectDataLocation::getHash() const {
    return hash::mixtureOf(DataLocation::getHash(), m_entryIndex);
}

void babelwires::ListProjectDataLocation::serializeContents(Serializer& serializer) const {
    DataLocation::serializeContents(serializer);
    serializer.serializeValue("entryIndex", m_entryIndex);
}

void babelwires::ListProjectDataLocation::deserializeContents(Deserializer& deserializer) {
    DataLocation::deserializeContents(deserializer);
    deserializer.deserializeValue("entryIndex", m_entryIndex);
    std::string tmp;
}
