/**
 * The Deserializer supports the loading of serialized data, where the particular representation (e.g. XML) of data is
 * abstracted.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Serialization/serializable.hpp>
#include <BaseLib/Serialization/serializableValue.hpp>
#include <BaseLib/Serialization/serializerDeserializerCommon.hpp>
#include <BaseLib/Utilities/result.hpp>
#include <BaseLib/exceptions.hpp>

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
        /// These methods return a ResultT<bool> where:
        /// - Success with true means key was found and parsed successfully
        /// - Success with false means key was missing (no error)
        /// - Error means there was a parsing problem

        virtual ResultT<bool> tryDeserializeValue(std::string_view key, bool& value) = 0;
        virtual ResultT<bool> tryDeserializeValue(std::string_view key, std::string& value) = 0;
        virtual ResultT<bool> tryDeserializeValue(std::string_view key, std::uint64_t& value) = 0;
        virtual ResultT<bool> tryDeserializeValue(std::string_view key, std::uint32_t& value) = 0;
        virtual ResultT<bool> tryDeserializeValue(std::string_view key, std::uint16_t& value) = 0;
        virtual ResultT<bool> tryDeserializeValue(std::string_view key, std::uint8_t& value) = 0;
        virtual ResultT<bool> tryDeserializeValue(std::string_view key, std::int64_t& value) = 0;
        virtual ResultT<bool> tryDeserializeValue(std::string_view key, std::int32_t& value) = 0;
        virtual ResultT<bool> tryDeserializeValue(std::string_view key, std::int16_t& value) = 0;
        virtual ResultT<bool> tryDeserializeValue(std::string_view key, std::int8_t& value) = 0;

        /// Objects with methods "serializeToString" and "deserializeFromString" can be deserialized as values.
        template <typename V>
        std::enable_if_t<
            IsSerializableValue<V>::value &&
                std::is_same_v<decltype(V::deserializeFromString(std::declval<std::string>())), ResultT<V>>,
            ResultT<bool>>
        tryDeserializeValue(std::string_view key, V& value);

        /// Deserialize a value of type T with the given key.
        /// The result will hold an error if the key is not found or there is a parse error.
        template <typename T> Result deserializeValue(std::string_view key, T& value);

        /// Deserialize a child object of type T.
        /// The key is often the name of a field, but by default it duplicates the name of the object's type.
        template <typename T>
        std::unique_ptr<T> deserializeObject(std::string_view key = T::serializationType,
                                             IsOptional isOptional = IsOptional::Required);

        /// A non-standard iterator, which provides access to the deserialized objects of base type T in an array.
        template <typename T> struct Iterator;

        /// If the array is present in the data, get an iterator to the beginning of the array with the given key.
        /// Otherwise return a result with an iterator that is not valid.
        /// It's assumed that the caller does not need to distinguish between a missing array and an empty array
        /// (even if an empty array is not valid for the type).
        template <typename T> ResultT<Iterator<T>> tryDeserializeArray(std::string_view key);

        /// Get an iterator to the beginning of the array with the given key.
        /// Returns a Result containing the iterator on success, or an error if the array is not found.
        /// After checking the result, calling code can use for(auto& it = *result; it.isValid(); ++it) { ... }
        template <typename T> ResultT<Iterator<T>> deserializeArray(std::string_view key);

        /// A non-standard iterator, which provides access to deserialized values of type T in an array.
        template <typename T> struct ValueIterator;

        /// If the value array is present in the data, get an iterator to the beginning of the value array with the
        /// given key. Otherwise return a result with an iterator that is not valid. It's assumed that the caller does
        /// not need to distinguish between a missing array and an empty array
        /// (even if an empty array is not valid for the type).
        template <typename T>
        ResultT<ValueIterator<T>> tryDeserializeValueArray(std::string_view, std::string_view typeName = "element");

        /// Get a ValueIterator to the beginning of the value array with the given key.
        /// Returns a Result containing the iterator on success, or an error if the array is not found.
        /// After checking the result, calling code can use for(auto& it = *result; it.isValid(); ++it) { ... }
        template <typename T>
        ResultT<ValueIterator<T>> deserializeValueArray(std::string_view, std::string_view typeName = "element");

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
    std::unique_ptr<T> getObject();

    Iterator(std::unique_ptr<AbstractIterator> impl, Deserializer& deserializer);
};



template <typename T> struct babelwires::Deserializer::ValueIterator : BaseIterator {
    /// Pass in a tempValue if T cannot be default constructed.
    T deserializeValue(T tempValue = T());

    ValueIterator(std::unique_ptr<AbstractIterator> impl, Deserializer& deserializer, std::string_view typeName);
};

#include <BaseLib/Serialization/deserializer_inl.hpp>
