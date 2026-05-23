/**
 * The YamlDeserializer implements the Deserializer and loads data from a YAML representation.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/Serialization/YAML/yamlDeserializer.hpp>

#include <BaseLib/Result/resultDSL.hpp>

#include <limits>
#include <string>
#include <type_traits>

namespace {
    template <typename T> using BigInt = std::conditional_t<std::is_signed_v<T>, std::int64_t, std::uint64_t>;

    std::string_view tryGetTypeNameFromTag(const YAML::Node& node) {
        const std::string& tag = node.Tag();
        if ((tag.size() > 1) && (tag.front() == '!')) {
            return std::string_view(tag).substr(1);
        }
        return {};
    }
}

void babelwires::YamlDeserializer::keyWasQueried(std::string_view key) {
    if (!m_yamlContext.empty()) {
        m_yamlContext.back().m_keysQueried.insert(std::string(key));
    }
}

babelwires::YamlDeserializer::YamlDeserializer(const DeserializationRegistryInterface& deserializationRegistry,
                                               UserLogger& userLogger)
    : Deserializer(userLogger, deserializationRegistry) {
    m_yamlContext.emplace_back();
}

babelwires::Result babelwires::YamlDeserializer::parse(std::string_view yamlText) {
    try {
        const std::vector<YAML::Node> documents = YAML::LoadAll(std::string(yamlText));
        if (documents.empty()) {
            return Error() << "YAML parsing failed: the document was empty";
        }
        if (documents.size() != 1) {
            return Error() << "YAML parsing failed: expected exactly one document";
        }
        if (!documents[0].IsMap()) {
            return Error() << "YAML parsing failed: the top-level document must be a map";
        }

        m_documentNode = YAML::Node(YAML::NodeType::Map);
        m_documentNode[c_contentsKey] = documents[0];
        m_yamlContext.clear();
        m_yamlContext.emplace_back(ContextEntry{m_documentNode, false, false, std::string(), {}});
        return initialize();
    } catch (const YAML::Exception& e) {
        return Error() << "YAML parsing failed: " << e.what();
    }
}

const YAML::Node& babelwires::YamlDeserializer::getCurrentNode() const {
    assert(!m_yamlContext.empty() && m_yamlContext.back().m_node && "There is no current YAML node");
    return m_yamlContext.back().m_node;
}

void babelwires::YamlDeserializer::contextPush(std::string_view key, const YAML::Node& node, bool isArray,
                                               bool isArrayElement) {
    if (!key.empty()) {
        keyWasQueried(key);
    }
    m_yamlContext.emplace_back(ContextEntry{node, isArray, isArrayElement, std::string(key), {}});
}

babelwires::Result babelwires::YamlDeserializer::contextPop() {
    const ContextEntry& currentContext = m_yamlContext.back();
    const YAML::Node& currentNode = currentContext.m_node;

    if (currentContext.m_isArray) {
        if (!currentNode.IsSequence()) {
            return Error() << "Expected a sequence";
        }
    } else if (currentNode.IsMap()) {
        std::set<std::string, std::less<>> keysSeen;
        for (const auto& keyAndValue : currentNode) {
            const YAML::Node keyNode = keyAndValue.first;
            if (!keyNode.IsScalar()) {
                return Error() << "Unexpected non-scalar key in YAML map";
            }
            const std::string key = keyNode.Scalar();
            if (!keysSeen.insert(key).second) {
                return Error() << "Duplicate YAML key \"" << key << "\"";
            }
            if (currentContext.m_keysQueried.find(key) == currentContext.m_keysQueried.end()) {
                return Error() << "Unexpected field \"" << key << "\"";
            }
        }
    } else if (!(currentContext.m_isArrayElement && currentNode.IsScalar())) {
        return Error() << "Unexpected YAML content";
    }

    m_yamlContext.pop_back();
    return {};
}

babelwires::ResultT<bool> babelwires::YamlDeserializer::tryGetValueNode(std::string_view key, YAML::Node& valueNode) {
    const YAML::Node& currentNode = getCurrentNode();
    if (m_yamlContext.back().m_isArrayElement && currentNode.IsScalar()) {
        if (key == c_defaultValueArrayValueKey) {
            valueNode = currentNode;
            return true;
        }
        return false;
    }

    if (!currentNode.IsMap()) {
        return Error() << "Expected a map when reading \"" << key << "\"";
    }

    const YAML::Node childNode = currentNode[key];
    if (!childNode) {
        return false;
    }
    keyWasQueried(key);
    valueNode = childNode;
    return true;
}

template <typename INT_TYPE>
babelwires::ResultT<bool> babelwires::YamlDeserializer::getIntValue(std::string_view key, INT_TYPE& value) {
    YAML::Node valueNode;
    ASSIGN_OR_ERROR(const bool wasFound, tryGetValueNode(key, valueNode));
    if (!wasFound) {
        return false;
    }
    if (!valueNode.IsScalar()) {
        return Error() << "Field \"" << key << "\" did not contain an int";
    }

    try {
        const BigInt<INT_TYPE> bigValue = valueNode.as<BigInt<INT_TYPE>>();
        if ((bigValue < std::numeric_limits<INT_TYPE>::min()) || (bigValue > std::numeric_limits<INT_TYPE>::max())) {
            return Error() << "Field \"" << key << "\" was out of range";
        }
        value = static_cast<INT_TYPE>(bigValue);
        return true;
    } catch (const YAML::BadConversion&) {
        return Error() << "Field \"" << key << "\" did not contain an int";
    }
}

babelwires::ResultT<bool> babelwires::YamlDeserializer::tryDeserializeValue(std::string_view key, std::uint64_t& value) {
    return getIntValue(key, value);
}

babelwires::ResultT<bool> babelwires::YamlDeserializer::tryDeserializeValue(std::string_view key, std::uint32_t& value) {
    return getIntValue(key, value);
}

babelwires::ResultT<bool> babelwires::YamlDeserializer::tryDeserializeValue(std::string_view key, std::uint16_t& value) {
    return getIntValue(key, value);
}

babelwires::ResultT<bool> babelwires::YamlDeserializer::tryDeserializeValue(std::string_view key, std::uint8_t& value) {
    return getIntValue(key, value);
}

babelwires::ResultT<bool> babelwires::YamlDeserializer::tryDeserializeValue(std::string_view key, std::int32_t& value) {
    return getIntValue(key, value);
}

babelwires::ResultT<bool> babelwires::YamlDeserializer::tryDeserializeValue(std::string_view key, std::int64_t& value) {
    return getIntValue(key, value);
}

babelwires::ResultT<bool> babelwires::YamlDeserializer::tryDeserializeValue(std::string_view key, std::int16_t& value) {
    return getIntValue(key, value);
}

babelwires::ResultT<bool> babelwires::YamlDeserializer::tryDeserializeValue(std::string_view key, std::int8_t& value) {
    return getIntValue(key, value);
}

babelwires::ResultT<bool> babelwires::YamlDeserializer::tryDeserializeValue(std::string_view key, bool& value) {
    YAML::Node valueNode;
    ASSIGN_OR_ERROR(const bool wasFound, tryGetValueNode(key, valueNode));
    if (!wasFound) {
        return false;
    }
    if (!valueNode.IsScalar()) {
        return Error() << "Field \"" << key << "\" did not contain a bool";
    }
    try {
        value = valueNode.as<bool>();
        return true;
    } catch (const YAML::BadConversion&) {
        return Error() << "Field \"" << key << "\" did not contain a bool";
    }
}

babelwires::ResultT<bool> babelwires::YamlDeserializer::tryDeserializeValue(std::string_view key, std::string& value) {
    YAML::Node valueNode;
    ASSIGN_OR_ERROR(const bool wasFound, tryGetValueNode(key, valueNode));
    if (!wasFound) {
        return false;
    }
    if (!valueNode.IsScalar()) {
        return Error() << "Field \"" << key << "\" did not contain a scalar";
    }
    value = valueNode.Scalar();
    return true;
}

bool babelwires::YamlDeserializer::pushObject(std::string_view key) {
    const YAML::Node& currentNode = getCurrentNode();
    if (!currentNode.IsMap()) {
        return false;
    }
    const YAML::Node childNode = currentNode[key];
    if (!childNode) {
        return false;
    }
    contextPush(key, childNode, false, false);
    return true;
}

babelwires::Result babelwires::YamlDeserializer::popObject() {
    return contextPop();
}

bool babelwires::YamlDeserializer::pushArray(std::string_view key) {
    const YAML::Node& currentNode = getCurrentNode();
    if (!currentNode.IsMap()) {
        return false;
    }
    const YAML::Node childNode = currentNode[key];
    if (!childNode) {
        return false;
    }
    contextPush(key, childNode, true, false);
    return true;
}

babelwires::Result babelwires::YamlDeserializer::popArray() {
    return contextPop();
}

std::string_view babelwires::YamlDeserializer::getCurrentTypeName() {
    assert(!m_yamlContext.back().m_isArray && "You cannot query the type of an array");

    const YAML::Node& currentNode = getCurrentNode();
    if (const std::string_view tagTypeName = tryGetTypeNameFromTag(currentNode); !tagTypeName.empty()) {
        return tagTypeName;
    }
    if (currentNode.IsMap()) {
        if (!m_yamlContext.back().m_isArrayElement) {
            return m_yamlContext.back().m_key;
        }
    }
    return {};
}

bool babelwires::YamlDeserializer::currentValueArrayElementMatchesType(std::string_view expectedTypeName) {
    const YAML::Node& currentNode = getCurrentNode();
    if (currentNode.IsScalar()) {
        return true;
    }
    if (currentNode.IsMap()) {
        const std::string_view tagTypeName = tryGetTypeNameFromTag(currentNode);
        if (tagTypeName.empty()) {
            return true;
        }
        return tagTypeName == expectedTypeName;
    }
    return false;
}

babelwires::YamlDeserializer::IteratorImpl::IteratorImpl(YamlDeserializer& deserializer, const YAML::Node& arrayNode)
    : m_deserializer(deserializer)
    , m_arrayNode(arrayNode) {
    if (m_arrayNode.size() > 0) {
        m_deserializer.contextPush({}, m_arrayNode[m_index], false, true);
    }
}

babelwires::Result babelwires::YamlDeserializer::IteratorImpl::advance() {
    assert(isValid());
    DO_OR_ERROR(m_deserializer.contextPop());
    ++m_index;
    if (isValid()) {
        m_deserializer.contextPush({}, m_arrayNode[m_index], false, true);
    }
    return {};
}

bool babelwires::YamlDeserializer::IteratorImpl::isValid() const {
    return m_index < m_arrayNode.size();
}

std::unique_ptr<babelwires::Deserializer::AbstractIterator> babelwires::YamlDeserializer::getIteratorImpl() {
    assert(m_yamlContext.back().m_isArray && "Current YAML node is not an array");
    return std::make_unique<IteratorImpl>(*this, m_yamlContext.back().m_node);
}

babelwires::ErrorStorage babelwires::YamlDeserializer::addContextDescription(const ErrorStorage& e) const {
    Error error;
    error << e;
    if (!m_yamlContext.empty()) {
        const ContextEntry& currentContext = m_yamlContext.back();
        if (!currentContext.m_key.empty()) {
            error << " when parsing field \"" << currentContext.m_key << "\"";
        } else if (currentContext.m_isArrayElement) {
            error << " when parsing a YAML array element";
        }

        const YAML::Mark mark = currentContext.m_node.Mark();
        if (mark.line >= 0) {
            error << " at line " << (mark.line + 1);
            if (mark.column >= 0) {
                error << ", column " << (mark.column + 1);
            }
        }
    }
    return error;
}