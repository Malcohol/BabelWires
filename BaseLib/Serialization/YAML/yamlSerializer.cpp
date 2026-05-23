/**
 * The YamlSerializer implements Serializer and writes data in a YAML representation.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/Serialization/YAML/yamlSerializer.hpp>

#include <cassert>
#include <string>

namespace {
    template <typename TValue> TValue normalizeYamlScalarValue(TValue value) {
        return value;
    }

    std::string makeLocalYamlTag(std::string_view typeName) {
        return std::string("!") + std::string(typeName);
    }

    std::string normalizeYamlScalarValue(std::string_view value) {
        return std::string(value);
    }

    unsigned int normalizeYamlScalarValue(std::uint16_t value) { return static_cast<unsigned int>(value); }
    unsigned int normalizeYamlScalarValue(std::uint8_t value) { return static_cast<unsigned int>(value); }
    int normalizeYamlScalarValue(std::int16_t value) { return static_cast<int>(value); }
    int normalizeYamlScalarValue(std::int8_t value) { return static_cast<int>(value); }

    bool shouldUseFlowStyle(const YAML::Node& node) {
        if (node.IsSequence()) {
            for (const YAML::Node& childNode : node) {
                if (!childNode.IsScalar()) {
                    return false;
                }
            }
            return true;
        }

        if (node.IsMap()) {
            for (const auto& entry : node) {
                if (!entry.first.IsScalar() || !entry.second.IsScalar()) {
                    return false;
                }
            }
            return true;
        }

        return false;
    }

    void applyPreferredStyle(YAML::Node& node) {
        if (shouldUseFlowStyle(node)) {
            node.SetStyle(YAML::EmitterStyle::Flow);
        }
    }
}

babelwires::YamlSerializer::YamlSerializer() { initialize(); }

YAML::Node& babelwires::YamlSerializer::getCurrentNode() {
    ensureCurrentNodeIsMaterialized();
    assert(!m_yamlContext.empty() && "There is no current YAML node");
    return m_yamlContext.back().m_node;
}

void babelwires::YamlSerializer::serializeScalarValue(std::string_view key, YAML::Node value) {
    ContextEntry& currentContext = m_yamlContext.back();
    if (currentContext.m_isPendingValueArrayElement && !currentContext.m_node &&
        (key == c_defaultValueArrayValueKey)) {
        currentContext.m_node = std::move(value);
        currentContext.m_isScalarArrayElement = true;
        return;
    }
    getCurrentNode()[key] = std::move(value);
}

void babelwires::YamlSerializer::ensureCurrentNodeIsMaterialized() {
    assert(!m_yamlContext.empty() && "There is no current YAML context");
    ContextEntry& currentContext = m_yamlContext.back();
    if (!currentContext.m_isPendingArrayElement || currentContext.m_node) {
        return;
    }
    currentContext.m_node = YAML::Node(YAML::NodeType::Map);
    currentContext.m_node.SetTag(makeLocalYamlTag(currentContext.m_pendingArrayElementTypeName));
}

void babelwires::YamlSerializer::doPushObject(std::string_view typeName) {
    if (m_yamlContext.empty()) {
        contextPush(YAML::Node(YAML::NodeType::Map), false);
        return;
    }

    assert(m_yamlContext.back().m_isArray && "Objects without keys can only appear at the root or inside arrays");
    contextPush(YAML::Node(YAML::NodeType::Undefined), false);
    ContextEntry& currentContext = m_yamlContext.back();
    currentContext.m_isPendingArrayElement = true;
    currentContext.m_pendingArrayElementTypeName = typeName;
}

void babelwires::YamlSerializer::doPushValueArrayElement(std::string_view typeName) {
    doPushObject(typeName);
    ContextEntry& currentContext = m_yamlContext.back();
    currentContext.m_isPendingValueArrayElement = true;
}

void babelwires::YamlSerializer::doPushObjectWithKey(std::string_view typeName, std::string_view key) {
    assert(!m_yamlContext.empty() && !m_yamlContext.back().m_isArray && "Keyed objects must be children of objects");
    ensureCurrentNodeIsMaterialized();
    YAML::Node newNode(YAML::NodeType::Map);
    if (key != typeName) {
        newNode.SetTag(makeLocalYamlTag(typeName));
    }
    contextPush(std::move(newNode), false, std::string(key));
}

void babelwires::YamlSerializer::doPopObject() {
    ContextEntry& currentContext = m_yamlContext.back();
    if (currentContext.m_isPendingArrayElement && !currentContext.m_node) {
        ensureCurrentNodeIsMaterialized();
    }
    ContextEntry completedContext = std::move(m_yamlContext.back());
    contextPop();
    applyPreferredStyle(completedContext.m_node);

    if (m_yamlContext.empty()) {
        m_root = completedContext.m_node;
    } else if (m_yamlContext.back().m_isArray) {
        m_yamlContext.back().m_node.push_back(completedContext.m_node);
    } else {
        m_yamlContext.back().m_node[completedContext.m_key] = completedContext.m_node;
    }
}

void babelwires::YamlSerializer::doPushArray(std::string_view key) {
    assert(!m_yamlContext.empty() && !m_yamlContext.back().m_isArray && "Arrays must be children of objects");
    ensureCurrentNodeIsMaterialized();
    contextPush(YAML::Node(YAML::NodeType::Sequence), true, std::string(key));
}

void babelwires::YamlSerializer::doPopArray() {
    ContextEntry completedContext = std::move(m_yamlContext.back());
    contextPop();
    applyPreferredStyle(completedContext.m_node);
    if (m_yamlContext.empty()) {
        m_root = completedContext.m_node;
    } else {
        m_yamlContext.back().m_node[completedContext.m_key] = completedContext.m_node;
    }
}

void babelwires::YamlSerializer::doSerializeValue(std::string_view key, bool value) {
    serializeScalarValue(key, YAML::Node(normalizeYamlScalarValue(value)));
}

void babelwires::YamlSerializer::doSerializeValue(std::string_view key, std::string_view value) {
    serializeScalarValue(key, YAML::Node(normalizeYamlScalarValue(value)));
}

void babelwires::YamlSerializer::doSerializeValue(std::string_view key, std::uint64_t value) {
    serializeScalarValue(key, YAML::Node(normalizeYamlScalarValue(value)));
}

void babelwires::YamlSerializer::doSerializeValue(std::string_view key, std::uint32_t value) {
    serializeScalarValue(key, YAML::Node(normalizeYamlScalarValue(value)));
}

void babelwires::YamlSerializer::doSerializeValue(std::string_view key, std::uint16_t value) {
    serializeScalarValue(key, YAML::Node(normalizeYamlScalarValue(value)));
}

void babelwires::YamlSerializer::doSerializeValue(std::string_view key, std::uint8_t value) {
    serializeScalarValue(key, YAML::Node(normalizeYamlScalarValue(value)));
}

void babelwires::YamlSerializer::doSerializeValue(std::string_view key, std::int64_t value) {
    serializeScalarValue(key, YAML::Node(normalizeYamlScalarValue(value)));
}

void babelwires::YamlSerializer::doSerializeValue(std::string_view key, std::int32_t value) {
    serializeScalarValue(key, YAML::Node(normalizeYamlScalarValue(value)));
}

void babelwires::YamlSerializer::doSerializeValue(std::string_view key, std::int16_t value) {
    serializeScalarValue(key, YAML::Node(normalizeYamlScalarValue(value)));
}

void babelwires::YamlSerializer::doSerializeValue(std::string_view key, std::int8_t value) {
    serializeScalarValue(key, YAML::Node(normalizeYamlScalarValue(value)));
}

bool babelwires::YamlSerializer::isOrdinaryFieldKeyReserved(std::string_view key) const {
    return keyHasReservedPrefix(key, c_dollarMetadataPrefix);
}

void babelwires::YamlSerializer::write(std::ostream& os) {
    finalize();
    assert(m_yamlContext.empty() && "Not all YAML nodes have been popped");

    YAML::Emitter emitter;
    emitter << m_root;
    os << emitter.c_str();
}

void babelwires::YamlSerializer::contextPush(YAML::Node node, bool isArray, std::string key) {
    m_yamlContext.emplace_back(ContextEntry{std::move(node), isArray, false, false, false, std::move(key)});
}

void babelwires::YamlSerializer::contextPop() {
    m_yamlContext.pop_back();
}