/**
 * A ProjectDataLocation identifies some data within the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ProjectExtra/projectDataLocation.hpp>

#include <Common/Hash/hash.hpp>
#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::ProjectDataLocation::ProjectDataLocation(NodeId elementId, Path pathToValue)
    : DataLocation(std::move(pathToValue))
    , m_nodeId(elementId) {}

babelwires::NodeId babelwires::ProjectDataLocation::getNodeId() const {
    return m_nodeId;
}

std::size_t babelwires::ProjectDataLocation::getHash() const {
    return hash::mixtureOf(DataLocation::getHash(), m_nodeId);
}

bool babelwires::ProjectDataLocation::equals(const DataLocation& other) const {
    if (DataLocation::equals(other)) {
        if (const auto* otherProjectDataLocation = other.as<ProjectDataLocation>()) {
            return m_nodeId == otherProjectDataLocation->m_nodeId;
        }
    }
    return false;
}

void babelwires::ProjectDataLocation::writeToStream(std::ostream& os) const {
    DataLocation::writeToStream(os);
    os << " @ node " << m_nodeId;
}

void babelwires::ProjectDataLocation::serializeContents(Serializer& serializer) const {
    DataLocation::serializeContents(serializer);
    serializer.serializeValue("id", m_nodeId);
}

void babelwires::ProjectDataLocation::deserializeContents(Deserializer& deserializer) {
    DataLocation::deserializeContents(deserializer);
    deserializer.deserializeValue("id", m_nodeId);
}
