/**
 * A ProjectDataLocation identifies some data within the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ProjectExtra/projectDataLocation.hpp>

#include <Common/Hash/hash.hpp>
#include <Common/Serialization/serializer.hpp>
#include <Common/Serialization/deserializer.hpp>

bool babelwires::ProjectDataLocation::operator==(const DataLocation& other) const { 
    if (const auto* otherProjectDataLocation = other.as<ProjectDataLocation>()) {
        return (m_nodeId == otherProjectDataLocation->m_nodeId) && (m_pathToValue == otherProjectDataLocation->m_pathToValue);
    }
    return false;
}

std::string babelwires::ProjectDataLocation::toString() const {
    std::ostringstream os;
    os << "\"" << m_pathToValue << " @ node " << m_nodeId << "\"";
    return os.str();
}

babelwires::ProjectDataLocation::ProjectDataLocation(NodeId elementId, Path pathToValue)
    : m_nodeId(elementId)
    , m_pathToValue(std::move(pathToValue)) {

}

babelwires::NodeId babelwires::ProjectDataLocation::getNodeId() const {
    return m_nodeId;
}

const babelwires::Path& babelwires::ProjectDataLocation::getPathToValue() const {
    return m_pathToValue;
}

std::size_t babelwires::ProjectDataLocation::getHash() const {
    return hash::mixtureOf(m_nodeId, m_pathToValue);
}

void babelwires::ProjectDataLocation::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("id", m_nodeId);
    serializer.serializeValue("path", m_pathToValue);
}

void babelwires::ProjectDataLocation::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("id", m_nodeId);
    deserializer.deserializeValue("path", m_pathToValue);
}

void babelwires::ProjectDataLocation::visitIdentifiers(IdentifierVisitor& visitor) {
    for (auto& s : m_pathToValue) {
        if (s.isField()) {
            visitor(s.getField());
        }
    }
}

void babelwires::ProjectDataLocation::visitFilePaths(FilePathVisitor& visitor) {}
