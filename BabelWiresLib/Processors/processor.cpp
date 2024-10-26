/**
 * A Processor defines a processing operation from an input feature to an output feature.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Processors/processor.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::Processor::Processor(const ProjectContext& projectContext, const TypeRef& inputTypeRef,
                                 const TypeRef& outputTypeRef)
    : m_inputFeature(std::make_unique<babelwires::SimpleValueFeature>(projectContext.m_typeSystem, inputTypeRef))
    , m_outputFeature(std::make_unique<babelwires::SimpleValueFeature>(projectContext.m_typeSystem, outputTypeRef)) {
    const Type* const inputType = inputTypeRef.tryResolve(projectContext.m_typeSystem);
    if (!inputType) {
        throw ModelException() << "Input type reference " << inputTypeRef << " could not be resolved";
    }
    const Type* const outputType = outputTypeRef.tryResolve(projectContext.m_typeSystem);
    if (!outputType) {
        throw ModelException() << "Output type reference " << outputTypeRef << " could not be resolved";
    }
}

babelwires::Processor::~Processor() = default;

babelwires::ValueTreeNode& babelwires::Processor::getInputFeature() {
    return *m_inputFeature;
}

babelwires::ValueTreeNode& babelwires::Processor::getOutputFeature() {
    return *m_outputFeature;
}

const babelwires::ValueTreeNode& babelwires::Processor::getInputFeature() const {
    return *m_inputFeature;
}

const babelwires::ValueTreeNode& babelwires::Processor::getOutputFeature() const {
    return *m_outputFeature;
}

void babelwires::Processor::process(UserLogger& userLogger) {
    BackupScope scope(*m_outputFeature);
    processValue(userLogger, *m_inputFeature, *m_outputFeature);
}
