/**
 * NodeDatas carry the data sufficient to reconstruct a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>

#include <BabelWiresLib/Processors/processor.hpp>
#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Processors/processorFactoryRegistry.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNode.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>

#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>
#include <BaseLib/exceptions.hpp>

#include <cassert>

void babelwires::UiData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("x", m_uiPosition.m_x);
    serializer.serializeValue("y", m_uiPosition.m_y);
    serializer.serializeValue("width", m_uiSize.m_width);
}

babelwires::Result babelwires::UiData::deserializeContents(Deserializer& deserializer) {
    DO_OR_ERROR(deserializer.deserializeValue("x", m_uiPosition.m_x));
    DO_OR_ERROR(deserializer.deserializeValue("y", m_uiPosition.m_y));
    DO_OR_ERROR(deserializer.deserializeValue("width", m_uiSize.m_width));
    return {};
}

babelwires::NodeData::NodeData(const NodeData& other, ShallowCloneContext)
    : Cloneable(other)
    , m_factoryIdentifier(other.m_factoryIdentifier)
    , m_factoryVersion(other.m_factoryVersion)
    , m_id(other.m_id)
    , m_uiData(other.m_uiData) {}

babelwires::NodeData::NodeData(const NodeData& other)
    : NodeData(other, ShallowCloneContext()) {
    for (auto&& m : other.m_modifiers) {
        m_modifiers.push_back(m->clone());
    }
    m_expandedPaths = other.m_expandedPaths;
}

std::unique_ptr<babelwires::Node> babelwires::NodeData::createNode(const ProjectContext& context,
                                                                   UserLogger& userLogger, NodeId newId) const {
    std::unique_ptr<babelwires::Node> newNode = doCreateNode(context, userLogger, newId);
    newNode->applyLocalModifiers(userLogger);
    return newNode;
}

void babelwires::NodeData::addCommonKeyValuePairs(Serializer& serializer) const {
    serializer.serializeValue("id", m_id);
    serializer.serializeValue("factory", m_factoryIdentifier);
    // Factory versions are handled by the projectBundle.
}

babelwires::Result babelwires::NodeData::getCommonKeyValuePairs(Deserializer& deserializer) {
    DO_OR_ERROR(deserializer.deserializeValue("id", m_id));
    DO_OR_ERROR(deserializer.deserializeValue("factory", m_factoryIdentifier));
    // Factory versions are handled by the projectBundle.
    return {};
}

void babelwires::NodeData::serializeModifiers(Serializer& serializer) const {
    serializer.serializeArray("modifiers", m_modifiers);
}

babelwires::Result babelwires::NodeData::deserializeModifiers(Deserializer& deserializer) {
    ASSIGN_OR_ERROR(auto it, deserializer.tryDeserializeArray<ModifierData>("modifiers"));
    for (; it.isValid(); ++it) {
        ASSIGN_OR_ERROR(m_modifiers.emplace_back(), it.getObject());
    }
    return {};
}

void babelwires::NodeData::serializeUiData(Serializer& serializer) const {
    serializer.serializeObject(m_uiData);
    serializer.serializeValueArray("expandedPaths", m_expandedPaths, "path");
}

babelwires::Result babelwires::NodeData::deserializeUiData(Deserializer& deserializer) {
    DO_OR_ERROR(deserializer.tryDeserializeObjectByValue<UiData>(m_uiData));
    ASSIGN_OR_ERROR(auto it, deserializer.tryDeserializeValueArray<Path>("expandedPaths", "path"));
    for (; it.isValid(); ++it) {
        ASSIGN_OR_ERROR(m_expandedPaths.emplace_back(), it.deserializeValue());
    }
    return {};
}

void babelwires::NodeData::visitIdentifiers(IdentifierVisitor& visitor) {
    visitor(m_factoryIdentifier);
    for (auto& m : m_modifiers) {
        m->visitIdentifiers(visitor);
    }
    for (auto& p : m_expandedPaths) {
        p.visitIdentifiers(visitor);
    }
}

void babelwires::NodeData::visitFilePaths(FilePathVisitor& visitor) {
    for (auto& m : m_modifiers) {
        m->visitFilePaths(visitor);
    }
}
