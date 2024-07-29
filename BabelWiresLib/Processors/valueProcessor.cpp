/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Processors/valueProcessor.hpp>

#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::ValueProcessorBase::ValueProcessorBase(const ProjectContext& projectContext, const TypeRef& inputTypeRef,
                                           const TypeRef& outputTypeRef)
    : CommonProcessor(projectContext) {
    const Type* const inputType = inputTypeRef.tryResolve(projectContext.m_typeSystem);
    if (!inputType) {
        throw ModelException() << "Input type reference " << inputTypeRef << " could not be resolved";
    }
    const Type* const outputType = outputTypeRef.tryResolve(projectContext.m_typeSystem);
    if (!outputType) {
        throw ModelException() << "Output type reference " << inputTypeRef << " could not be resolved";
    }
    m_inputFeature->addField(std::make_unique<SimpleValueFeature>(inputTypeRef), getStepToValue());
    m_outputFeature->addField(std::make_unique<SimpleValueFeature>(outputTypeRef), getStepToValue());
}

babelwires::ShortId babelwires::ValueProcessorBase::getStepToValue() {
    return BW_SHORT_ID("value", "value", "490ff1e6-95bf-4913-b3c6-c682e2db189a");
}


const babelwires::SimpleValueFeature& babelwires::ValueProcessorBase::getInputValueFeature() const {
    return m_inputFeature->getChildFromStep(babelwires::PathStep(getStepToValue())).is<SimpleValueFeature>();
}

babelwires::SimpleValueFeature& babelwires::ValueProcessorBase::getOutputValueFeature() {
    return m_outputFeature->getChildFromStep(babelwires::PathStep(getStepToValue())).is<SimpleValueFeature>();
}

/*
babelwires::SimpleValueProcessor::SimpleValueProcessor(const ProjectContext& projectContext, const TypeRef& inputTypeRef,
                           const TypeRef& outputTypeRef) : ValueProcessorBase(projectContext, inputTypeRef, outputTypeRef) {}

void babelwires::SimpleValueProcessor::process(UserLogger& userLogger) {
    const ValueFeature& inputFeature = getInputValueFeature();
    ValueFeature& outputFeature = getOutputValueFeature();
    processValue(userLogger, inputFeature.getType(), inputFeature.getValue(), outputFeature.getType(), outputFeature.getValue());
}
*/