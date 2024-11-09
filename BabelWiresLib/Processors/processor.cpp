/**
 * A Processor defines an operation from an input ValueTree to an output ValueTree.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Processors/processor.hpp>

#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::Processor::Processor(const ProjectContext& projectContext, const TypeRef& inputTypeRef,
                                 const TypeRef& outputTypeRef)
    : m_inputValueTreeRoot(std::make_unique<babelwires::ValueTreeRoot>(projectContext.m_typeSystem, inputTypeRef))
    , m_outputValueTreeRoot(std::make_unique<babelwires::ValueTreeRoot>(projectContext.m_typeSystem, outputTypeRef)) {
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

babelwires::ValueTreeRoot& babelwires::Processor::getInput() {
    return *m_inputValueTreeRoot;
}

babelwires::ValueTreeRoot& babelwires::Processor::getOutput() {
    return *m_outputValueTreeRoot;
}

const babelwires::ValueTreeRoot& babelwires::Processor::getInput() const {
    return *m_inputValueTreeRoot;
}

const babelwires::ValueTreeRoot& babelwires::Processor::getOutput() const {
    return *m_outputValueTreeRoot;
}

void babelwires::Processor::process(UserLogger& userLogger) {
    BackupScope scope(*m_outputValueTreeRoot);
    processValue(userLogger, *m_inputValueTreeRoot, *m_outputValueTreeRoot);
}
