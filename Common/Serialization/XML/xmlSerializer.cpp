#include "Common/Serialization/XML/xmlSerializer.hpp"

#include <cassert>
#include <string>

#include "Common/Serialization/XML/xmlCommon.h"

tinyxml2::XMLElement* babelwires::XmlSerializer::getCurrentElement() {
    assert(!m_xmlContext.empty() && "There is no current element.");
    return m_xmlContext.back().m_element;
}

tinyxml2::XMLNode* babelwires::XmlSerializer::getCurrentNode() {
    if (m_xmlContext.empty()) {
        return &m_doc;
    } else {
        return m_xmlContext.back().m_element;
    }
}

babelwires::XmlSerializer::XmlSerializer() {
    m_doc.InsertEndChild(m_doc.NewDeclaration());
    initialize();
}

void babelwires::XmlSerializer::doPushArray(std::string_view key) {
    tinyxml2::XMLElement* newElement = m_doc.NewElement(toCStr(key));
    getCurrentNode()->InsertEndChild(newElement);
    contextPush(newElement, true);
}

void babelwires::XmlSerializer::doPopArray() {
    contextPop();
}

void babelwires::XmlSerializer::doPushObject(std::string_view typeName) {
    tinyxml2::XMLElement* newElement = m_doc.NewElement(toCStr(typeName));
    getCurrentNode()->InsertEndChild(newElement);
    contextPush(newElement, false);
}

void babelwires::XmlSerializer::doPushObjectWithKey(std::string_view typeName, std::string_view key) {
    tinyxml2::XMLElement* newElement = m_doc.NewElement(toCStr(key));
    if (key != typeName) {
        newElement->SetAttribute("typeName", toCStr(typeName));
    }
    getCurrentNode()->InsertEndChild(newElement);
    contextPush(newElement, false);
}

void babelwires::XmlSerializer::doPopObject() {
    contextPop();
}

void babelwires::XmlSerializer::serializeValue(std::string_view key, bool value) {
    getCurrentElement()->SetAttribute(toCStr(key), value);
}

void babelwires::XmlSerializer::serializeValue(std::string_view key, std::string_view value) {
    getCurrentElement()->SetAttribute(toCStr(key), toCStr(value));
}

void babelwires::XmlSerializer::serializeValue(std::string_view key, std::uint32_t value) {
    getCurrentElement()->SetAttribute(toCStr(key), value);
}

void babelwires::XmlSerializer::serializeValue(std::string_view key, std::uint16_t value) {
    getCurrentElement()->SetAttribute(toCStr(key), value);
}

void babelwires::XmlSerializer::serializeValue(std::string_view key, std::uint8_t value) {
    getCurrentElement()->SetAttribute(toCStr(key), value);
}

void babelwires::XmlSerializer::serializeValue(std::string_view key, std::int32_t value) {
    getCurrentElement()->SetAttribute(toCStr(key), value);
}

void babelwires::XmlSerializer::serializeValue(std::string_view key, std::int16_t value) {
    getCurrentElement()->SetAttribute(toCStr(key), value);
}

void babelwires::XmlSerializer::serializeValue(std::string_view key, std::int8_t value) {
    getCurrentElement()->SetAttribute(toCStr(key), value);
}

void babelwires::XmlSerializer::write(std::ostream& os) {
    finalize();
    assert(m_xmlContext.empty() && "Not all objects have been popped");
    tinyxml2::XMLPrinter printer;
    m_doc.Print(&printer);
    os << printer.CStr();
}

void babelwires::XmlSerializer::contextPush(tinyxml2::XMLElement* element, bool isArray) {
    const bool backWasArray = m_xmlContext.empty() ? false : m_xmlContext.back().m_isArray;
    m_xmlContext.emplace_back(ContextEntry{element, isArray, backWasArray});
}

void babelwires::XmlSerializer::contextPop() {
    m_xmlContext.pop_back();
}
