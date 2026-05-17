/**
 * The Serializer supports the saving of serialized data, where the particular representation (e.g. XML) of data is abstracted.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>

#include <BaseLib/Serialization/serializableValue.hpp>
#include <BaseLib/Serialization/serializerDeserializerCommon.hpp>

#include <cstdint>
#include <map>
#include <memory>
#include <ostream>
#include <string_view>

namespace babelwires {

    /// An interface for serializers.
        class BASELIB_API Serializer : public SerializerDeserializerCommon {
      public:
        virtual ~Serializer();

        /// Write the serialized document to the given stream.
        virtual void write(std::ostream& os) = 0;

        /// Serialize a child object.
        /// The key identifies the field in the containing object.
        /// By default it duplicates the serialized type name, but runtime type identity remains a separate concept.
        template <typename T> void serializeObject(const T& object, std::string_view key = T::s_serializationTypeName);

        /// Serialize an array of objects, whose contents described by the given span
        /// (i.e. object with begin and end methods).
        template <typename S> void serializeArray(std::string_view key, const S& span);

        /// Serialize an array of values, whose contents described by the given span
        /// (i.e. object with begin and end methods).
        /// Concrete backends may later choose a different wire representation while preserving the same semantics.
        template <typename S>
        void serializeValueArray(std::string_view key, const S& span,
                     std::string_view typeName = c_defaultValueArrayElementTypeName);

        // Serialize simple values.
        void serializeValue(std::string_view key, bool value);
        void serializeValue(std::string_view key, std::string_view value);
        void serializeValue(std::string_view key, std::uint64_t value);
        void serializeValue(std::string_view key, std::uint32_t value);
        void serializeValue(std::string_view key, std::uint16_t value);
        void serializeValue(std::string_view key, std::uint8_t value);
        void serializeValue(std::string_view key, std::int64_t value);
        void serializeValue(std::string_view key, std::int32_t value);
        void serializeValue(std::string_view key, std::int16_t value);
        void serializeValue(std::string_view key, std::int8_t value);

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
        void pushValueArrayElement(std::string_view typeName);
        void pushObjectWithKey(std::string_view typeName, std::string_view key);
        void popObject();
        void pushArray(std::string_view key);
        void popArray();

        void pushCommon();
        void popCommon();

        void assertOrdinaryFieldKey(std::string_view key) const;

      protected:
        virtual void doPushObject(std::string_view typeName) = 0;
        virtual void doPushValueArrayElement(std::string_view typeName);
        virtual void doPushObjectWithKey(std::string_view typeName, std::string_view key) = 0;
        virtual void doSerializeValue(std::string_view key, bool value) = 0;
        virtual void doSerializeValue(std::string_view key, std::string_view value) = 0;
        virtual void doSerializeValue(std::string_view key, std::uint64_t value) = 0;
        virtual void doSerializeValue(std::string_view key, std::uint32_t value) = 0;
        virtual void doSerializeValue(std::string_view key, std::uint16_t value) = 0;
        virtual void doSerializeValue(std::string_view key, std::uint8_t value) = 0;
        virtual void doSerializeValue(std::string_view key, std::int64_t value) = 0;
        virtual void doSerializeValue(std::string_view key, std::int32_t value) = 0;
        virtual void doSerializeValue(std::string_view key, std::int16_t value) = 0;
        virtual void doSerializeValue(std::string_view key, std::int8_t value) = 0;
        virtual bool isOrdinaryFieldKeyReserved(std::string_view key) const = 0;
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
    recordVersion(object.getSerializationTypeName(), object.getSerializationVersion());
    pushObjectWithKey(object.getSerializationTypeName(), key);
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
            recordVersion(obj.getSerializationTypeName(), obj.getSerializationVersion());
            pushObject(obj.getSerializationTypeName());
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
            pushValueArrayElement(typeName);
            serializeValue(c_defaultValueArrayValueKey, it);
            popObject();
        }
        popArray();
    }
}
