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

#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>

babelwires::ValueNodeData::ValueNodeData(TypeExp typeExp)
    : m_typeExp(std::move(typeExp)) {}

babelwires::ValueNodeData::ValueNodeData(const ValueNodeData& other, ShallowCloneContext shallowCloneContext)
    : NodeData(other, shallowCloneContext)
    , m_typeExp(other.m_typeExp) {}

bool babelwires::ValueNodeData::checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) {
    // For now, assume types do not need to be versioned.
    // In theory, a type might change the way that it is represented in a typeExp.
    // Not sure if that should be handled here.
    return true;
}

void babelwires::ValueNodeData::serializeContents(Serializer& serializer) const {
    addCommonKeyValuePairs(serializer);
    serializer.serializeObject(m_typeExp);
    serializeModifiers(serializer);
    serializeUiData(serializer);
}

babelwires::Result babelwires::ValueNodeData::deserializeContents(Deserializer& deserializer) {
    DO_OR_ERROR(getCommonKeyValuePairs(deserializer));
    DO_OR_ERROR(deserializer.deserializeObjectByValue<TypeExp>(m_typeExp));
    DO_OR_ERROR(deserializeModifiers(deserializer));
    DO_OR_ERROR(deserializeUiData(deserializer));
    return {};
}

std::unique_ptr<babelwires::Node>
babelwires::ValueNodeData::doCreateNode(const ProjectContext& context, UserLogger& userLogger,
                                                     NodeId newId) const {
    return std::make_unique<ValueNode>(context, userLogger, *this, newId);
}

const babelwires::TypeExp& babelwires::ValueNodeData::getTypeExp() const {
    return m_typeExp;
}