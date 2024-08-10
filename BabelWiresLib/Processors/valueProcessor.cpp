/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Processors/valueProcessor.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::ValueProcessor::ValueProcessor(const ProjectContext& projectContext, const TypeRef& inputTypeRef,
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

babelwires::Feature* babelwires::ValueProcessor::getInputFeature() {
    return m_inputFeature.get();
}

babelwires::Feature* babelwires::ValueProcessor::getOutputFeature() {
    return m_outputFeature.get();
}

void babelwires::ValueProcessor::process(UserLogger& userLogger) {
    m_outputFeature->backUpValue();
    try {
        processValue(userLogger, *m_inputFeature, *m_outputFeature);
        m_outputFeature->reconcileChangesFromBackup();
    } catch(std::exception&) {
        // TODO Maybe use a backup scope.
        m_outputFeature->reconcileChangesFromBackup();
        throw;
    }
}
