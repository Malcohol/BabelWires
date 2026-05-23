/**
 * Factory for creating serializer and deserializer backends for user-authored documents.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>

#include <memory>

namespace babelwires {

    class Serializer;
    class Deserializer;
    class DeserializationRegistryInterface;
    struct UserLogger;

    /// Factory for serializers and deserializers used for user-owned persistent documents.
    class BASELIB_API UserDocumentSerializationFactory {
      public:
        static std::unique_ptr<Serializer> createSerializer();

        static std::unique_ptr<Deserializer>
        createDeserializer(const DeserializationRegistryInterface& deserializationRegistry, UserLogger& userLogger);
    };

} // namespace babelwires
