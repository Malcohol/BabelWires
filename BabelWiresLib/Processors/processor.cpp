/**
 * A Processor defines an operation from an input ValueTree to an output ValueTree.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Processors/processor.hpp>

#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeExp.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <BaseLib/Identifiers/registeredIdentifier.hpp>

babelwires::Processor::Processor(const ProjectContext& projectContext, const TypeExp& inputTypeExp,
                                 const TypeExp& outputTypeExp)
    : m_inputValueTreeRoot(std::make_unique<babelwires::ValueTreeRoot>(projectContext.m_typeSystem, inputTypeExp))
    , m_outputValueTreeRoot(std::make_unique<babelwires::ValueTreeRoot>(projectContext.m_typeSystem, outputTypeExp)) {
    const TypePtr inputType = inputTypeExp.tryResolve(projectContext.m_typeSystem);
    if (!inputType) {
        throw ModelException() << "Input type reference " << inputTypeExp << " could not be resolved";
    }
    const TypePtr outputType = outputTypeExp.tryResolve(projectContext.m_typeSystem);
    if (!outputType) {
        throw ModelException() << "Output type reference " << outputTypeExp << " could not be resolved";
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
    try {
        processValue(userLogger, *m_inputValueTreeRoot, *m_outputValueTreeRoot);
    } catch (...) {
        onFailure();
        throw;
    }
}

void babelwires::Processor::onFailure() const {
    m_outputValueTreeRoot->setToDefault();
}
