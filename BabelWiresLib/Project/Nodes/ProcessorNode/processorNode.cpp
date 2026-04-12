/**
 * ProcessorNode are Nodes which carry a processor.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNode.hpp>

#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Processors/processor.hpp>
#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Processors/processorFactoryRegistry.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <BaseLib/Context/context.hpp>
#include <BaseLib/Log/userLogger.hpp>

babelwires::ProcessorNode::ProcessorNode(const Context& context, UserLogger& userLogger,
                                               const ProcessorNodeData& data, NodeId newId)
    : Node(data, newId) {
    const NodeData& elementData = getNodeData();
    const auto factoryResult = context.getService<ProcessorFactoryRegistry>().getRegisteredEntry(elementData.m_factoryIdentifier);
    if (!factoryResult) {
        setFactoryName(elementData.m_factoryIdentifier);
        setInternalFailure(factoryResult.error().toString());
        m_failedValueTree =
            std::make_unique<babelwires::ValueTreeRoot>(context.getService<TypeSystem>(), context.getService<TypeSystem>().getRegisteredType<FailureType>());
        userLogger.logError() << "Failed to create processor id=" << elementData.m_id
                                << ": " << factoryResult.error().toString();
        return;
    }
    const ProcessorFactory& factory = **factoryResult;
    auto newProcessor = factory.createNewProcessor(context);
    newProcessor->getInput().setToDefault();
    newProcessor->getOutput().setToDefault();
    setProcessor(std::move(newProcessor));
    setFactoryName(factory.getName());
}

babelwires::ProcessorNode::~ProcessorNode() = default;

const babelwires::ProcessorNodeData& babelwires::ProcessorNode::getNodeData() const {
    return static_cast<const ProcessorNodeData&>(Node::getNodeData());
}

babelwires::ValueTreeNode* babelwires::ProcessorNode::doGetOutputNonConst() {
    if (m_processor) {
        return &m_processor->getOutput();
    } else {
        return m_failedValueTree.get();
    }
}

babelwires::ValueTreeNode* babelwires::ProcessorNode::doGetInputNonConst() {
    if (m_processor) {
        return &m_processor->getInput();
    } else {
        return m_failedValueTree.get();
    }
}

const babelwires::ValueTreeNode* babelwires::ProcessorNode::getOutput() const {
    if (m_processor) {
        return &m_processor->getOutput();
    } else {
        return m_failedValueTree.get();
    }
}

const babelwires::ValueTreeNode* babelwires::ProcessorNode::getInput() const {
    if (m_processor) {
        return &m_processor->getInput();
    } else {
        return m_failedValueTree.get();
    }
}

void babelwires::ProcessorNode::setProcessor(std::unique_ptr<Processor> processor) {
    m_processor = std::move(processor);
    setValueTrees(getRootLabel(), &m_processor->getInput(), &m_processor->getOutput());
}

std::string babelwires::ProcessorNode::getRootLabel() const {
    if (m_processor) {
        return "Input/Output";
    } else {
        return "Failed";
    }
}

void babelwires::ProcessorNode::doProcess(UserLogger& userLogger) {
    if (m_processor) {
        if (getInput()->isChanged(ValueTreeNode::Changes::SomethingChanged)) {
            Result result = m_processor->process(userLogger);
            if (result) {
                if (isFailed()) {
                    clearInternalFailure();
                }
            } else {
                userLogger.logError() << "Processor id=" << getNodeId()
                                      << " failed to process correctly: " << result.error().toString();
                setInternalFailure(result.error().toString());
            }
        }
        if (isChanged(Changes::FeatureStructureChanged | Changes::CompoundExpandedOrCollapsed)) {
            setValueTrees(getRootLabel(), &m_processor->getInput(),
                                        &m_processor->getOutput());
        } else if (isChanged(Changes::ModifierChangesMask)) {
            updateModifierCache();
        }
    } else {
        setValueTrees(getRootLabel(), m_failedValueTree.get(), m_failedValueTree.get());
    }
}
