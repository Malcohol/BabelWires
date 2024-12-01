/**
 * ValueElementData describes the construction of a ValueElement.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>

#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::ValueElementData::ValueElementData(TypeRef typeRef)
    : m_typeRef(std::move(typeRef)) {}

babelwires::ValueElementData::ValueElementData(const ValueElementData& other, ShallowCloneContext shallowCloneContext)
    : NodeData(other, shallowCloneContext)
    , m_typeRef(other.m_typeRef) {}

bool babelwires::ValueElementData::checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) {
    // For now, assume types do not need to be versioned.
    // In theory, a type might change the way that it is represented in a typeRef.
    // Not sure if that should be handled here.
    return true;
}

void babelwires::ValueElementData::serializeContents(Serializer& serializer) const {
    addCommonKeyValuePairs(serializer);
    serializer.serializeObject(m_typeRef);
    serializeModifiers(serializer);
    serializeUiData(serializer);
}

void babelwires::ValueElementData::deserializeContents(Deserializer& deserializer) {
    getCommonKeyValuePairs(deserializer);
    m_typeRef = std::move(*deserializer.deserializeObject<TypeRef>());
    deserializeModifiers(deserializer);
    deserializeUiData(deserializer);
}

std::unique_ptr<babelwires::Node>
babelwires::ValueElementData::doCreateFeatureElement(const ProjectContext& context, UserLogger& userLogger,
                                                     ElementId newId) const {
    return std::make_unique<ValueElement>(context, userLogger, *this, newId);
}

const babelwires::TypeRef& babelwires::ValueElementData::getTypeRef() const {
    return m_typeRef;
}