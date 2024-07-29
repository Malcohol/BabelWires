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

babelwires::ValueProcessor::ValueProcessor(const ProjectContext& projectContext, const TypeRef& inputTypeRef,
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
    // TODO: Simple Value should be its own root.
    m_inputFeature->addField(std::make_unique<SimpleValueFeature>(inputTypeRef), getStepToValue());
    m_outputFeature->addField(std::make_unique<SimpleValueFeature>(outputTypeRef), getStepToValue());
}

babelwires::ShortId babelwires::ValueProcessor::getStepToValue() {
    return BW_SHORT_ID("value", "value", "490ff1e6-95bf-4913-b3c6-c682e2db189a");
}

void babelwires::ValueProcessor::process(UserLogger& userLogger) {
    const SimpleValueFeature& inputFeature = m_inputFeature->getChildFromStep(babelwires::PathStep(getStepToValue())).is<SimpleValueFeature>();
    SimpleValueFeature& outputFeature = m_outputFeature->getChildFromStep(babelwires::PathStep(getStepToValue())).is<SimpleValueFeature>();
    outputFeature.backUpValue();
    processValue(userLogger, inputFeature, outputFeature);
    outputFeature.reconcileChangesFromBackup();
}
