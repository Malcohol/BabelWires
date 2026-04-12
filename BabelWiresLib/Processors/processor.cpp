/**
 * A Processor defines an operation from an input ValueTree to an output ValueTree.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Processors/processor.hpp>

#include <BabelWiresLib/TypeSystem/typeExp.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <BaseLib/Context/context.hpp>
#include <BaseLib/Identifiers/registeredIdentifier.hpp>

babelwires::Processor::Processor(const Context& context, TypePtr inputType, TypePtr outputType)
    : m_inputValueTreeRoot(
          std::make_unique<babelwires::ValueTreeRoot>(context.getService<TypeSystem>(), std::move(inputType)))
    , m_outputValueTreeRoot(
          std::make_unique<babelwires::ValueTreeRoot>(context.getService<TypeSystem>(), std::move(outputType))) {}

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

babelwires::Result babelwires::Processor::process(UserLogger& userLogger) {
    Result result = processValue(userLogger, *m_inputValueTreeRoot, *m_outputValueTreeRoot);
    if (!result) {
        onFailure();
    }
    return result;
}

void babelwires::Processor::onFailure() const {
    m_outputValueTreeRoot->setToDefault();
}
