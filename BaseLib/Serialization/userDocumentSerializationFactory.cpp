/**
 * Factory for creating serializer and deserializer backends for user-authored documents.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/Serialization/userDocumentSerializationFactory.hpp>

#include <BaseLib/Serialization/XML/xmlDeserializer.hpp>
#include <BaseLib/Serialization/XML/xmlSerializer.hpp>

#include <memory>

std::unique_ptr<babelwires::Serializer> babelwires::UserDocumentSerializationFactory::createSerializer() {
    return std::make_unique<XmlSerializer>();
}

std::unique_ptr<babelwires::Deserializer> babelwires::UserDocumentSerializationFactory::createDeserializer(
    const DeserializationRegistryInterface& deserializationRegistry, UserLogger& userLogger) {
    return std::make_unique<XmlDeserializer>(deserializationRegistry, userLogger);
}
