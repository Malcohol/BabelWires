/**
 * The XmlDeserializer implements the Deserializer and loads data from an XML representation.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Serialization/deserializer.hpp>

#include <set>
#include <tinyxml2.h>

namespace babelwires {

    class XmlDeserializer : public Deserializer {
      public:
        XmlDeserializer(std::string_view xmlText, const DeserializationRegistry& deserializationRegistry,
                        UserLogger& userLogger);

        ResultT<bool> tryDeserializeValue(std::string_view key, bool& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::string& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::uint64_t& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::uint32_t& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::uint16_t& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::uint8_t& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::int64_t& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::int32_t& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::int16_t& value) override;
        ResultT<bool> tryDeserializeValue(std::string_view key, std::int8_t& value) override;

        /// Client code must call this before this object is destroyed.
        using Deserializer::finalize;

      protected:
        struct IteratorImpl : Deserializer::AbstractIterator {
            IteratorImpl(XmlDeserializer& deserializer, const tinyxml2::XMLElement* arrayElement);

            void operator++() override;
            bool isValid() const override;

            XmlDeserializer& m_deserializer;
            const tinyxml2::XMLElement* m_currentElement;
        };

        std::unique_ptr<AbstractIterator> getIteratorImpl() override;

        bool pushObject(std::string_view key) override;
        Result popObject() override;

        bool pushArray(std::string_view key) override;
        Result popArray() override;

        std::string_view getCurrentTypeName() override;

        ErrorStorage addContextDescription(const ErrorStorage& e) const override;

      private:
        friend IteratorImpl;
        void contextPush(std::string_view key, const tinyxml2::XMLElement* element, bool isArray);
        Result contextPop();
        const tinyxml2::XMLElement* getCurrentElement() const;
        const tinyxml2::XMLNode* getCurrentNode() const;
        void keyWasQueried(std::string_view key);

        template <typename INT_TYPE>
        ResultT<bool> getIntValue(const tinyxml2::XMLElement& element, std::string_view key, INT_TYPE& value);

      private:
        tinyxml2::XMLDocument m_doc;

        struct ContextEntry {
            const tinyxml2::XMLElement* m_element = nullptr;
            bool m_isArray = false;
            bool m_isArrayElement = false;

            /// The set of children which have been accessed.
            /// If a client fails to query for a child, then an exception is thrown declaring that the child was not
            /// expected.
            std::set<std::string, std::less<>> m_keysQueried;
        };

        std::vector<ContextEntry> m_xmlContext;
    };

} // namespace babelwires
