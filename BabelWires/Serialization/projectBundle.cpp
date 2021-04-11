/**
 * A ProjectBundle carries ProjectData along with metadata sufficient to version it.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Serialization/projectBundle.hpp"

#include "Common/exceptions.hpp"

#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"

namespace {
    template <typename SOURCE_REG, typename TARGET_REG>
    void convertPath(const babelwires::FeaturePath& path, SOURCE_REG& sourceReg, TARGET_REG& targetReg,
                     babelwires::FieldNameRegistry::Authority authority) {
        for (const auto& p : path) {
            if (p.isField()) {
                babelwires::FieldIdentifier sourceId = p.getField();
                // Unregistered fields stay unregistered.
                if (sourceId.getDiscriminator() != 0) {
                    babelwires::FieldIdentifier newId = sourceId;
                    newId.setDiscriminator(0);
                    // This can throw, but an exception here is only meaningful in the loading case.
                    // In the saving case, the exception is caught and triggers an assertion.
                    babelwires::FieldNameRegistry::ValueType fieldData = sourceReg->getDeserializedFieldData(sourceId);
                    newId = targetReg->addFieldName(newId, *std::get<1>(fieldData), *std::get<2>(fieldData), authority);
                    p.getField().setDiscriminator(newId.getDiscriminator());
                }
            }
        }
    }

    template <typename SOURCE_REG, typename TARGET_REG>
    void convertProjectData(babelwires::ProjectData& projectData, SOURCE_REG&& sourceReg, TARGET_REG&& targetReg,
                            babelwires::FieldNameRegistry::Authority authority) {
        for (const auto& element : projectData.m_elements) {
            // TODO Obtain paths via a query on modifierData, rather than assuming structure.
            for (const auto& modifier : element->m_modifiers) {
                convertPath<SOURCE_REG, TARGET_REG>(modifier->m_pathToFeature, sourceReg, targetReg, authority);
                if (const auto* assignModifier =
                        dynamic_cast<const babelwires::AssignFromFeatureData*>(modifier.get())) {
                    convertPath<SOURCE_REG, TARGET_REG>(assignModifier->m_pathToSourceFeature, sourceReg, targetReg,
                                                        authority);
                }
            }
            for (const auto& path : element->m_expandedPaths) {
                convertPath<SOURCE_REG, TARGET_REG>(path, sourceReg, targetReg, authority);
            }
        }
    }
} // namespace

babelwires::ProjectBundle::ProjectBundle() = default;

babelwires::ProjectBundle::ProjectBundle(ProjectData&& projectData)
    : m_projectData(std::move(projectData)) {
    FieldNameRegistry::ReadAccess sourceRegistry = FieldNameRegistry::read();
    try {
        convertProjectData(m_projectData, sourceRegistry, &m_fieldNameRegistry,
                           FieldNameRegistry::Authority::isTemporary);
    } catch (const ParseException& e) {
        assert(false && "A field with a discriminator did not resolve.");
    }

    for (const auto& element : m_projectData.m_elements) {
        VersionNumber& storedVersion = m_metadata.m_factoryMetadata[element->m_factoryIdentifier];
        assert(((storedVersion == 0) || (storedVersion == element->m_factoryVersion)) &&
               "Inconsistent factory versions in ProjectData");
        storedVersion = element->m_factoryVersion;
    }
}

babelwires::ProjectData babelwires::ProjectBundle::resolveAgainstCurrentContext(const ProjectContext& context,
                                                                                UserLogger& userLogger) && {
    FieldNameRegistry::WriteAccess targetRegistry = FieldNameRegistry::write();
    convertProjectData(m_projectData, &m_fieldNameRegistry, targetRegistry,
                       FieldNameRegistry::Authority::isProvisional);

    for (auto& element : m_projectData.m_elements) {
        element->m_factoryVersion = m_metadata.m_factoryMetadata[element->m_factoryIdentifier];
    }

    for (auto& element : m_projectData.m_elements) {
        // Can log the same message multiple times.
        element->checkFactoryVersion(context, userLogger);
    }

    return std::move(m_projectData);
}

namespace {
    struct FactoryMetadata : babelwires::Serializable {
        SERIALIZABLE(FactoryMetadata, "factory", void, 1);
        std::string m_factoryIdentifier;
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
    serializer.serializeObject(m_projectData);
    serializer.serializeObject(m_fieldNameRegistry);
    std::vector<FactoryMetadata> factoryMetadata;
    for (const auto& [k, v] : m_metadata.m_factoryMetadata) {
        FactoryMetadata metadata;
        metadata.m_factoryIdentifier = k;
        metadata.m_factoryVersion = v;
        factoryMetadata.emplace_back(metadata);
    }
    serializer.serializeArray("factoryMetadata", factoryMetadata);
}

void babelwires::ProjectBundle::deserializeContents(Deserializer& deserializer) {
    m_projectData = std::move(*deserializer.deserializeObject<ProjectData>(ProjectData::serializationType));
    m_fieldNameRegistry =
        std::move(*deserializer.deserializeObject<FieldNameRegistry>(FieldNameRegistry::serializationType));
    for (auto it = deserializer.deserializeArray<FactoryMetadata>("factoryMetadata"); it.isValid(); ++it) {
        auto fm = it.getObject();
        m_metadata.m_factoryMetadata.insert(std::make_pair(std::move(fm->m_factoryIdentifier), fm->m_factoryVersion));
    }
}
