/**
 * The Deserializer supports the loading of serialized data, where the particular representation (e.g. XML) of data is
 * abstracted.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/Serialization/deserializer.hpp>

#include <BaseLib/Result/resultDSL.hpp>

babelwires::Deserializer::Deserializer(UserLogger& userLogger, const DeserializationRegistry& deserializationRegistry)
    : m_userLogger(userLogger)
    , m_deserializationRegistry(&deserializationRegistry) {}

babelwires::Deserializer::~Deserializer() {
    assert(m_wasFinalized && "The deserializer was not finalized");
}

babelwires::Result babelwires::Deserializer::initialize() {
    if (!pushObject("contents")) {
        return Error() << "The element \"contents\" is missing";
    }
    DO_OR_ERROR(deserializeMetadata(*this, m_userLogger));
    return {};
}

const babelwires::DeserializationRegistry& babelwires::Deserializer::getDeserializationRegistry() const {
    assert(m_deserializationRegistry && "The deserialization registry pointer was null");
    return *m_deserializationRegistry;
}

void babelwires::Deserializer::setDeserializationRegistry(const DeserializationRegistry& deserializationRegistry) {
    m_deserializationRegistry = &deserializationRegistry;
}

babelwires::Result babelwires::Deserializer::finalize() {
    m_wasFinalized = true;
    return popObject();
}

void babelwires::Deserializer::finalizeOnError() {
    m_wasFinalized = true;
}

babelwires::ResultT<std::unique_ptr<babelwires::Serializable>>
babelwires::Deserializer::deserializeCurrentObject(const void* tagOfTypeSought) {
    const std::string_view currentTypeName = getCurrentTypeName();
    if (const DeserializationRegistry::Entry* entry = getDeserializationRegistry().findEntry(currentTypeName)) {
        const void* entryTag = entry->m_baseClassTag;
        // The tags form a path up the type tree, so search for a match for T.
        while (entryTag && (tagOfTypeSought != entryTag)) {
            entryTag = *static_cast<const void* const*>(entryTag);
        }
        if (entryTag) {
            // The registered entry is a subtype of the type we're trying to deserialize.
            return entry->m_factory(*this);
        } else {
            return Error() << "The type \"" << currentTypeName << "\" was not expected here";
        }
    }
    return Error() << "Unknown type \"" << currentTypeName << "\"";
}

babelwires::DeserializableClassScope::DeserializableClassScope(Deserializer& deserializer)
    : m_deserializer(deserializer)
    , m_previousRegistry(&deserializer.getDeserializationRegistry())
    , m_overlayRegistry(*m_previousRegistry) {
    m_deserializer.setDeserializationRegistry(m_overlayRegistry);
}

babelwires::DeserializableClassScope::~DeserializableClassScope() {
    assert(m_previousRegistry && "The previous registry should always be set");
    m_deserializer.setDeserializationRegistry(*m_previousRegistry);
}

babelwires::Result babelwires::Deserializer::BaseIterator::advance() {
    assert(isValid());
    DO_OR_ERROR(m_impl->advance());
    if (!m_typeName.empty() && isValid()) {
        if (m_deserializer.getCurrentTypeName() != m_typeName) {
            return Error() << "Not expecting an object of type \"" << m_deserializer.getCurrentTypeName() << "\"";
        }
    }
    DO_OR_ERROR(checkFinished());
    return {};
}

bool babelwires::Deserializer::BaseIterator::isValid() const {
    return m_impl && m_impl->isValid();
};

babelwires::Deserializer::BaseIterator::BaseIterator(std::unique_ptr<AbstractIterator> impl, Deserializer& deserializer,
                                                     std::string typeName)
    : m_impl(std::move(impl))
    , m_deserializer(deserializer)
    , m_typeName(std::move(typeName)) {
    checkFinished();
}

babelwires::Deserializer::BaseIterator::~BaseIterator() {
    // We cannot know if the client code has already encountered an error and is just unwinding the stack.
    // The best we can do is to check if the iterator was already finished, and if not, try to finish it.
    checkFinished();
}

babelwires::Result babelwires::Deserializer::BaseIterator::checkFinished() {
    if (!m_finished && m_impl && !isValid()) {
        DO_OR_ERROR(m_deserializer.popArray());
        m_finished = true;
    }
    return {};
}
