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

babelwires::Deserializer::Deserializer(UserLogger& userLogger, const DeserializationRegistryInterface& deserializationRegistry)
    : m_userLogger(userLogger)
    , m_deserializationRegistry(&deserializationRegistry) {}

babelwires::Deserializer::~Deserializer() {
    assert(m_wasFinalized && "The deserializer was not finalized");
}

babelwires::Result babelwires::Deserializer::initialize() {
    if (!pushObject(c_contentsKey)) {
        return Error() << "The element \"" << c_contentsKey << "\" is missing";
    }
    DO_OR_ERROR(deserializeMetadata(*this, m_userLogger));
    return {};
}

const babelwires::DeserializationRegistryInterface& babelwires::Deserializer::getDeserializationRegistry() const {
    assert(m_deserializationRegistry && "The deserialization registry pointer was null");
    return *m_deserializationRegistry;
}

void babelwires::Deserializer::setDeserializationRegistry(const DeserializationRegistryInterface& deserializationRegistry) {
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
babelwires::Deserializer::deserializeCurrentObject(const DeserializationTreeNode& treeNodeOfExpectedType) {
    const std::string_view currentTypeName = getCurrentTypeName();
    if (const DeserializationRegistryInterface::Entry* entry = getDeserializationRegistry().findEntry(currentTypeName)) {
        const DeserializationTreeNode* treeNode = entry->m_node;
        // Look for the expected type in the inheritance hierarchy of the type we deserialized.
        while (treeNode && (treeNodeOfExpectedType.m_serializationType != treeNode->m_serializationType)) {
            treeNode = treeNode->m_parentNode;
        }
        if (treeNode) {
            // The registered entry is a subtype of the type we're trying to deserialize.
            return entry->m_factory(*this);
        } else {
            return Error() << "The type \"" << currentTypeName << "\" was not expected here";
        }
    }
    return Error() << "Unknown type \"" << currentTypeName << "\"";
}

babelwires::ResultT<std::unique_ptr<babelwires::Serializable>> babelwires::Deserializer::deserializeCurrentObjectOfExactType(
    const DeserializationTreeNode& expectedTreeNode, std::string_view expectedTypeName) {
    const std::string_view currentTypeName = getCurrentTypeName();
    if (const DeserializationRegistryInterface::Entry* entry = getDeserializationRegistry().findEntry(currentTypeName)) {
        if (entry->m_node == &expectedTreeNode) {
            return entry->m_factory(*this);
        }
        return Error() << "The type \"" << currentTypeName << "\" cannot be deserialized by value as \""
                       << expectedTypeName << "\"";
    }
    return Error() << "Unknown type \"" << currentTypeName << "\"";
}

babelwires::Result babelwires::Deserializer::BaseIterator::advance() {
    assert(isValid());
    DO_OR_ERROR(m_impl->advance());
    DO_OR_ERROR(checkCurrentValueArrayElementType());
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
    checkCurrentValueArrayElementType();
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

babelwires::Result babelwires::Deserializer::BaseIterator::checkCurrentValueArrayElementType() {
    if (!m_typeName.empty() && isValid()) {
        if (!m_deserializer.currentValueArrayElementMatchesType(m_typeName)) {
            return Error() << "Not expecting an object of type \"" << m_deserializer.getCurrentTypeName() << "\"";
        }
    }
    return {};
}

bool babelwires::Deserializer::currentValueArrayElementMatchesType(std::string_view expectedTypeName) {
    return getCurrentTypeName() == expectedTypeName;
}
