/**
 * The XmlDeserializer implements the Deserializer and loads data from an XML representation.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/Serialization/XML/xmlDeserializer.hpp>

#include <BaseLib/Serialization/XML/xmlCommon.h>
#include <cassert>
#include <sstream>

void babelwires::XmlDeserializer::keyWasQueried(std::string_view key) {
    m_xmlContext.back().m_keysQueried.insert(std::string(key));
}

namespace {
    // Work around the fact that tinyxml2 doesn't use overloading for integer queries.

    template <typename T> using BigInt = std::conditional_t<std::is_signed_v<T>, std::int64_t, std::uint64_t>;

    template <typename T>
    std::enable_if_t<std::is_signed_v<T>, tinyxml2::XMLError>
    queryIntAttribute(const tinyxml2::XMLElement& element, std::string_view key, BigInt<T>& value) {
        return element.QueryInt64Attribute(babelwires::toCStr(key), &value);
    }

    template <typename T>
    std::enable_if_t<std::is_unsigned_v<T>, tinyxml2::XMLError>
    queryIntAttribute(const tinyxml2::XMLElement& element, std::string_view key, BigInt<T>& value) {
        return element.QueryUnsigned64Attribute(babelwires::toCStr(key), &value);
    }
} // namespace

template <typename INT_TYPE>
babelwires::ResultT<bool> babelwires::XmlDeserializer::getIntValue(const tinyxml2::XMLElement& element,
                                                                   std::string_view key, INT_TYPE& value) {
    keyWasQueried(key);
    BigInt<INT_TYPE> bigValue;
    switch (queryIntAttribute<INT_TYPE>(element, key, bigValue)) {
        case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
            return Error() << "Attribute \"" << key << "\" did not contain an int";
        case tinyxml2::XML_NO_ATTRIBUTE:
            return false;
        case tinyxml2::XML_SUCCESS:
            break;
        default:
            return Error() << "Unhandled XML error";
    }
    if ((bigValue < std::numeric_limits<INT_TYPE>::min()) || (bigValue > std::numeric_limits<INT_TYPE>::max())) {
        return Error() << "Attribute \"" << key << "\" was out of range";
    }
    value = static_cast<INT_TYPE>(bigValue);
    return true;
}

babelwires::XmlDeserializer::XmlDeserializer(const DeserializationRegistry& deserializationRegistry,
                                             UserLogger& userLogger)
    : Deserializer(userLogger, deserializationRegistry) {
    m_xmlContext.emplace_back();
}

babelwires::Result babelwires::XmlDeserializer::parse(std::string_view xmlText) {
    m_doc.Parse(xmlText.data(), xmlText.size());
    return initialize();
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

babelwires::ResultT<bool> babelwires::XmlDeserializer::tryDeserializeValue(std::string_view key, std::uint64_t& value) {
    return getIntValue(*getCurrentElement(), key, value);
}

babelwires::ResultT<bool> babelwires::XmlDeserializer::tryDeserializeValue(std::string_view key, std::uint32_t& value) {
    return getIntValue(*getCurrentElement(), key, value);
}

babelwires::ResultT<bool> babelwires::XmlDeserializer::tryDeserializeValue(std::string_view key, std::uint16_t& value) {
    return getIntValue(*getCurrentElement(), key, value);
}

babelwires::ResultT<bool> babelwires::XmlDeserializer::tryDeserializeValue(std::string_view key, std::uint8_t& value) {
    return getIntValue(*getCurrentElement(), key, value);
}

babelwires::ResultT<bool> babelwires::XmlDeserializer::tryDeserializeValue(std::string_view key, std::int32_t& value) {
    return getIntValue(*getCurrentElement(), key, value);
}

babelwires::ResultT<bool> babelwires::XmlDeserializer::tryDeserializeValue(std::string_view key, std::int64_t& value) {
    return getIntValue(*getCurrentElement(), key, value);
}

babelwires::ResultT<bool> babelwires::XmlDeserializer::tryDeserializeValue(std::string_view key, std::int16_t& value) {
    return getIntValue(*getCurrentElement(), key, value);
}

babelwires::ResultT<bool> babelwires::XmlDeserializer::tryDeserializeValue(std::string_view key, std::int8_t& value) {
    return getIntValue(*getCurrentElement(), key, value);
}

babelwires::ResultT<bool> babelwires::XmlDeserializer::tryDeserializeValue(std::string_view key, bool& value) {
    keyWasQueried(key);
    switch (getCurrentElement()->QueryBoolAttribute(babelwires::toCStr(key), &value)) {
        case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
            return Error() << "Attribute \"" << key << "\" did not contain a bool";
        case tinyxml2::XML_NO_ATTRIBUTE:
            return false;
        case tinyxml2::XML_SUCCESS:
            break;
        default:
            return Error() << "Unhandled XML error";
    }
    return true;
}

babelwires::ResultT<bool> babelwires::XmlDeserializer::tryDeserializeValue(std::string_view key, std::string& value) {
    keyWasQueried(key);
    const char* attr = getCurrentElement()->Attribute(toCStr(key));
    if (!attr) {
        return false;
    }
    value = attr;
    return true;
}

void babelwires::XmlDeserializer::contextPush(std::string_view key, const tinyxml2::XMLElement* element, bool isArray) {
    keyWasQueried(key);
    m_xmlContext.emplace_back(ContextEntry{element, isArray, m_xmlContext.back().m_isArray});
}

babelwires::Result babelwires::XmlDeserializer::contextPop() {
    // Check that all the children were queried, to ensure all content in the file gets deserialized correctly.
    const tinyxml2::XMLNode* node = getCurrentNode()->FirstChild();
    while (node) {
        auto& keysQueried = m_xmlContext.back().m_keysQueried;
        if (const tinyxml2::XMLElement* element = node->ToElement()) {
            const char* const name = element->Name();
            if (keysQueried.find(name) == keysQueried.end()) {
                return Error() << "Unexpected element \"" << name << "\"";
            }
        } else if (!node->ToComment()) {
            return Error() << "Unexpected content";
        }

        node = node->NextSibling();
    }

    const tinyxml2::XMLAttribute* attr = getCurrentElement()->FirstAttribute();
    while (attr) {
        auto& keysQueried = m_xmlContext.back().m_keysQueried;
        if (keysQueried.find(attr->Name()) == keysQueried.end()) {
            return Error() << "Unexpected attribute " << attr->Name();
        }

        attr = attr->Next();
    }

    m_xmlContext.pop_back();
    return {};
}

bool babelwires::XmlDeserializer::pushObject(std::string_view key) {
    const tinyxml2::XMLElement* element = getCurrentNode()->FirstChildElement(toCStr(key));
    if (element) {
        contextPush(key, element, false);
    }
    return element;
}

babelwires::Result babelwires::XmlDeserializer::popObject() {
    return contextPop();
}

bool babelwires::XmlDeserializer::pushArray(std::string_view key) {
    const tinyxml2::XMLElement* element = getCurrentNode()->FirstChildElement(toCStr(key));
    if (element) {
        contextPush(key, element, true);
    }
    return element;
}

babelwires::Result babelwires::XmlDeserializer::popArray() {
    return contextPop();
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
    // TODO RESULT
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

babelwires::ErrorStorage babelwires::XmlDeserializer::addContextDescription(const ErrorStorage& e) const {
    const tinyxml2::XMLNode* const node = getCurrentNode();
    Error error;
    error << e;
    if (!m_xmlContext.empty()) {
        if (const tinyxml2::XMLElement* const element = m_xmlContext.back().m_element) {
            error << " when parsing element <" << element->Name() << ">";
        }
    }
    error << " at line " << node->GetLineNum();
    return error;
}