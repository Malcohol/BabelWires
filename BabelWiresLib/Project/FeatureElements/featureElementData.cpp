/**
 * FeatureElementDatas carry the data sufficient to reconstruct a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Project/FeatureElements/featureElementData.hpp"

#include "BabelWiresLib/Features/recordFeature.hpp"
#include "BabelWiresLib/Processors/processor.hpp"
#include "BabelWiresLib/Processors/processorFactory.hpp"
#include "BabelWiresLib/Processors/processorFactoryRegistry.hpp"
#include "BabelWiresLib/Project/FeatureElements/processorElement.hpp"
#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"
#include "BabelWiresLib/Project/projectContext.hpp"

#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"
#include "Common/exceptions.hpp"

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

babelwires::ElementData::ElementData(const ElementData& other, ShallowCloneContext)
    : Cloneable(other)
    , m_factoryIdentifier(other.m_factoryIdentifier)
    , m_factoryVersion(other.m_factoryVersion)
    , m_id(other.m_id)
    , m_uiData(other.m_uiData) {}

babelwires::ElementData::ElementData(const ElementData& other)
    : ElementData(other, ShallowCloneContext()) {
    for (auto&& m : other.m_modifiers) {
        m_modifiers.push_back(m->clone());
    }
    m_expandedPaths = other.m_expandedPaths;
}

std::unique_ptr<babelwires::FeatureElement> babelwires::ElementData::createFeatureElement(const ProjectContext& context,
                                                                                          UserLogger& userLogger,
                                                                                          ElementId newId) const {
    std::unique_ptr<babelwires::FeatureElement> newElement = doCreateFeatureElement(context, userLogger, newId);
    newElement->applyLocalModifiers(userLogger);
    return newElement;
}

void babelwires::ElementData::addCommonKeyValuePairs(Serializer& serializer) const {
    serializer.serializeValue("id", m_id);
    serializer.serializeValue("factory", m_factoryIdentifier);
    // Factory versions are handled by the projectBundle.
}

void babelwires::ElementData::getCommonKeyValuePairs(Deserializer& deserializer) {
    deserializer.deserializeValue("id", m_id);
    deserializer.deserializeValue("factory", m_factoryIdentifier);
    // Factory versions are handled by the projectBundle.
}

void babelwires::ElementData::serializeModifiers(Serializer& serializer) const {
    serializer.serializeArray("modifiers", m_modifiers);
}

void babelwires::ElementData::deserializeModifiers(Deserializer& deserializer) {
    for (auto it = deserializer.deserializeArray<ModifierData>("modifiers", Deserializer::IsOptional::Optional);
         it.isValid(); ++it) {
        m_modifiers.emplace_back(it.getObject());
    }
}

void babelwires::ElementData::serializeUiData(Serializer& serializer) const {
    serializer.serializeObject(m_uiData);
    serializer.serializeValueArray("expandedPaths", m_expandedPaths, "path");
}

void babelwires::ElementData::deserializeUiData(Deserializer& deserializer) {
    if (auto uiData =
            deserializer.deserializeObject<UiData>(UiData::serializationType, Deserializer::IsOptional::Optional)) {
        m_uiData = *uiData;
    }
    for (auto it = deserializer.deserializeValueArray<FeaturePath>("expandedPaths", Deserializer::IsOptional::Optional,
                                                                   "path");
         it.isValid(); ++it) {
        m_expandedPaths.emplace_back(std::move(it.deserializeValue()));
    }
}

void babelwires::ElementData::visitIdentifiers(IdentifierVisitor& visitor) {
    for (auto& m : m_modifiers) {
        m->visitIdentifiers(visitor);
    }
    for (auto& p : m_expandedPaths) {
        for (auto& s : p) {
            if (s.isField()) {
                visitor(s.getField());
            }
        }
    }
}

void babelwires::ElementData::visitFilePaths(FilePathVisitor& visitor) {
    for (auto& m : m_modifiers) {
        m->visitFilePaths(visitor);
    }
}
