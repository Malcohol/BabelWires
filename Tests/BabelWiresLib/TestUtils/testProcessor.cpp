#include <Tests/BabelWiresLib/TestUtils/testProcessor.hpp>

#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>
#include <BabelWiresLib/ValueTree/Utilities/modelUtilities.hpp>

babelwires::ShortId testUtils::TestProcessorInputOutputType::getIntId() {
    return BW_SHORT_ID(s_intIdInitializer, s_intFieldName, s_intUuid);
}

babelwires::ShortId testUtils::TestProcessorInputOutputType::getOptIntId() {
    return BW_SHORT_ID(s_optIntIdInitializer, s_optIntFieldName, s_optIntUuid);
}

babelwires::ShortId testUtils::TestProcessorInputOutputType::getArrayId() {
    return BW_SHORT_ID(s_arrayIdInitializer, s_arrayFieldName, s_arrayUuid);
}

babelwires::ShortId testUtils::TestProcessorInputOutputType::getRecordId() {
    return BW_SHORT_ID(s_recordIdInitializer, s_recordFieldName, s_recordUuid);
}

testUtils::TestProcessorInputOutputType::TestProcessorInputOutputType()
    : RecordType({{getIntId(), babelwires::DefaultIntType::getThisType()},
                  {getOptIntId(), babelwires::DefaultIntType::getThisType(), Optionality::optionalDefaultInactive},
                  {getArrayId(),
                   babelwires::ArrayTypeConstructor::makeTypeRef(babelwires::DefaultIntType::getThisType(), 2, 8)},
                  {getRecordId(), TestSimpleRecordType::getThisType()}}) {}

const babelwires::Path testUtils::TestProcessorInputOutputType::s_pathToInt =
    babelwires::Path::deserializeFromString("Int");
const babelwires::Path testUtils::TestProcessorInputOutputType::s_pathToArray =
    babelwires::Path::deserializeFromString("Array");
const babelwires::Path testUtils::TestProcessorInputOutputType::s_pathToArray_0 =
    babelwires::Path::deserializeFromString("Array/0");
const babelwires::Path testUtils::TestProcessorInputOutputType::s_pathToArray_1 =
    babelwires::Path::deserializeFromString("Array/1");
const babelwires::Path testUtils::TestProcessorInputOutputType::s_pathToArray_2 =
    babelwires::Path::deserializeFromString("Array/2");
const babelwires::Path testUtils::TestProcessorInputOutputType::s_pathToArray_3 =
    babelwires::Path::deserializeFromString("Array/3");
const babelwires::Path testUtils::TestProcessorInputOutputType::s_pathToArray_4 =
    babelwires::Path::deserializeFromString("Array/4");
const babelwires::Path testUtils::TestProcessorInputOutputType::s_pathToRecord =
    babelwires::Path::deserializeFromString("Record");
const babelwires::Path testUtils::TestProcessorInputOutputType::s_pathToInt2 =
    babelwires::Path::deserializeFromString("Record/intR0");

testUtils::TestProcessor::TestProcessor(const babelwires::ProjectContext& context)
    : babelwires::Processor(context, testUtils::TestProcessorInputOutputType::getThisType(),
                            testUtils::TestProcessorInputOutputType::getThisType()) {}

void testUtils::TestProcessor::processValue(babelwires::UserLogger& userLogger, const babelwires::ValueTreeNode& input,
                                            babelwires::ValueTreeNode& output) const {
    TestProcessorInputOutputType::ConstInstance in{input};
    TestProcessorInputOutputType::Instance out{output};

    const unsigned int inputValue = in.getInt().get();

    out.getInt().set(inputValue);
    out.getRecord()->setValue(in.getRecord()->getValue());
    if (in.tryGetOpInt()) {
        out.activateAndGetOpInt().set(in.tryGetOpInt()->get());
    } else {
        out.deactivateOpInt();
    }
    out.getArray().setSize(2 + inputValue);
    for (int i = 0; i < out.getArray().getSize(); ++i) {
        out.getArray().getEntry(i).set(in.getInt().get() + i);
    }
}
