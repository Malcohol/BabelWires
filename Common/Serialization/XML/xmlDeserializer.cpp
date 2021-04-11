/**
 * The XmlDeserializer implements the Deserializer and loads data from an XML representation.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "Common/Serialization/XML/xmlDeserializer.hpp"

#include "Common/Serialization/XML/xmlCommon.h"
#include <cassert>
#include <sstream>

void babelwires::XmlDeserializer::keyWasQueried(std::string_view key) {
    m_xmlContext.back().m_keysQueried.insert(std::string(key));
}

template <typename INT_TYPE>
bool babelwires::XmlDeserializer::getIntValue(const tinyxml2::XMLElement& element, std::string_view key,
                                              INT_TYPE& value, IsOptional isOptional) {
    keyWasQueried(key);
    int ret;
    switch (element.QueryIntAttribute(babelwires::toCStr(key), &ret)) {
        case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
            throw babelwires::ParseException() << "Attribute \"" << key << "\" did not contain an int";
        case tinyxml2::XML_NO_ATTRIBUTE:
            if (isOptional == babelwires::Deserializer::IsOptional::Required) {
                throw babelwires::ParseException() << "Required attribute \"" << key << "\" not found";
            }
            return false;
        case tinyxml2::XML_SUCCESS:
            break;
    }
    if ((ret < std::numeric_limits<INT_TYPE>::min()) || (ret > std::numeric_limits<INT_TYPE>::max())) {
        throw babelwires::ParseException() << "Attribute \"" << key << "\" was out of range";
    }
    value = static_cast<INT_TYPE>(ret);
    return true;
}

babelwires::XmlDeserializer::XmlDeserializer(std::string_view xmlText,
                                             const DeserializationRegistry& deserializationRegistry,
                                             UserLogger& userLogger)
    : Deserializer(userLogger, deserializationRegistry) {
    m_doc.Parse(xmlText.data(), xmlText.size());
    m_xmlContext.emplace_back();
    initialize();
}

const tinyxml2::XMLElement* babelwires::XmlDeserializer::getCurrentElement() const {
    assert(m_xmlContext.back().m_element && "There is no current element");
    return m_xmlContext.back().m_element;
}

const tinyxml2::XMLNode* babelwires::XmlDeserializer::getCurrentNode() const {
    if (m_xmlContext.size() == 1) {
        return &m_doc;
    } else {
        return getCurrentElement();
    }
}

bool babelwires::XmlDeserializer::deserializeValue(std::string_view key, std::uint32_t& value, IsOptional isOptional) {
    return getIntValue(*getCurrentElement(), key, value, isOptional);
}

bool babelwires::XmlDeserializer::deserializeValue(std::string_view key, std::uint16_t& value, IsOptional isOptional) {
    return getIntValue(*getCurrentElement(), key, value, isOptional);
}

bool babelwires::XmlDeserializer::deserializeValue(std::string_view key, std::uint8_t& value, IsOptional isOptional) {
    return getIntValue(*getCurrentElement(), key, value, isOptional);
}

bool babelwires::XmlDeserializer::deserializeValue(std::string_view key, std::int32_t& value, IsOptional isOptional) {
    return getIntValue(*getCurrentElement(), key, value, isOptional);
}

bool babelwires::XmlDeserializer::deserializeValue(std::string_view key, std::int16_t& value, IsOptional isOptional) {
    return getIntValue(*getCurrentElement(), key, value, isOptional);
}

bool babelwires::XmlDeserializer::deserializeValue(std::string_view key, std::int8_t& value, IsOptional isOptional) {
    return getIntValue(*getCurrentElement(), key, value, isOptional);
}

bool babelwires::XmlDeserializer::deserializeValue(std::string_view key, bool& value, IsOptional isOptional) {
    keyWasQueried(key);
    switch (getCurrentElement()->QueryBoolAttribute(babelwires::toCStr(key), &value)) {
        case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
            throw babelwires::ParseException() << "Attribute \"" << key << "\" did not contain an bool";
        case tinyxml2::XML_NO_ATTRIBUTE:
            if (isOptional == babelwires::Deserializer::IsOptional::Required) {
                throw babelwires::ParseException() << "Required attribute \"" << key << "\" not found";
            }
            return false;
        case tinyxml2::XML_SUCCESS:
            break;
    }
    return true;
}

bool babelwires::XmlDeserializer::deserializeValue(std::string_view key, std::string& value, IsOptional isOptional) {
    keyWasQueried(key);
    const char* attr = getCurrentElement()->Attribute(toCStr(key));
    if (!attr) {
        if (isOptional == babelwires::Deserializer::IsOptional::Required) {
            throw babelwires::ParseException() << "No such attribute \"" << key << "\"";
        } else {
            return false;
        }
    }
    value = attr;
    return true;
}

void babelwires::XmlDeserializer::contextPush(std::string_view key, const tinyxml2::XMLElement* element, bool isArray) {
    keyWasQueried(key);
    m_xmlContext.emplace_back(ContextEntry{element, isArray, m_xmlContext.back().m_isArray});
}

void babelwires::XmlDeserializer::contextPop() {
    // Check that all the children were queried, to ensure all content in the file gets deserialized correctly.
    const tinyxml2::XMLNode* node = getCurrentNode()->FirstChild();
    while (node) {
        auto& keysQueried = m_xmlContext.back().m_keysQueried;
        if (const tinyxml2::XMLElement* element = node->ToElement()) {
            const char* const name = element->Name();
            if (keysQueried.find(name) == keysQueried.end()) {
                throw babelwires::ParseException() << "Unexpected element \"" << name << "\"";
            }
        } else if (!node->ToComment()) {
            throw babelwires::ParseException() << "Unexpected content";
        }

        node = node->NextSibling();
    }

    const tinyxml2::XMLAttribute* attr = getCurrentElement()->FirstAttribute();
    while (attr) {
        auto& keysQueried = m_xmlContext.back().m_keysQueried;
        if (keysQueried.find(attr->Name()) == keysQueried.end()) {
            throw babelwires::ParseException() << "Unexpected attribute " << attr->Name();
        }

        attr = attr->Next();
    }

    m_xmlContext.pop_back();
}

bool babelwires::XmlDeserializer::pushObject(std::string_view key) {
    const tinyxml2::XMLElement* element = getCurrentNode()->FirstChildElement(toCStr(key));
    if (element) {
        contextPush(key, element, false);
    }
    return element;
}

void babelwires::XmlDeserializer::popObject() {
    contextPop();
}

bool babelwires::XmlDeserializer::pushArray(std::string_view key) {
    const tinyxml2::XMLElement* element = getCurrentNode()->FirstChildElement(toCStr(key));
    if (element) {
        contextPush(key, element, true);
    }
    return element;
}

void babelwires::XmlDeserializer::popArray() {
    contextPop();
}

std::string_view babelwires::XmlDeserializer::getCurrentTypeName() {
    assert(!m_xmlContext.back().m_isArray && "You can't query the type of an array");
    if (m_xmlContext.back().m_isArrayElement) {
        return getCurrentElement()->Name();
    } else if (const char* attr = getCurrentElement()->Attribute("typeName")) {
        keyWasQueried("typeName");
        return attr;
    } else {
        return getCurrentElement()->Name();
    }
}

babelwires::XmlDeserializer::IteratorImpl::IteratorImpl(XmlDeserializer& deserializer,
                                                        const tinyxml2::XMLElement* arrayElement)
    : m_deserializer(deserializer)
    , m_currentElement(arrayElement->FirstChildElement()) {
    if (m_currentElement) {
        m_deserializer.contextPush(m_currentElement->Name(), m_currentElement, false);
    }
}

void babelwires::XmlDeserializer::IteratorImpl::operator++() {
    assert(isValid());
    m_deserializer.contextPop();
    m_currentElement = m_currentElement->NextSiblingElement();
    if (m_currentElement) {
        m_deserializer.contextPush(m_currentElement->Name(), m_currentElement, false);
    }
}

bool babelwires::XmlDeserializer::IteratorImpl::isValid() const {
    return m_currentElement;
}

std::unique_ptr<babelwires::Deserializer::AbstractIterator> babelwires::XmlDeserializer::getIteratorImpl() {
    assert(m_xmlContext.back().m_isArray);
    return std::make_unique<IteratorImpl>(*this, m_xmlContext.back().m_element);
}

void babelwires::XmlDeserializer::addContextDescription(ParseException& e) const {
    const tinyxml2::XMLNode* const node = getCurrentNode();
    if (!m_xmlContext.empty()) {
        const tinyxml2::XMLElement* const element = getCurrentElement();
        e << " in element <" << element->Name() << ">";
    }
    e << " at line " << node->GetLineNum();
}
