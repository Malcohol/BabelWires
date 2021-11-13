/**
 * A ProjectBundle carries ProjectData along with metadata sufficient to version it.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Serialization/projectBundle.hpp"

#include "BabelWiresLib/FileFormat/filePath.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifierData.hpp"

#include "Common/exceptions.hpp"

#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"

namespace {
    template <typename SOURCE_REG, typename TARGET_REG> struct IdentifierVisitor : babelwires::IdentifierVisitor {
        IdentifierVisitor(const SOURCE_REG& sourceReg, TARGET_REG& targetReg,
                            babelwires::IdentifierRegistry::Authority authority)
            : m_sourceReg(sourceReg)
            , m_targetReg(targetReg)
            , m_authority(authority) {}

        template <typename IDENTIFIER> void visit(IDENTIFIER& sourceId) {
            if (sourceId.getDiscriminator() != 0) {
                IDENTIFIER newId = sourceId;
                newId.setDiscriminator(0);
                // This can throw, but an exception here is only meaningful in the loading case.
                // In the saving case, the exception is caught and triggers an assertion.
                const babelwires::IdentifierRegistry::ValueType fieldData =
                    m_sourceReg->getDeserializedIdentifierData(sourceId);
                newId = m_targetReg->addIdentifierWithMetadata(newId, *std::get<1>(fieldData), *std::get<2>(fieldData),
                                                             m_authority);
                sourceId.setDiscriminator(newId.getDiscriminator());
            }
        }

        void operator()(babelwires::Identifier& identifier) override { visit(identifier); }
        void operator()(babelwires::LongIdentifier& identifier) override { visit(identifier); }

        const SOURCE_REG& m_sourceReg;
        TARGET_REG& m_targetReg;
        babelwires::IdentifierRegistry::Authority m_authority;
    };

    template <typename SOURCE_REG, typename TARGET_REG>
    void convertProjectData(babelwires::ProjectData& projectData, SOURCE_REG&& sourceReg, TARGET_REG&& targetReg,
                            babelwires::IdentifierRegistry::Authority authority) {
        IdentifierVisitor<SOURCE_REG, TARGET_REG> visitor(sourceReg, targetReg, authority);
        projectData.visitIdentifiers(visitor);
    }
} // namespace

babelwires::ProjectBundle::ProjectBundle() = default;

babelwires::ProjectBundle::ProjectBundle(std::filesystem::path pathToProjectFile, ProjectData&& projectData)
    : m_projectData(std::move(projectData))
    , m_projectFilePath(pathToProjectFile) {
    interpretIdentifiersInCurrentContext();
    interpretFilePathsInCurrentProjectPath();
    captureCurrentFactoryMetadata();
}

babelwires::ProjectData babelwires::ProjectBundle::resolveAgainstCurrentContext(
    const ProjectContext& context, const std::filesystem::path& pathToProjectFile, UserLogger& userLogger) && {
    resolveIdentifiersAgainstCurrentContext();
    resolveFilePathsAgainstCurrentProjectPath(pathToProjectFile, userLogger);
    adaptDataToCurrentFactories(context, userLogger);
    return std::move(m_projectData);
}

void babelwires::ProjectBundle::interpretIdentifiersInCurrentContext() {
    IdentifierRegistry::ReadAccess sourceRegistry = IdentifierRegistry::read();
    try {
        convertProjectData(m_projectData, sourceRegistry, &m_identifierRegistry,
                           IdentifierRegistry::Authority::isTemporary);
    } catch (const ParseException&) {
        assert(false && "A field with a discriminator did not resolve.");
    }
}

void babelwires::ProjectBundle::interpretFilePathsInCurrentProjectPath() {
    if (!m_projectFilePath.empty()) {
        const std::filesystem::path projectPath = m_projectFilePath;
        babelwires::FilePathVisitor visitor = [&](FilePath& filePath) {
            filePath.interpretRelativeTo(projectPath.parent_path());
        };
        m_projectData.visitFilePaths(visitor);
    }
}

void babelwires::ProjectBundle::captureCurrentFactoryMetadata() {
    for (const auto& element : m_projectData.m_elements) {
        VersionNumber& storedVersion = m_factoryMetadata[element->m_factoryIdentifier];
        assert(((storedVersion == 0) || (storedVersion == element->m_factoryVersion)) &&
               "Inconsistent factory versions in ProjectData");
        storedVersion = element->m_factoryVersion;
    }
}

void babelwires::ProjectBundle::resolveIdentifiersAgainstCurrentContext() {
    IdentifierRegistry::WriteAccess targetRegistry = IdentifierRegistry::write();
    convertProjectData(m_projectData, &m_identifierRegistry, targetRegistry,
                       IdentifierRegistry::Authority::isProvisional);
}

void babelwires::ProjectBundle::resolveFilePathsAgainstCurrentProjectPath(
    const std::filesystem::path& pathToProjectFile, UserLogger& userLogger) {
    const std::filesystem::path newBase =
        pathToProjectFile.empty() ? std::filesystem::path() : pathToProjectFile.parent_path();
    const std::filesystem::path oldBase =
        m_projectFilePath.empty() ? std::filesystem::path() : std::filesystem::path(m_projectFilePath).parent_path();
    babelwires::FilePathVisitor visitor = [&](FilePath& filePath) {
        filePath.resolveRelativeTo(newBase, oldBase, userLogger);
    };
    m_projectData.visitFilePaths(visitor);
}

void babelwires::ProjectBundle::adaptDataToCurrentFactories(const ProjectContext& context, UserLogger& userLogger) {
    for (auto& element : m_projectData.m_elements) {
        element->m_factoryVersion = m_factoryMetadata[element->m_factoryIdentifier];
    }

    for (auto& element : m_projectData.m_elements) {
        // Can log the same message multiple times.
        element->checkFactoryVersion(context, userLogger);
    }
}

namespace {
    struct FactoryInfoPair : babelwires::Serializable {
        SERIALIZABLE(FactoryInfoPair, "factory", void, 1);
        babelwires::LongIdentifier m_factoryIdentifier = "nofact";
        babelwires::VersionNumber m_factoryVersion;

        void serializeContents(babelwires::Serializer& serializer) const override {
            serializer.serializeValue("id", m_factoryIdentifier);
            serializer.serializeValue("version", m_factoryVersion);
        }
        void deserializeContents(babelwires::Deserializer& deserializer) override {
            deserializer.deserializeValue("id", m_factoryIdentifier);
            deserializer.deserializeValue("version", m_factoryVersion);
        }
    };
} // namespace

void babelwires::ProjectBundle::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("filePath", m_projectFilePath);
    serializer.serializeObject(m_projectData);
    std::vector<FactoryInfoPair> factoryMetadata;
    for (const auto& [k, v] : m_factoryMetadata) {
        FactoryInfoPair metadata;
        metadata.m_factoryIdentifier = k;
        metadata.m_factoryVersion = v;
        factoryMetadata.emplace_back(metadata);
    }
    serializer.serializeArray("factoryMetadata", factoryMetadata);
    serializer.serializeObject(m_identifierRegistry);
}

void babelwires::ProjectBundle::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("filePath", m_projectFilePath, babelwires::Deserializer::IsOptional::Optional);
    m_projectData = std::move(*deserializer.deserializeObject<ProjectData>(ProjectData::serializationType));
    for (auto it = deserializer.deserializeArray<FactoryInfoPair>("factoryMetadata"); it.isValid(); ++it) {
        auto fm = it.getObject();
        m_factoryMetadata.insert(std::make_pair(std::move(fm->m_factoryIdentifier), fm->m_factoryVersion));
    }
    m_identifierRegistry =
        std::move(*deserializer.deserializeObject<IdentifierRegistry>(IdentifierRegistry::serializationType));
}
