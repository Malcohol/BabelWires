/**
 * ValueNodeData describes the construction of a ValueNode.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>

#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::ValueNodeData::ValueNodeData(TypeRef typeRef)
    : m_typeRef(std::move(typeRef)) {}

babelwires::ValueNodeData::ValueNodeData(const ValueNodeData& other, ShallowCloneContext shallowCloneContext)
    : NodeData(other, shallowCloneContext)
    , m_typeRef(other.m_typeRef) {}

bool babelwires::ValueNodeData::checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) {
    // For now, assume types do not need to be versioned.
    // In theory, a type might change the way that it is represented in a typeRef.
    // Not sure if that should be handled here.
    return true;
}

void babelwires::ValueNodeData::serializeContents(Serializer& serializer) const {
    addCommonKeyValuePairs(serializer);
    serializer.serializeObject(m_typeRef);
    serializeModifiers(serializer);
    serializeUiData(serializer);
}

void babelwires::ValueNodeData::deserializeContents(Deserializer& deserializer) {
    getCommonKeyValuePairs(deserializer);
    m_typeRef = std::move(*deserializer.deserializeObject<TypeRef>());
    deserializeModifiers(deserializer);
    deserializeUiData(deserializer);
}

std::unique_ptr<babelwires::Node>
babelwires::ValueNodeData::doCreateNode(const ProjectContext& context, UserLogger& userLogger,
                                                     ElementId newId) const {
    return std::make_unique<ValueNode>(context, userLogger, *this, newId);
}

const babelwires::TypeRef& babelwires::ValueNodeData::getTypeRef() const {
    return m_typeRef;
}