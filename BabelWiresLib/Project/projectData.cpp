/**
 * ProjectData carries data sufficient to reconstruct the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/projectData.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

void babelwires::ProjectData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("id", m_projectId);
    serializer.serializeArray("elements", m_elements);
}

void babelwires::ProjectData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("id", m_projectId, Deserializer::IsOptional::Optional);
    auto it = deserializer.deserializeArray<NodeData>("elements");
    while (it.isValid()) {
        m_elements.emplace_back(std::move(it.getObject()));
        ++it;
    }
}

void babelwires::ProjectData::visitIdentifiers(IdentifierVisitor& visitor) {
    for (auto& m : m_elements) {
        m->visitIdentifiers(visitor);
    }
}

void babelwires::ProjectData::visitFilePaths(FilePathVisitor& visitor) {
    for (auto& m : m_elements) {
        m->visitFilePaths(visitor);
    }
}
