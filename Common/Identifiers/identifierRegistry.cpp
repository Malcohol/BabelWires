/**
 * A singleton that associates global metadata with identifiers.
 * 
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "Common/Identifiers/identifierRegistry.hpp"

#include "Common/Log/debugLogger.hpp"
#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"
#include "Common/exceptions.hpp"

babelwires::IdentifierRegistry::IdentifierRegistry() = default;
babelwires::IdentifierRegistry::IdentifierRegistry(IdentifierRegistry&&) = default;
babelwires::IdentifierRegistry::~IdentifierRegistry() = default;
babelwires::IdentifierRegistry& babelwires::IdentifierRegistry::operator=(IdentifierRegistry&&) = default;

babelwires::IdentifierRegistry::InstanceData::InstanceData()
    : m_uuid("00000000-0000-0000-0000-000000000000")
    , m_identifier("Invald")
    , m_authority(Authority::isProvisional) {}

babelwires::IdentifierRegistry::InstanceData::InstanceData(std::string fieldName, Uuid uuid, Identifier identifier,
                                                          Authority authority)
    : m_fieldName(std::move(fieldName))
    , m_uuid(std::move(uuid))
    , m_identifier(identifier)
    , m_authority(authority) {}

void babelwires::IdentifierRegistry::InstanceData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("id", m_identifier);
    serializer.serializeValue("name", m_fieldName);
    serializer.serializeValue("uuid", m_uuid);
}

void babelwires::IdentifierRegistry::InstanceData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("id", m_identifier);
    deserializer.deserializeValue("name", m_fieldName);
    deserializer.deserializeValue("uuid", m_uuid);
    m_authority = Authority::isProvisional;
}

babelwires::Identifier babelwires::IdentifierRegistry::addIdentifierWithMetadata(babelwires::Identifier identifier,
                                                                        const std::string& name, const Uuid& uuid,
                                                                        Authority authority) {
    const Identifier::Discriminator discriminator = identifier.getDiscriminator();
    assert((discriminator == 0) && "The identifier already has a discriminator: Did you already register it?");

    const auto it = m_uuidToInstanceDataMap.find(uuid);
    if (it == m_uuidToInstanceDataMap.end()) {
        if (authority != Authority::isTemporary) {
            logDebug() << ((authority == Authority::isAuthoritative) ? "Authoritatively" : "Provisionally")
                       << " registering Identifier " << identifier << " as \"" << name << "\"";
        }

        auto [uit, _] = m_uuidToInstanceDataMap.insert(
            std::pair(uuid, std::make_unique<InstanceData>(name, uuid, identifier, authority)));

        Data& data = m_instanceDatasFromIdentifier[identifier];

        const int newDiscriminator = data.m_instanceDatas.size() + 1;
        // I could fail safe here, but it seems very unlikely to happen. If it does, then the system needs a rethink.
        assert((newDiscriminator <= Identifier::c_maxDiscriminator) && "Too many duplicate identifiers");
        data.m_instanceDatas.emplace_back(uit->second.get());
        identifier.setDiscriminator(newDiscriminator);
        uit->second->m_identifier = identifier;
    } else {
        InstanceData& instanceData = *it->second;
        if (authority != Authority::isAuthoritative) {
            // Since we're loading from a file or saving into a temporary registry, just take whatever is in memory.
            identifier = instanceData.m_identifier;
        } else {
            // We may be registering from code at some point after the identifier has been registered from a file.
            // In this case, update the stored data.
            assert((((instanceData.m_authority == Authority::isProvisional) ||
                     ((instanceData.m_identifier == identifier) && instanceData.m_fieldName == name))) &&
                   "Uuid is registered twice from code with inconsistent data");
            logDebug() << "Authoritatively updating Identifier " << identifier << " as \"" << name << "\"";
            instanceData.m_fieldName = name;
            instanceData.m_authority = authority;
            // TODO Warn when identifier has changed in a file, and we register subsequently.
            // Some kind of versioning will be required to support this.
            identifier = instanceData.m_identifier;
        }
    }

    return identifier;
}

const babelwires::IdentifierRegistry::InstanceData*
babelwires::IdentifierRegistry::getInstanceData(Identifier identifier) const {
    const babelwires::Identifier::Discriminator index = identifier.getDiscriminator();
    if (index > 0) {
        identifier.setDiscriminator(0);
        const auto& it = m_instanceDatasFromIdentifier.find(identifier);
        if (it != m_instanceDatasFromIdentifier.end()) {
            const Data& data = it->second;
            if (index <= data.m_instanceDatas.size()) {
                return data.m_instanceDatas[index - 1];
            }
        }
    }
    return nullptr;
}

babelwires::IdentifierRegistry::ValueType
babelwires::IdentifierRegistry::getDeserializedIdentifierData(Identifier identifier) const {
    if (const babelwires::IdentifierRegistry::InstanceData* data = getInstanceData(identifier)) {
        return ValueType{identifier, &data->m_fieldName, &data->m_uuid};
    }
    throw ParseException() << "Identifier \"" << identifier
                           << "\" not found in the identifier metadata. Note that unregistered fields (those with no "
                              "discriminator) are allowed";
}

std::string babelwires::IdentifierRegistry::getName(babelwires::Identifier identifier) const {
    if (const InstanceData* data = getInstanceData(identifier)) {
        return data->m_fieldName;
    }
    // TODO
    return identifier.serializeToString();
}

std::shared_mutex babelwires::IdentifierRegistry::s_mutex;
babelwires::IdentifierRegistry* babelwires::IdentifierRegistry::s_singletonInstance;

babelwires::IdentifierRegistry::ReadAccess::ReadAccess(std::shared_mutex* mutex, const IdentifierRegistry* registry)
    : m_mutex(mutex)
    , m_registry(registry) {
    m_mutex->lock_shared();
}

babelwires::IdentifierRegistry::ReadAccess::ReadAccess(ReadAccess&& other)
    : m_mutex(other.m_mutex)
    , m_registry(other.m_registry) {
    other.m_mutex = nullptr;
    other.m_registry = nullptr;
}

babelwires::IdentifierRegistry::ReadAccess::~ReadAccess() {
    if (m_mutex) {
        m_mutex->unlock_shared();
    }
}

babelwires::IdentifierRegistry::WriteAccess::WriteAccess(std::shared_mutex* mutex, IdentifierRegistry* registry)
    : m_mutex(mutex)
    , m_registry(registry) {
    m_mutex->lock();
}

babelwires::IdentifierRegistry::WriteAccess::WriteAccess(WriteAccess&& other)
    : m_mutex(other.m_mutex)
    , m_registry(other.m_registry) {
    other.m_mutex = nullptr;
    other.m_registry = nullptr;
}

babelwires::IdentifierRegistry::WriteAccess::~WriteAccess() {
    if (m_mutex) {
        m_mutex->unlock();
    }
}

babelwires::IdentifierRegistry* babelwires::IdentifierRegistry::swapInstance(babelwires::IdentifierRegistry* reg) {
    std::lock_guard guard(s_mutex);
    babelwires::IdentifierRegistry* old = s_singletonInstance;
    s_singletonInstance = reg;
    return old;
}

babelwires::IdentifierRegistry::const_iterator babelwires::IdentifierRegistry::begin() const {
    return const_iterator(m_uuidToInstanceDataMap.begin());
}

babelwires::IdentifierRegistry::const_iterator babelwires::IdentifierRegistry::end() const {
    return const_iterator(m_uuidToInstanceDataMap.end());
}

babelwires::IdentifierRegistry::ReadAccess babelwires::IdentifierRegistry::read() {
    assert(s_singletonInstance && "No IdentifierRegistry instance is currently set as the singleton instance");
    return ReadAccess(&s_mutex, s_singletonInstance);
}

babelwires::IdentifierRegistry::WriteAccess babelwires::IdentifierRegistry::write() {
    assert(s_singletonInstance && "No IdentifierRegistry instance is currently set as the singleton instance");
    return WriteAccess(&s_mutex, s_singletonInstance);
}

void babelwires::IdentifierRegistry::serializeContents(Serializer& serializer) const {
    std::vector<const InstanceData*> contents;
    for (const auto& [_, v] : m_uuidToInstanceDataMap) {
        contents.emplace_back(v.get());
    }
    if (contents.empty()) {
        return;
    }
    // We'd like the table sorted by identifier.
    // The default ordering for identifiers is unaware of disciminators.
    std::sort(contents.begin(), contents.end(), [](const auto* a, const auto* b) {
        const babelwires::Identifier idA = a->m_identifier;
        const babelwires::Identifier idB = b->m_identifier;
        if (idA < idB) {
            return true;
        } else if (idB < idA) {
            return false;
        } else {
            return idA.getDiscriminator() < idB.getDiscriminator();
        }
    });
    serializer.serializeArray("identifiers", contents);
}

void babelwires::IdentifierRegistry::deserializeContents(Deserializer& deserializer) {
    for (auto it = deserializer.deserializeArray<InstanceData>("identifiers", Deserializer::IsOptional::Optional);
         it.isValid(); ++it) {
        std::unique_ptr<InstanceData> instanceDataPtr = it.getObject();
        InstanceData* instanceData = instanceDataPtr.get();

        const Identifier::Discriminator discriminator = instanceDataPtr->m_identifier.getDiscriminator();
        if (discriminator == 0) {
            throw ParseException() << "An identifier in the identifier metadata had no discriminator";
        }

        auto [uit, wasInserted] = m_uuidToInstanceDataMap.insert(
            std::pair<Uuid, std::unique_ptr<InstanceData>>(instanceData->m_uuid, std::move(instanceDataPtr)));
        if (!wasInserted) {
            throw ParseException() << "An identifier with uuid \"" << uit->first << "\" was duplicated";
        }

        Data& data = m_instanceDatasFromIdentifier[instanceData->m_identifier];

        data.m_instanceDatas.resize(discriminator);
        if (data.m_instanceDatas[discriminator - 1]) {
            throw ParseException() << "The identifier registry already has an identifier \""
                                   << instanceData->m_identifier << "\"";
        }
        data.m_instanceDatas[discriminator - 1] = uit->second.get();
    }
}

babelwires::IdentifierRegistryScope::IdentifierRegistryScope()
    : m_previousInstance(babelwires::IdentifierRegistry::swapInstance(&m_currentInstance)) {}

babelwires::IdentifierRegistryScope::~IdentifierRegistryScope() {
    babelwires::IdentifierRegistry::swapInstance(m_previousInstance);
}

babelwires::IdentifierRegistry::const_iterator::ValueType
babelwires::IdentifierRegistry::const_iterator::operator*() const {
    return {m_it->second->m_identifier, &m_it->second->m_fieldName, &m_it->second->m_uuid};
}
