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
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>

babelwires::ValueNode::ValueNode(const ProjectContext& context, UserLogger& userLogger,
                                       const ValueNodeData& data, NodeId newId)
    : Node(data, newId) {
    setFactoryName(data.getTypeExp().toString());
    TypePtr nodeType;
    try {
        nodeType = data.getTypeExp().resolve(context.m_typeSystem);
    } catch (const TypeSystemException& e) {
        std::ostringstream message;
        message << "Type Reference " << data.getTypeExp().toString() << " could not be resolved: " << e.what();
        setInternalFailure(message.str());
        nodeType = context.m_typeSystem.getRegisteredType<FailureType>();
    }
    m_valueTreeRoot = std::make_unique<ValueTreeRoot>(context.m_typeSystem, std::move(nodeType));
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
    if (m_valueTreeRoot->getType()->getTypeExp() == FailureType::getThisIdentifier()) {
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
