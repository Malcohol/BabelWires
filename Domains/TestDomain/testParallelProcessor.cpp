#include <Domains/TestDomain/testParallelProcessor.hpp>

#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

namespace {
    babelwires::TypeRef getLimitedIntType() {
        return babelwires::IntTypeConstructor::makeTypeRef(-20, 20, 0);
    }
} // namespace

testDomain::TestParallelProcessorInput::TestParallelProcessorInput()
    : babelwires::ParallelProcessorInputBase({{BW_SHORT_ID("intVal", "intVal", "914142db-873d-4c7e-a40b-0b465993538c"), getLimitedIntType()}},
                                             TestParallelProcessor::getCommonArrayId(), getLimitedIntType()) {}

testDomain::TestParallelProcessorOutput::TestParallelProcessorOutput()
    : babelwires::ParallelProcessorOutputBase(TestParallelProcessor::getCommonArrayId(), getLimitedIntType()) {}

testDomain::TestParallelProcessor::TestParallelProcessor(const babelwires::ProjectContext& context)
    : babelwires::ParallelProcessor(context, TestParallelProcessorInput::getThisType(),
                                    TestParallelProcessorOutput::getThisType()) {}

babelwires::ShortId testDomain::TestParallelProcessor::getCommonArrayId() {
    return BW_SHORT_ID("array", "array", "0eed9f2e-c22a-4b9b-a1f7-c8b02f9a86ed");
}

void testDomain::TestParallelProcessor::processEntry(babelwires::UserLogger& userLogger,
                                                     const babelwires::ValueTreeNode& input,
                                                     const babelwires::ValueTreeNode& inputEntry,
                                                     babelwires::ValueTreeNode& outputEntry) const {
    {
        // Log the input path.
        userLogger.logInfo() << babelwires::getPathTo(&inputEntry);
    }

    babelwires::ConstInstance<babelwires::IntType> entryIn{inputEntry};
    babelwires::Instance<babelwires::IntType> entryOut{outputEntry};

    const babelwires::ValueTreeNode& intValueTreeNode =
        input.getChildFromStep(babelwires::PathStep("intVal")).is<babelwires::ValueTreeNode>();

    entryOut.set(entryIn.get() + intValueTreeNode.getValue()->is<babelwires::IntValue>().get());
}
