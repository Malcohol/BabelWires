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
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNode.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
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

void babelwires::UiData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("x", m_uiPosition.m_x);
    deserializer.deserializeValue("y", m_uiPosition.m_y);
    deserializer.deserializeValue("width", m_uiSize.m_width);
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
                                                                                          UserLogger& userLogger,
                                                                                          NodeId newId) const {
    std::unique_ptr<babelwires::Node> newNode = doCreateNode(context, userLogger, newId);
    newNode->applyLocalModifiers(userLogger);
    return newNode;
}

void babelwires::NodeData::addCommonKeyValuePairs(Serializer& serializer) const {
    serializer.serializeValue("id", m_id);
    serializer.serializeValue("factory", m_factoryIdentifier);
    // Factory versions are handled by the projectBundle.
}

void babelwires::NodeData::getCommonKeyValuePairs(Deserializer& deserializer) {
    deserializer.deserializeValue("id", m_id);
    deserializer.deserializeValue("factory", m_factoryIdentifier);
    // Factory versions are handled by the projectBundle.
}

void babelwires::NodeData::serializeModifiers(Serializer& serializer) const {
    serializer.serializeArray("modifiers", m_modifiers);
}

void babelwires::NodeData::deserializeModifiers(Deserializer& deserializer) {
    for (auto it = deserializer.deserializeArray<ModifierData>("modifiers", Deserializer::IsOptional::Optional);
         it.isValid(); ++it) {
        m_modifiers.emplace_back(it.getObject());
    }
}

void babelwires::NodeData::serializeUiData(Serializer& serializer) const {
    serializer.serializeObject(m_uiData);
    serializer.serializeValueArray("expandedPaths", m_expandedPaths, "path");
}

void babelwires::NodeData::deserializeUiData(Deserializer& deserializer) {
    if (auto uiData =
            deserializer.deserializeObject<UiData>(UiData::serializationType, Deserializer::IsOptional::Optional)) {
        m_uiData = *uiData;
    }
    for (auto it = deserializer.deserializeValueArray<Path>("expandedPaths", Deserializer::IsOptional::Optional,
                                                                   "path");
         it.isValid(); ++it) {
        m_expandedPaths.emplace_back(std::move(it.deserializeValue()));
    }
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
