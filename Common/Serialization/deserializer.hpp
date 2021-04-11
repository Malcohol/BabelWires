#pragma once

#include "Common/Serialization/serializable.hpp"
#include "Common/Serialization/serializableValue.hpp"
#include "Common/Serialization/serializerDeserializerCommon.hpp"
#include "Common/exceptions.hpp"

#include <cassert>
#include <cstdint>
#include <memory>
#include <string_view>

namespace babelwires {

    class Deserializer : public SerializerDeserializerCommon {
      public:
        Deserializer(UserLogger& userLogger, const DeserializationRegistry& deserializationRegistry);
        virtual ~Deserializer();

        enum class IsOptional {
            /// A ParseException will be thrown if the object or value is not found.
            Required,

            /// No exception is thrown if the object or value is not found.
            Optional
        };

        /// Get a value from the current object.
        /// If optional, these methods return a boolean to indicate success.
        /// Otherwise they throw a ParseException.

        virtual bool deserializeValue(std::string_view key, bool& value,
                                      IsOptional isOptional = IsOptional::Required) = 0;
        virtual bool deserializeValue(std::string_view key, std::string& value,
                                      IsOptional isOptional = IsOptional::Required) = 0;
        virtual bool deserializeValue(std::string_view key, std::uint32_t& value,
                                      IsOptional isOptional = IsOptional::Required) = 0;
        virtual bool deserializeValue(std::string_view key, std::uint16_t& value,
                                      IsOptional isOptional = IsOptional::Required) = 0;
        virtual bool deserializeValue(std::string_view key, std::uint8_t& value,
                                      IsOptional isOptional = IsOptional::Required) = 0;
        virtual bool deserializeValue(std::string_view key, std::int32_t& value,
                                      IsOptional isOptional = IsOptional::Required) = 0;
        virtual bool deserializeValue(std::string_view key, std::int16_t& value,
                                      IsOptional isOptional = IsOptional::Required) = 0;
        virtual bool deserializeValue(std::string_view key, std::int8_t& value,
                                      IsOptional isOptional = IsOptional::Required) = 0;

        /// Objects with methods "serializeToString" and "deserializeToString" can be deserialized as values.
        template <typename V>
        std::enable_if_t<IsSerializableValue<V>::value, bool>
        deserializeValue(std::string_view key, V& value, IsOptional isOptional = IsOptional::Required) {
            std::string asString;
            const bool ret = deserializeValue(key, asString, isOptional);
            if (ret) {
                value = V::deserializeFromString(asString);
            }
            return ret;
        }

        /// Deserialize a child object of type T.
        /// The key is often the name of a field, but by default it duplicates the name of the object's type.
        template <typename T>
        std::unique_ptr<T> deserializeObject(std::string_view key = T::serializationType,
                                             IsOptional isOptional = IsOptional::Required);

        /// A non-standard iterator, which provides access to the deserialized objects of base type T in an array.
        template <typename T> struct Iterator;

        /// Get an iterator to the beginning of the array with the given key.
        template <typename T>
        Iterator<T> deserializeArray(std::string_view key, IsOptional isOptional = IsOptional::Required);

        /// A non-standard iterator, which provides access to deserialized values of type T in an array.
        template <typename T> struct ValueIterator;

        /// Get a ValueIterator to the beginning of the value array with the given key.
        template <typename T>
        ValueIterator<T> deserializeValueArray(std::string_view, IsOptional isOptional = IsOptional::Required,
                                               std::string_view typeName = "element");

        /// Get a description of the current parsing location which can be used in errors and warnings (e.g. a line
        /// number).
        virtual void addContextDescription(ParseException& e) const = 0;

      protected:
        /// Subclass must call this before the Deserializer interface is used.
        void initialize();

        /// Subclass must call this before the contents are considered finalized.
        void finalize();

      protected:
        template <typename T> std::unique_ptr<T> deserializeCurrentObject();

        void* deserializeCurrentObject(const void* tagOfTypeSought);

        struct BaseIterator;
        struct AbstractIterator {
            virtual ~AbstractIterator() = default;
            virtual void operator++() = 0;
            virtual bool isValid() const = 0;
        };

        virtual std::unique_ptr<AbstractIterator> getIteratorImpl() = 0;

        /// Find the child, returns false if the child is missing.
        virtual bool pushObject(std::string_view key) = 0;
        virtual void popObject() = 0;

        virtual bool pushArray(std::string_view key) = 0;
        virtual void popArray() = 0;

        virtual std::string_view getCurrentTypeName() = 0;

      protected:
        bool m_wasFinalized = false;
        UserLogger& m_userLogger;
        const DeserializationRegistry& m_deserializationRegistry;
    };

} // namespace babelwires

template <typename T> std::unique_ptr<T> babelwires::Deserializer::deserializeCurrentObject() {
    return std::unique_ptr<T>(static_cast<T*>(deserializeCurrentObject(T::getSerializationTag())));
}

template <typename T>
std::unique_ptr<T> babelwires::Deserializer::deserializeObject(std::string_view key, IsOptional isOptional) {
    if (!pushObject(key)) {
        if (isOptional == IsOptional::Required) {
            throw ParseException() << "Missing child \"" << key << "\"";
        } else {
            return nullptr;
        }
    }
    auto ret = deserializeCurrentObject<T>();
    popObject();
    return ret;
}

struct babelwires::Deserializer::BaseIterator {
    void operator++();
    bool isValid() const;

    BaseIterator(std::unique_ptr<AbstractIterator> impl, Deserializer& deserializer,
                 std::string typeName = std::string());
    ~BaseIterator();

  protected:
    void checkFinished();

  protected:
    std::unique_ptr<AbstractIterator> m_impl;
    Deserializer& m_deserializer;
    bool m_finished = false;
    /// Only used for value arrays.
    std::string m_typeName;
};

template <typename T> struct babelwires::Deserializer::Iterator : BaseIterator {
    std::unique_ptr<T> getObject() { return m_deserializer.deserializeCurrentObject<T>(); }

    Iterator(std::unique_ptr<AbstractIterator> impl, Deserializer& deserializer)
        : BaseIterator(std::move(impl), deserializer) {}
};

template <typename T>
babelwires::Deserializer::Iterator<T> babelwires::Deserializer::deserializeArray(std::string_view key,
                                                                                 IsOptional isOptional) {
    if (!pushArray(key)) {
        if (isOptional == IsOptional::Required) {
            throw ParseException() << "Missing child \"" << key << "\"";
        } else {
            return Iterator<T>(nullptr, *this);
        }
    }
    return Iterator<T>(getIteratorImpl(), *this);
}

template <typename T> struct babelwires::Deserializer::ValueIterator : BaseIterator {
    T deserializeValue() {
        T value;
        m_deserializer.deserializeValue("value", value);
        return value;
    }

    ValueIterator(std::unique_ptr<AbstractIterator> impl, Deserializer& deserializer, std::string_view typeName)
        : BaseIterator(std::move(impl), deserializer, std::string(typeName)) {}
};

template <typename T>
babelwires::Deserializer::ValueIterator<T> babelwires::Deserializer::deserializeValueArray(std::string_view key,
                                                                                           IsOptional isOptional,
                                                                                           std::string_view typeName) {
    if (!pushArray(key)) {
        if (isOptional == IsOptional::Required) {
            throw ParseException() << "Missing child \"" << key << "\"";
        } else {
            return ValueIterator<T>(nullptr, *this, typeName);
        }
    }
    return ValueIterator<T>(getIteratorImpl(), *this, typeName);
}
