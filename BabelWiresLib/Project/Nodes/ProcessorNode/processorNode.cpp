/**
 * ProcessorNode are Nodes which carry a processor.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNode.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Processors/processor.hpp>
#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Processors/processorFactoryRegistry.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>

#include <Common/Log/userLogger.hpp>

babelwires::ProcessorNode::ProcessorNode(const ProjectContext& context, UserLogger& userLogger,
                                               const ProcessorNodeData& data, NodeId newId)
    : Node(data, newId) {
    const NodeData& elementData = getNodeData();
    try {
        const ProcessorFactory& factory = context.m_processorReg.getRegisteredEntry(elementData.m_factoryIdentifier);
        auto newProcessor = factory.createNewProcessor(context);
        newProcessor->getInput().setToDefault();
        newProcessor->getOutput().setToDefault();
        setProcessor(std::move(newProcessor));
        setFactoryName(factory.getName());
    } catch (const BaseException& e) {
        setFactoryName(elementData.m_factoryIdentifier);
        setInternalFailure(e.what());
        m_failedValueTree =
            std::make_unique<babelwires::ValueTreeRoot>(context.m_typeSystem, FailureType::getThisType());
        userLogger.logError() << "Failed to create processor id=" << elementData.m_id << ": " << e.what();
    }
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
            try {
                m_processor->process(userLogger);
                if (isFailed()) {
                    clearInternalFailure();
                }
            } catch (const BaseException& e) {
                userLogger.logError() << "Processor id=" << getNodeId()
                                      << " failed to process correctly: " << e.what();
                setInternalFailure(e.what());
                // TODO: Is this definitely the desired outcome?
                m_processor->getOutput().setToDefault();
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
