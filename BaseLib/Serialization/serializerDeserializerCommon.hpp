/**
 * Functionality common to serialization and deserialization.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Result/result.hpp>
#include <BaseLib/common.hpp>

#include <map>
#include <string>
#include <string_view>

namespace babelwires {

    struct UserLogger;
    class Serializer;
    class Deserializer;
    class DeserializationRegistryInterface;

    /// Some common functionality shared by the serializer and the deserializer.
    class BASELIB_API SerializerDeserializerCommon {
      public:
        /// Serialized documents always have a single outer container owned by the serialization abstraction.
        inline static constexpr std::string_view c_contentsKey = "contents";

        /// Version metadata is stored as ordinary serialization-system data under the outer container.
        inline static constexpr std::string_view c_serializationMetadataKey = "serializationMetadata";
        inline static constexpr std::string_view c_serializationMetadataTypeKey = "type";
        inline static constexpr std::string_view c_serializationMetadataVersionKey = "version";

        /// Backends may choose their own concrete wire representation for value arrays.
        /// If wrapper-object representations are required, these names can be used.
        inline static constexpr std::string_view c_defaultValueArrayElementTypeName = "element";
        inline static constexpr std::string_view c_defaultValueArrayValueKey = "value";

        /// Reserved runtime type metadata is backend-defined. Map-like backends are expected to use this key.
        inline static constexpr std::string_view c_runtimeTypeMetadataKey = "$type";

        /// Map-like backends such as YAML and JSON reserve the $ prefix for backend metadata.
        inline static constexpr std::string_view c_dollarMetadataPrefix = "$";

        static bool keyHasReservedPrefix(std::string_view key, std::string_view prefix);

        /// 0 is returned if the type is not found.
        VersionNumber getTypeVersion(std::string_view typeName);

      protected:
        /// Record the version of the type being serialized.
        void recordVersion(std::string_view typeName, VersionNumber typeVersion);

        /// Serialize the type versions into the serializer.
        void serializeMetadata(Serializer& serializer);

        /// Deserialize the type versions from the deserializer.
        Result deserializeMetadata(Deserializer& deserializer, UserLogger& userLogger);

      protected:
        /// Record the version for each type name, so it can be added to the file.
        std::map<std::string, VersionNumber, std::less<>> m_serializationVersions;
    };

} // namespace babelwires
