/**
 * A ProjectBundle carries ProjectData along with metadata sufficient to version it.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Serialization/projectBundle.hpp>

#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>

#include <BaseLib/DataContext/filePath.hpp>

#include <BaseLib/exceptions.hpp>

babelwires::ProjectBundle::ProjectBundle(std::filesystem::path pathToProjectFile, ProjectData&& projectData)
    : DataBundle(std::move(pathToProjectFile), std::move(projectData)) {
}

void babelwires::ProjectBundle::interpretAdditionalMetadataInCurrentContext() {
    // Capture current factory metadata
    for (const auto& node : getData().m_nodes) {
        VersionNumber& storedVersion = m_factoryMetadata[node->m_factoryIdentifier];
        assert(((storedVersion == 0) || (storedVersion == node->m_factoryVersion)) &&
               "Inconsistent factory versions in ProjectData");
        storedVersion = node->m_factoryVersion;
    }
}

void babelwires::ProjectBundle::adaptDataToAdditionalMetadata(const DataContext& context, UserLogger& userLogger) {
    const ProjectContext& projectContext = static_cast<const ProjectContext&>(context);
    for (auto& node : getData().m_nodes) {
        node->m_factoryVersion = m_factoryMetadata[node->m_factoryIdentifier];
    }

    for (auto& node : getData().m_nodes) {
        // Can log the same message multiple times.
        node->checkFactoryVersion(projectContext, userLogger);
    }
}

namespace {
    struct FactoryInfoPair : babelwires::Serializable {
        SERIALIZABLE(FactoryInfoPair, "factory", void, 1);
        babelwires::LongId m_factoryIdentifier = "nofact";
        babelwires::VersionNumber m_factoryVersion;

        void serializeContents(babelwires::Serializer& serializer) const override {
            serializer.serializeValue("id", m_factoryIdentifier);
            serializer.serializeValue("version", m_factoryVersion);
        }
        babelwires::Result deserializeContents(babelwires::Deserializer& deserializer) override {
            DO_OR_ERROR(deserializer.deserializeValue("id", m_factoryIdentifier));
            DO_OR_ERROR(deserializer.deserializeValue("version", m_factoryVersion));
            return {};
        }
    };
} // namespace

void babelwires::ProjectBundle::serializeAdditionalMetadata(Serializer& serializer) const {
    std::vector<FactoryInfoPair> factoryMetadata;
    for (const auto& [k, v] : m_factoryMetadata) {
        FactoryInfoPair metadata;
        metadata.m_factoryIdentifier = k;
        metadata.m_factoryVersion = v;
        factoryMetadata.emplace_back(metadata);
    }
    serializer.serializeArray("factoryMetadata", factoryMetadata);
}

babelwires::Result babelwires::ProjectBundle::deserializeAdditionalMetadata(Deserializer& deserializer) {
    ASSIGN_OR_ERROR(auto it, deserializer.deserializeArray<FactoryInfoPair>("factoryMetadata"));
    while (it.isValid()) {
        ASSIGN_OR_ERROR(auto fm, it.getObject());
        m_factoryMetadata.insert(std::make_pair(std::move(fm->m_factoryIdentifier), fm->m_factoryVersion));
        DO_OR_ERROR(it.advance());
    }
    return {};
}

void babelwires::ProjectBundle::visitIdentifiers(IdentifierVisitor& visitor) {
    getData().visitIdentifiers(visitor);
    // Visit the identifiers in the factory metadata.
    decltype(m_factoryMetadata) newMap;
    for (const auto& [k, v] : m_factoryMetadata) {
        LongId newKey = k;
        visitor(newKey);
        newMap.insert(std::pair(newKey, v));
    }
    m_factoryMetadata.swap(newMap);
}

void babelwires::ProjectBundle::visitFilePaths(FilePathVisitor& visitor) {
    getData().visitFilePaths(visitor);
}
