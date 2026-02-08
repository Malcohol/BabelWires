/**
 * Functionality common to serialization and deserialization.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Utilities/result.hpp>
#include <BaseLib/common.hpp>

#include <map>
#include <string>
#include <string_view>

namespace babelwires {

    struct UserLogger;
    class Serializer;
    class Deserializer;
    class DeserializationRegistry;

    /// Some common functionality shared by the serializer and the deserializer.
    class SerializerDeserializerCommon {
      public:
        /// 0 is returned if the type is not found.
        VersionNumber getTypeVersion(std::string_view typeName);

      protected:
        /// Record the version of the type being serialized.
        void recordVersion(std::string_view typeName, VersionNumber typeVersion);

        /// Serialize the type versions into the serializer.
        void serializeMetadata(Serializer& serializer);

        /// Deserialize the type versions from the deserializer.
        Result deserializeMetadata(Deserializer& deserializer, UserLogger& userLogger,
                                   const DeserializationRegistry& deserializationRegistry);

      protected:
        /// Record the version for each type name, so it can be added to the file.
        std::map<std::string, VersionNumber, std::less<>> m_serializationVersions;
    };

} // namespace babelwires
