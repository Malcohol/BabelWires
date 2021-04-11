/**
 * The XmlSerializer implements Serializer and writes data in an XML representation.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "Common/Serialization/serializer.hpp"

#include <tinyxml2.h>

#include <ostream>
#include <vector>

namespace babelwires {

    class XmlSerializer : public Serializer {
      public:
        /// Create a serializer
        XmlSerializer();

        void serializeValue(std::string_view key, bool value) override;
        void serializeValue(std::string_view key, std::string_view value) override;
        void serializeValue(std::string_view key, std::uint32_t value) override;
        void serializeValue(std::string_view key, std::uint16_t value) override;
        void serializeValue(std::string_view key, std::uint8_t value) override;
        void serializeValue(std::string_view key, std::int32_t value) override;
        void serializeValue(std::string_view key, std::int16_t value) override;
        void serializeValue(std::string_view key, std::int8_t value) override;

        /// Writes the contents to the stream.
        /// Asserts that all pushed objects have been popped.
        void write(std::ostream& os);

      protected:
        void doPushObject(std::string_view typeName) override;
        void doPushObjectWithKey(std::string_view typeName, std::string_view key) override;
        void doPopObject() override;
        void doPushArray(std::string_view key) override;
        void doPopArray() override;

      private:
        void contextPush(tinyxml2::XMLElement* element, bool isArray);
        void contextPop();
        tinyxml2::XMLElement* getCurrentElement();
        tinyxml2::XMLNode* getCurrentNode();

      private:
        tinyxml2::XMLDocument m_doc;
        struct ContextEntry {
            tinyxml2::XMLElement* m_element;
            bool m_isArray = false;
            bool m_isArrayElement = false;
        };

        std::vector<ContextEntry> m_xmlContext;
    };

} // namespace babelwires
