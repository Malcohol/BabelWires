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

babelwires::Processor::Processor(const ProjectContext& projectContext, TypePtr inputType, TypePtr outputType)
    : m_inputValueTreeRoot(
          std::make_unique<babelwires::ValueTreeRoot>(projectContext.m_typeSystem, std::move(inputType)))
    , m_outputValueTreeRoot(
          std::make_unique<babelwires::ValueTreeRoot>(projectContext.m_typeSystem, std::move(outputType))) {}

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
