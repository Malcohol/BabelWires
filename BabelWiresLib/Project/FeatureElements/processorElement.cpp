/**
 * ProcessorElement are FeatureElements which carry a processor.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Project/FeatureElements/processorElement.hpp"

#include "BabelWiresLib/Features/modelExceptions.hpp"
#include "BabelWiresLib/Processors/processor.hpp"
#include "BabelWiresLib/Processors/processorFactory.hpp"
#include "BabelWiresLib/Processors/processorFactoryRegistry.hpp"
#include "BabelWiresLib/Project/FeatureElements/failedFeature.hpp"
#include "BabelWiresLib/Project/FeatureElements/processorElementData.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"
#include "BabelWiresLib/Project/projectContext.hpp"

#include "Common/Log/userLogger.hpp"

babelwires::ProcessorElement::ProcessorElement(const ProjectContext& context, UserLogger& userLogger,
                                               const ProcessorElementData& data, ElementId newId)
    : FeatureElement(data, newId) {
    const ElementData& elementData = getElementData();
    try {
        const ProcessorFactory& factory = context.m_processorReg.getRegisteredEntry(elementData.m_factoryIdentifier);
        setProcessor(factory.createNewProcessor());
        setFactoryName(factory.getName());
    } catch (const BaseException& e) {
        setFactoryName(elementData.m_factoryIdentifier);
        setInternalFailure(e.what());
        m_sharedDummyFeature = std::make_unique<babelwires::FailedFeature>();
        userLogger.logError() << "Failed to create processor id=" << elementData.m_id << ": " << e.what();
    }
}

const babelwires::ProcessorElementData& babelwires::ProcessorElement::getElementData() const {
    return static_cast<const ProcessorElementData&>(FeatureElement::getElementData());
}

babelwires::RecordFeature* babelwires::ProcessorElement::getOutputFeature() {
    if (m_processor) {
        return m_processor->getOutputFeature();
    } else {
        return m_sharedDummyFeature.get();
    }
}

babelwires::RecordFeature* babelwires::ProcessorElement::getInputFeature() {
    if (m_processor) {
        return m_processor->getInputFeature();
    } else {
        return m_sharedDummyFeature.get();
    }
}

void babelwires::ProcessorElement::setProcessor(std::unique_ptr<Processor> processor) {
    m_contentsCache.setFeatures(processor->getInputFeature(), processor->getOutputFeature());
    m_processor = std::move(processor);
}

void babelwires::ProcessorElement::doProcess(UserLogger& userLogger) {
    if (getInputFeature()->isChanged(Feature::Changes::SomethingChanged)) {
        if (m_processor) {
            try {
                m_processor->process(userLogger);
                if (isFailed()) {
                    clearInternalFailure();
                }
            } catch (const BaseException& e) {
                userLogger.logError() << "Processor id=" << getElementId()
                                      << " failed to process correctly: " << e.what();
                setInternalFailure(e.what());
                // TODO: Is this definitely the desired outcome?
                m_processor->getOutputFeature()->setToDefault();
            }
        }
    }
    if (isChanged(Changes::FeatureStructureChanged | Changes::CompoundExpandedOrCollapsed)) {
        if (m_processor) {
            m_contentsCache.setFeatures(m_processor->getInputFeature(), m_processor->getOutputFeature());
        }
    } else if (isChanged(Changes::ModifierChangesMask)) {
        m_contentsCache.updateModifierCache();
    }
}
