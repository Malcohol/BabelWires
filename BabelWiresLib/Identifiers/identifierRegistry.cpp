/**
 * A singleton which ensures that fieldIdentifiers are unique in the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Identifiers/identifierRegistry.hpp"

#include "Common/Log/debugLogger.hpp"
#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"
#include "Common/exceptions.hpp"

babelwires::FieldNameRegistry::FieldNameRegistry() = default;
babelwires::FieldNameRegistry::FieldNameRegistry(FieldNameRegistry&&) = default;
babelwires::FieldNameRegistry::~FieldNameRegistry() = default;
babelwires::FieldNameRegistry& babelwires::FieldNameRegistry::operator=(FieldNameRegistry&&) = default;

babelwires::FieldNameRegistry::InstanceData::InstanceData()
    : m_uuid("00000000-0000-0000-0000-000000000000")
    , m_identifier("Invald")
    , m_authority(Authority::isProvisional) {}

babelwires::FieldNameRegistry::InstanceData::InstanceData(std::string fieldName, Uuid uuid, Identifier identifier,
                                                          Authority authority)
    : m_fieldName(std::move(fieldName))
    , m_uuid(std::move(uuid))
    , m_identifier(identifier)
    , m_authority(authority) {}

void babelwires::FieldNameRegistry::InstanceData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("id", m_identifier);
    serializer.serializeValue("name", m_fieldName);
    serializer.serializeValue("uuid", m_uuid);
}

void babelwires::FieldNameRegistry::InstanceData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("id", m_identifier);
    deserializer.deserializeValue("name", m_fieldName);
    deserializer.deserializeValue("uuid", m_uuid);
    m_authority = Authority::isProvisional;
}

babelwires::Identifier babelwires::FieldNameRegistry::addFieldName(babelwires::Identifier identifier,
                                                                        const std::string& name, const Uuid& uuid,
                                                                        Authority authority) {
    const Identifier::Discriminator discriminator = identifier.getDiscriminator();
    assert((discriminator == 0) && "The field already has a discriminator: Did you already register it?");

    const auto it = m_uuidToInstanceDataMap.find(uuid);
    if (it == m_uuidToInstanceDataMap.end()) {
        if (authority != Authority::isTemporary) {
            logDebug() << ((authority == Authority::isAuthoritative) ? "Authoritatively" : "Provisionally")
                       << " registering Identifier " << identifier << " as \"" << name << "\"";
        }

        auto [uit, _] = m_uuidToInstanceDataMap.insert(
            std::pair(uuid, std::make_unique<InstanceData>(name, uuid, identifier, authority)));

        Data& data = m_fieldData[identifier];

        const int newDiscriminator = data.m_fieldNames.size() + 1;
        // I could fail safe here, but it seems very unlikely to happen. If it does, then the system needs a rethink.
        assert((newDiscriminator <= Identifier::c_maxDiscriminator) && "Too many duplicate field identifiers");
        data.m_fieldNames.emplace_back(uit->second.get());
        identifier.setDiscriminator(newDiscriminator);
        uit->second->m_identifier = identifier;
    } else {
        InstanceData& instanceData = *it->second;
        if (authority != Authority::isAuthoritative) {
            // Since we're loading from a file or saving into a temporary registry, just take whatever is in memory.
            identifier = instanceData.m_identifier;
        } else {
            // We may be registering from code at some point after the field has been registered from a file.
            // In this case, update the stored data.
            assert((((instanceData.m_authority == Authority::isProvisional) ||
                     ((instanceData.m_identifier == identifier) && instanceData.m_fieldName == name))) &&
                   "Uuid is registered twice from code with inconsistent field data");
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

const babelwires::FieldNameRegistry::InstanceData*
babelwires::FieldNameRegistry::getInstanceData(Identifier identifier) const {
    const babelwires::Identifier::Discriminator index = identifier.getDiscriminator();
    if (index > 0) {
        identifier.setDiscriminator(0);
        const auto& it = m_fieldData.find(identifier);
        if (it != m_fieldData.end()) {
            const Data& data = it->second;
            if (index <= data.m_fieldNames.size()) {
                return data.m_fieldNames[index - 1];
            }
        }
    }
    return nullptr;
}

babelwires::FieldNameRegistry::ValueType
babelwires::FieldNameRegistry::getDeserializedFieldData(Identifier identifier) const {
    if (const babelwires::FieldNameRegistry::InstanceData* data = getInstanceData(identifier)) {
        return ValueType{identifier, &data->m_fieldName, &data->m_uuid};
    }
    throw ParseException() << "Identifier \"" << identifier
                           << "\" not found in the field metadata. Note that unregistered fields (those with no "
                              "discriminator) are allowed";
}

std::string babelwires::FieldNameRegistry::getFieldName(babelwires::Identifier identifier) const {
    if (const InstanceData* data = getInstanceData(identifier)) {
        return data->m_fieldName;
    }
    // TODO
    return identifier.serializeToString();
}

std::shared_mutex babelwires::FieldNameRegistry::s_mutex;
babelwires::FieldNameRegistry* babelwires::FieldNameRegistry::s_singletonInstance;

babelwires::FieldNameRegistry::ReadAccess::ReadAccess(std::shared_mutex* mutex, const FieldNameRegistry* registry)
    : m_mutex(mutex)
    , m_registry(registry) {
    m_mutex->lock_shared();
}

babelwires::FieldNameRegistry::ReadAccess::ReadAccess(ReadAccess&& other)
    : m_mutex(other.m_mutex)
    , m_registry(other.m_registry) {
    other.m_mutex = nullptr;
    other.m_registry = nullptr;
}

babelwires::FieldNameRegistry::ReadAccess::~ReadAccess() {
    if (m_mutex) {
        m_mutex->unlock_shared();
    }
}

babelwires::FieldNameRegistry::WriteAccess::WriteAccess(std::shared_mutex* mutex, FieldNameRegistry* registry)
    : m_mutex(mutex)
    , m_registry(registry) {
    m_mutex->lock();
}

babelwires::FieldNameRegistry::WriteAccess::WriteAccess(WriteAccess&& other)
    : m_mutex(other.m_mutex)
    , m_registry(other.m_registry) {
    other.m_mutex = nullptr;
    other.m_registry = nullptr;
}

babelwires::FieldNameRegistry::WriteAccess::~WriteAccess() {
    if (m_mutex) {
        m_mutex->unlock();
    }
}

babelwires::FieldNameRegistry* babelwires::FieldNameRegistry::swapInstance(babelwires::FieldNameRegistry* reg) {
    std::lock_guard guard(s_mutex);
    babelwires::FieldNameRegistry* old = s_singletonInstance;
    s_singletonInstance = reg;
    return old;
}

babelwires::FieldNameRegistry::const_iterator babelwires::FieldNameRegistry::begin() const {
    return const_iterator(m_uuidToInstanceDataMap.begin());
}

babelwires::FieldNameRegistry::const_iterator babelwires::FieldNameRegistry::end() const {
    return const_iterator(m_uuidToInstanceDataMap.end());
}

babelwires::FieldNameRegistry::ReadAccess babelwires::FieldNameRegistry::read() {
    assert(s_singletonInstance && "No FieldNameRegistry instance is currently set as the singleton instance");
    return ReadAccess(&s_mutex, s_singletonInstance);
}

babelwires::FieldNameRegistry::WriteAccess babelwires::FieldNameRegistry::write() {
    assert(s_singletonInstance && "No FieldNameRegistry instance is currently set as the singleton instance");
    return WriteAccess(&s_mutex, s_singletonInstance);
}

void babelwires::FieldNameRegistry::serializeContents(Serializer& serializer) const {
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
    serializer.serializeArray("fields", contents);
}

void babelwires::FieldNameRegistry::deserializeContents(Deserializer& deserializer) {
    for (auto it = deserializer.deserializeArray<InstanceData>("fields", Deserializer::IsOptional::Optional);
         it.isValid(); ++it) {
        std::unique_ptr<InstanceData> instanceDataPtr = it.getObject();
        InstanceData* instanceData = instanceDataPtr.get();

        const Identifier::Discriminator discriminator = instanceDataPtr->m_identifier.getDiscriminator();
        if (discriminator == 0) {
            throw ParseException() << "A field in the field metadata had no discriminator";
        }

        auto [uit, wasInserted] = m_uuidToInstanceDataMap.insert(
            std::pair<Uuid, std::unique_ptr<InstanceData>>(instanceData->m_uuid, std::move(instanceDataPtr)));
        if (!wasInserted) {
            throw ParseException() << "A field with the uuid \"" << uit->first << "\" was duplicated";
        }

        Data& data = m_fieldData[instanceData->m_identifier];

        data.m_fieldNames.resize(discriminator);
        if (data.m_fieldNames[discriminator - 1]) {
            throw ParseException() << "The field name registry already has an identifier \""
                                   << instanceData->m_identifier << "\"";
        }
        data.m_fieldNames[discriminator - 1] = uit->second.get();
    }
}

babelwires::FieldNameRegistryScope::FieldNameRegistryScope()
    : m_previousInstance(babelwires::FieldNameRegistry::swapInstance(&m_currentInstance)) {}

babelwires::FieldNameRegistryScope::~FieldNameRegistryScope() {
    babelwires::FieldNameRegistry::swapInstance(m_previousInstance);
}

babelwires::FieldNameRegistry::const_iterator::ValueType
babelwires::FieldNameRegistry::const_iterator::operator*() const {
    return {m_it->second->m_identifier, &m_it->second->m_fieldName, &m_it->second->m_uuid};
}
