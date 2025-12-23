/**
 * ValueNodes are Nodes which carry a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>

#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>

babelwires::ValueNode::ValueNode(const ProjectContext& context, UserLogger& userLogger,
                                       const ValueNodeData& data, NodeId newId)
    : Node(data, newId) {
    setFactoryName(data.getTypeRef().toString());
    TypeRef typeRefForConstruction = data.getTypeRef();
    if (!typeRefForConstruction.tryResolve(context.m_typeSystem)) {
        typeRefForConstruction = DefaultFailureType::getThisType();
        std::ostringstream message;
        message << "Type Reference " << data.getTypeRef().toString() << " could not be resolved";
        setInternalFailure(message.str());
    }
    m_valueTreeRoot = std::make_unique<ValueTreeRoot>(context.m_typeSystem, typeRefForConstruction);
}

babelwires::ValueNode::~ValueNode() = default;

const babelwires::ValueNodeData& babelwires::ValueNode::getNodeData() const {
    return static_cast<const ValueNodeData&>(Node::getNodeData());
}

babelwires::ValueTreeNode* babelwires::ValueNode::doGetInputNonConst() {
    return m_valueTreeRoot.get();
}

babelwires::ValueTreeNode* babelwires::ValueNode::doGetOutputNonConst() {
    return m_valueTreeRoot.get();
}

const babelwires::ValueTreeNode* babelwires::ValueNode::getInput() const {
    return m_valueTreeRoot.get();
}

const babelwires::ValueTreeNode* babelwires::ValueNode::getOutput() const {
    return m_valueTreeRoot.get();
}

std::string babelwires::ValueNode::getRootLabel() const {
    if (m_valueTreeRoot->getTypeRef() == DefaultFailureType::getThisType()) {
        return "Failed";
    } else {
        return "Value";
    }
}

void babelwires::ValueNode::doProcess(UserLogger& userLogger) {
    if (isChanged(Changes::FeatureStructureChanged | Changes::CompoundExpandedOrCollapsed)) {
        setValueTrees(getRootLabel(), m_valueTreeRoot.get(), m_valueTreeRoot.get());
    } else if (isChanged(Changes::ModifierChangesMask)) {
        updateModifierCache();
    }
}
