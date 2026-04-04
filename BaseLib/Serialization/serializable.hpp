/**
 * An interface for classes which support serialization/deserialization.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>

#include <BaseLib/Result/resultDSL.hpp>
#include <BaseLib/Serialization/deserializationRegistryInterface.hpp>
#include <BaseLib/Utilities/classUniqueString.hpp>

#include <cassert>
#include <functional>
#include <memory>
#include <string_view>

namespace babelwires {

    class Serializer;
    class Deserializer;

    /// Classes which are serializable must derive from this interface.
    struct BASELIB_API Serializable {
        virtual ~Serializable() = default;

        /// Concrete classes need to implement this, to serialize their contents.
        virtual void serializeContents(Serializer& serializer) const = 0;

        /// Concrete classes need to implement this, to deserialize their contents.
        virtual Result deserializeContents(Deserializer& deserializer) = 0;

        /// This is supplied automatically by both SERIALIZABLE macros.
        virtual std::string_view getSerializationTypeName() const = 0;

        /// This is supplied automatically by the SERIALIZABLE macro.
        /// If there is a s_serializationVersion member, that is used. Otherwise the version is assumed to be 1.
        virtual VersionNumber getSerializationVersion() const = 0;
    };

/// For abstract classes whose subtypes can be serialized.
/// Abstract classes do not need to provide a TYPENAME, so a unique string is generated for them.
#define SERIALIZABLE_ABSTRACT(T, PARENT)                                                                               \
    using SerializableParent = PARENT;                                                                                 \
    inline static const babelwires::DeserializationTreeNode s_deserializationTreeNode{                                 \
        babelwires::getClassUniqueString<T>(), babelwires::Detail::getParentDeserializationTreeNode<T>()};             \
    static constexpr const babelwires::DeserializationTreeNode& getDeserializationTreeNode() {                         \
        return s_deserializationTreeNode;                                                                             \
    }

/// For concrete classes which can be serialized and deserialized.
/// A class must provide implementations of serializeContents and deserializeContents.
// TODO Macro tricks to make PARENT and VERSION optional.
#define SERIALIZABLE(T, TYPENAME, PARENT, VERSION)                                                                     \
    using SerializableParent = PARENT;                                                                                 \
    inline static const babelwires::DeserializationTreeNode s_deserializationTreeNode{                                 \
        TYPENAME, babelwires::Detail::getParentDeserializationTreeNode<T>()};                                          \
    static constexpr const babelwires::DeserializationTreeNode& getDeserializationTreeNode() {                         \
        return s_deserializationTreeNode;                                                                             \
    }                                                                                                                  \
    std::string_view getSerializationTypeName() const override {                                                       \
        return s_serializationTypeName;                                                                                \
    }                                                                                                                  \
    static constexpr char s_serializationTypeName[] = TYPENAME;                                                        \
    static constexpr int s_serializationVersion = VERSION;                                                             \
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
        deserializingFactory, s_serializationVersion, &s_deserializationTreeNode};

    // Implementation details
    namespace Detail {
        template <typename T> class IsSerializable {
          private:
            using Yes = char[1];
            using No = char[2];

            template <typename U> static Yes& test(typename U::SerializableParent*);
            template <typename U> static No& test(...);

          public:
            enum { value = sizeof(test<T>(0)) == sizeof(Yes) };
        };

        template <typename T> constexpr const DeserializationTreeNode* getParentDeserializationTreeNode() {
            if constexpr (IsSerializable<T>::value && IsSerializable<typename T::SerializableParent>::value) {
                return &T::SerializableParent::s_deserializationTreeNode;
            } else {
                return nullptr;
            }
        }

    } // namespace Detail

} // namespace babelwires