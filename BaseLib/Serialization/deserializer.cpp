/**
 * The Deserializer supports the loading of serialized data, where the particular representation (e.g. XML) of data is abstracted.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/Serialization/deserializer.hpp>

babelwires::Deserializer::Deserializer(UserLogger& userLogger, const DeserializationRegistry& deserializationRegistry)
    : m_userLogger(userLogger)
    , m_deserializationRegistry(deserializationRegistry) {}

babelwires::Deserializer::~Deserializer() {
    assert(((std::uncaught_exceptions() > 0) || m_wasFinalized) && "The deserializer was not finalized");
}

void babelwires::Deserializer::BaseIterator::operator++() {
    assert(isValid());
    ++(*m_impl);
    if (!m_typeName.empty() && isValid()) {
        if (m_deserializer.getCurrentTypeName() != m_typeName) {
            throw ParseException()
                << "Not expecting an object of type \"" << m_deserializer.getCurrentTypeName() << "\"";
        }
    }
    checkFinished();
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
    checkFinished();
}

void babelwires::Deserializer::BaseIterator::checkFinished() {
    if (!m_finished && m_impl && !isValid()) {
        m_deserializer.popArray();
        m_finished = true;
    }
}

void babelwires::Deserializer::initialize() {
    if (!pushObject("contents")) {
        throw ParseException() << "The element \"contents\" is missing";
    }
    deserializeMetadata(*this, m_userLogger, m_deserializationRegistry);
}

void babelwires::Deserializer::finalize() {
    popObject();
    m_wasFinalized = true;
}

void* babelwires::Deserializer::deserializeCurrentObject(const void* tagOfTypeSought) {
    const std::string_view currentTypeName = getCurrentTypeName();
    if (const DeserializationRegistry::Entry* entry = m_deserializationRegistry.findEntry(currentTypeName)) {
        const void* entryTag = entry->m_baseClassTag;
        // The tags form a path up the type tree, so search for a match for T.
        while (entryTag && (tagOfTypeSought != entryTag)) {
            entryTag = *static_cast<const void* const*>(entryTag);
        }
        if (entryTag) {
            // The registered entry is a subtype of the type we're trying to deserialize.
            return entry->m_factory(*this);
        } else {
            throw ParseException() << "The type \"" << currentTypeName << "\" was not expected here";
        }
    }
    throw ParseException() << "Unknown type \"" << currentTypeName << "\"";
}