/**
 * Functionality common to serialization and deserialization.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/Serialization/serializerDeserializerCommon.hpp>

#include <BaseLib/Log/userLogger.hpp>
#include <BaseLib/Serialization/deserializationRegistry.hpp>
#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>

#include <cassert>

babelwires::VersionNumber babelwires::SerializerDeserializerCommon::getTypeVersion(std::string_view typeName) {
    const auto it = m_serializationVersions.find(typeName);
    if (it != m_serializationVersions.end()) {
        return it->second;
    }
    return 0;
}

void babelwires::SerializerDeserializerCommon::recordVersion(std::string_view typeName, VersionNumber typeVersion) {
    const auto [it, wasInserted] = m_serializationVersions.insert(std::pair(typeName, typeVersion));
    if (!wasInserted) {
        assert((typeVersion == it->second) && "The types in SerializableBase::s_entryListHead should all be unique");
    }
}

namespace {
    struct SerializationMetadata : babelwires::Serializable {
        SERIALIZABLE(SerializationMetadata, "serializable", void, 1);
        std::string m_type;
        babelwires::VersionNumber m_version;

        SerializationMetadata() = default;

        SerializationMetadata(std::string_view t, babelwires::VersionNumber v)
            : m_type(t)
            , m_version(v) {}

        void serializeContents(babelwires::Serializer& serializer) const override {
            serializer.serializeValue("type", m_type);
            serializer.serializeValue("version", m_version);
        }

        babelwires::Result deserializeContents(babelwires::Deserializer& deserializer) override {
            DO_OR_ERROR(deserializer.deserializeValue("type", m_type));
            DO_OR_ERROR(deserializer.deserializeValue("version", m_version));
            return {};
        }
    };
} // namespace

void babelwires::SerializerDeserializerCommon::serializeMetadata(Serializer& serializer) {
    std::vector<SerializationMetadata> metadata;
    for (auto& [t, v] : m_serializationVersions) {
        metadata.emplace_back(SerializationMetadata(t, v));
    }
    serializer.serializeArray("serializationMetadata", metadata);
}

babelwires::Result babelwires::SerializerDeserializerCommon::deserializeMetadata(
    Deserializer& deserializer, UserLogger& userLogger, const DeserializationRegistry& deserializationRegistry) {
    ASSIGN_OR_ERROR(auto it, deserializer.deserializeArray<SerializationMetadata>("serializationMetadata"));
    for (; it.isValid(); ++it) {
        ASSIGN_OR_ERROR(auto ptr, it.getObject());
        const auto [_, wasInserted] = m_serializationVersions.insert(std::pair(ptr->m_type, ptr->m_version));
        if (!wasInserted) {
            return Error() << "The type \"" << ptr->m_type << "\" already has a version";
        }
        if (ptr->m_version == 0) {
            return Error() << "The type \"" << ptr->m_type
                                << "\" has version 0, but this is not a meaningful version.";
        }
        const DeserializationRegistry::Entry* entry = deserializationRegistry.findEntry(ptr->m_type);
        if (entry && entry->m_version < ptr->m_version) {
            userLogger.logWarning() << "The type \"" << ptr->m_type << "\" being loaded has version " << ptr->m_version
                                    << ", but the latest version known to this software is " << entry->m_version
                                    << ". The data was possibly created by a newer version of this software, and it "
                                        "may not load correctly.";
        }
    }
    return {};
}
