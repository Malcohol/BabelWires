/**
 * The Serializer supports the saving of serialized data, where the particular representation (e.g. XML) of data is abstracted.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "Common/Serialization/serializableValue.hpp"
#include "Common/Serialization/serializerDeserializerCommon.hpp"

#include <cstdint>
#include <map>
#include <memory>
#include <string_view>

namespace babelwires {

    template <typename T> class SerializableConcrete;

    /// An interface for serializers.
    struct Serializer : public SerializerDeserializerCommon {
        virtual ~Serializer();

        /// Serialize the object. The key is often the name of a field, but by
        /// default it duplicates the name of the object's type.
        template <typename T> void serializeObject(const T& object, std::string_view key = T::serializationType);

        /// Serialize an array of objects, whose contents described by the given span
        /// (i.e. object with begin and end methods).
        template <typename S> void serializeArray(std::string_view key, const S& span);

        /// Serialize an array of values, whose contents described by the given span
        /// (i.e. object with begin and end methods).
        template <typename S>
        void serializeValueArray(std::string_view key, const S& span, std::string_view typeName = "element");

        // Serialize simple values.
        virtual void serializeValue(std::string_view key, bool value) = 0;
        virtual void serializeValue(std::string_view key, std::string_view value) = 0;
        virtual void serializeValue(std::string_view key, std::uint32_t value) = 0;
        virtual void serializeValue(std::string_view key, std::uint16_t value) = 0;
        virtual void serializeValue(std::string_view key, std::uint8_t value) = 0;
        virtual void serializeValue(std::string_view key, std::int32_t value) = 0;
        virtual void serializeValue(std::string_view key, std::int16_t value) = 0;
        virtual void serializeValue(std::string_view key, std::int8_t value) = 0;

        /// Objects with methods "serializeToString" and "deserializeToString" can be serialized as values.
        template <typename V>
        std::enable_if_t<IsSerializableValue<V>::value> serializeValue(std::string_view key, const V& value) {
            serializeValue(key, value.serializeToString());
        }

      protected:
        /// Subclass must call this before the Serializer interface is used.
        void initialize();

        /// Subclass must call this before the contents are considered finalized.
        void finalize();

      protected:
        void pushObject(std::string_view typeName);
        void pushObjectWithKey(std::string_view typeName, std::string_view key);
        void popObject();
        void pushArray(std::string_view key);
        void popArray();

        void pushCommon();
        void popCommon();

        void setValueCommon(std::string_view key, std::string_view value);

      protected:
        virtual void doPushObject(std::string_view typeName) = 0;
        virtual void doPushObjectWithKey(std::string_view typeName, std::string_view key) = 0;
        virtual void doPopObject() = 0;
        virtual void doPushArray(std::string_view key) = 0;
        virtual void doPopArray() = 0;

      protected:
        /// The depth of the stack of objects.
        int m_depth = 0;

        /// True if no contents have been serialized yet.
        bool m_isEmpty = true;

        bool m_wasFinalized = false;
    };

} // namespace babelwires

template <typename T> void babelwires::Serializer::serializeObject(const T& object, std::string_view key) {
    recordVersion(object.getSerializationType(), object.getSerializationVersion());
    pushObjectWithKey(object.getSerializationType(), key);
    object.serializeContents(*this);
    popObject();
}

// TODO Make more general.
template <typename T> const T& asReference(const T& t) {
    return t;
}
template <typename T> const T& asReference(const T* t) {
    return *t;
}
template <typename T> const T& asReference(const std::unique_ptr<T>& t) {
    return *t;
}
template <typename T> const T& asReference(const std::shared_ptr<T>& t) {
    return *t;
}

template <typename S> void babelwires::Serializer::serializeArray(std::string_view key, const S& span) {
    if (span.begin() != span.end()) {
        pushArray(key);
        for (const auto& it : span) {
            auto& obj = asReference(it);
            recordVersion(obj.getSerializationType(), obj.getSerializationVersion());
            pushObject(obj.getSerializationType());
            obj.serializeContents(*this);
            popObject();
        }
        popArray();
    }
}

template <typename S>
void babelwires::Serializer::serializeValueArray(std::string_view key, const S& span, std::string_view typeName) {
    if (span.begin() != span.end()) {
        pushArray(key);
        for (const auto& it : span) {
            pushObject(typeName);
            serializeValue("value", it);
            popObject();
        }
        popArray();
    }
}
