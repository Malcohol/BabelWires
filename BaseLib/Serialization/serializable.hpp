/**
 * An interface for classes which support serialization/deserialization.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Result/resultDSL.hpp>
#include <BaseLib/Serialization/deserializationRegistryInterface.hpp>

#include <cassert>
#include <functional>
#include <memory>
#include <string_view>

namespace babelwires {

    class Serializer;
    class Deserializer;

    /// Classes which are serializable must derive from this interface.
    struct Serializable {
        virtual ~Serializable() = default;

        /// Concrete classes need to implement this, to serialize their contents.
        virtual void serializeContents(Serializer& serializer) const = 0;

        /// Concrete classes need to implement this, to deserialize their contents.
        virtual Result deserializeContents(Deserializer& deserializer) = 0;

        /// This is supplied automatically by both SERIALIZABLE macros.
        virtual std::string_view getSerializationType() const = 0;

        /// This is supplied automatically by the SERIALIZABLE macro.
        /// If there is a serializationVersion member, that is used. Otherwise the version is assumed to be 1.
        virtual VersionNumber getSerializationVersion() const = 0;
    };

/// For abstract classes whose subtypes can be serialized.
#define SERIALIZABLE_ABSTRACT(T, PARENT)                                                                               \
    using SerializableParent = PARENT;                                                                                 \
    static constexpr void* getSerializationTag() {                                                                     \
        return babelwires::Detail::getSerializationTag<T>();                                                           \
    }

/// For concrete classes which can be serialized and deserialized.
/// A class must provide implementations of serializeContents and deserializeContents.
// TODO Macro tricks to make PARENT and VERSION optional.
#define SERIALIZABLE(T, TYPENAME, PARENT, VERSION)                                                                     \
    SERIALIZABLE_ABSTRACT(T, PARENT);                                                                                  \
    std::string_view getSerializationType() const override {                                                           \
        return serializationType;                                                                                      \
    }                                                                                                                  \
    static constexpr char serializationType[] = TYPENAME;                                                              \
    static constexpr int serializationVersion = VERSION;                                                               \
    static_assert(VERSION != 0, "Version must be greater than 0");                                                     \
    babelwires::VersionNumber getSerializationVersion() const override {                                               \
        return VERSION;                                                                                                \
    }                                                                                                                  \
    static const babelwires::DeserializationRegistryInterface::Entry* getDeserializationRegistryEntry() {              \
        return &s_registryEntry;                                                                                       \
    }                                                                                                                  \
    static babelwires::ResultT<std::unique_ptr<babelwires::Serializable>> deserializingFactory(                        \
        babelwires::Deserializer& deserializer) {                                                                      \
        /* make_unique requires a public default constructor. */                                                       \
        std::unique_ptr<T> newObject(new T());                                                                         \
        auto result = newObject->deserializeContents(deserializer);                                                    \
        if (!result) {                                                                                                 \
            /* TODO Add context, e.g. ", when deserializing <TYPENAME>?" */                                            \
            return result.error();                                                                                     \
        }                                                                                                              \
        return std::move(newObject);                                                                                   \
    }                                                                                                                  \
    inline static const babelwires::DeserializationRegistryInterface::Entry s_registryEntry{                           \
        deserializingFactory, serializationType, serializationVersion, babelwires::Detail::getSerializationTag<T>()};

    // Implementation details
    namespace Detail {
        template <typename T> struct SerializableType {
            /// The _address_ of this pointer uniquely defines a type.
            /// The value is the address of T's parent's tag, or void if the parent doesn't have one.
            /// This enables the system to validate subtype relationships during deserialization.
            static const void* s_serializationTag;
        };

        template <typename T> class IsSerializable {
          private:
            using Yes = char[1];
            using No = char[2];

            template <typename U> static Yes& test(typename U::SerializableParent*);
            template <typename U> static No& test(...);

          public:
            enum { value = sizeof(test<T>(0)) == sizeof(Yes) };
        };

        template <typename T> constexpr void* getSerializationTag() {
            if constexpr (IsSerializable<T>::value) {
                return &SerializableType<T>::s_serializationTag;
            } else {
                return nullptr;
            }
        }

        template <typename T> constexpr void* getParentsSerializationTag() {
            if constexpr (IsSerializable<T>::value && IsSerializable<typename T::SerializableParent>::value) {
                return T::SerializableParent::getSerializationTag();
            } else {
                return nullptr;
            }
        }

        template <typename T> const void* SerializableType<T>::s_serializationTag = getParentsSerializationTag<T>();

    } // namespace Detail

} // namespace babelwires