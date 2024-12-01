/**
 * A DataLocation carries enough data to reconstruct a ComplexValueEditor.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ProjectExtra/dataLocation.hpp>

#include <Common/Hash/hash.hpp>
#include <Common/Serialization/serializer.hpp>
#include <Common/Serialization/deserializer.hpp>

babelwires::DataLocation::DataLocation(NodeId elementId, Path pathToValue)
    : m_elementId(elementId)
    , m_pathToValue(std::move(pathToValue)) {

}

babelwires::NodeId babelwires::DataLocation::getNodeId() const {
    return m_elementId;
}

const babelwires::Path& babelwires::DataLocation::getPathToValue() const {
    return m_pathToValue;
}

std::size_t babelwires::DataLocation::getHash() const {
    return hash::mixtureOf(m_elementId, m_pathToValue);
}

void babelwires::DataLocation::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("id", m_elementId);
    serializer.serializeValue("path", m_pathToValue);
}

void babelwires::DataLocation::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("id", m_elementId);
    deserializer.deserializeValue("path", m_pathToValue);
}

void babelwires::DataLocation::visitIdentifiers(IdentifierVisitor& visitor) {
    for (auto& s : m_pathToValue) {
        if (s.isField()) {
            visitor(s.getField());
        }
    }
}

void babelwires::DataLocation::visitFilePaths(FilePathVisitor& visitor) {}
