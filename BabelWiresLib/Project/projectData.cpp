/**
 * ProjectData carries data sufficient to reconstruct the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/projectData.hpp>

#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>

babelwires::ProjectData::ProjectData(const ProjectData& other) {
    m_nodes.reserve(other.m_nodes.size());
    for (const auto& n : other.m_nodes) {
        m_nodes.emplace_back(n->clone());
    }
}

void babelwires::ProjectData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("id", m_projectId);
    serializer.serializeArray("elements", m_nodes);
}

void babelwires::ProjectData::deserializeContents(Deserializer& deserializer) {
    // Optional - ignore failure
    deserializer.deserializeValue("id", m_projectId);
    auto it = deserializer.deserializeArray<NodeData>("elements");
    while (it.isValid()) {
        m_nodes.emplace_back(std::move(it.getObject()));
        ++it;
    }
}

void babelwires::ProjectData::visitIdentifiers(IdentifierVisitor& visitor) {
    for (auto& m : m_nodes) {
        m->visitIdentifiers(visitor);
    }
}

void babelwires::ProjectData::visitFilePaths(FilePathVisitor& visitor) {
    for (auto& m : m_nodes) {
        m->visitFilePaths(visitor);
    }
}
