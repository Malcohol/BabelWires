#include <Tests/BabelWiresLib/TestUtils/testProcessor.hpp>

#include <BabelWiresLib/Features/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>

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
    : RecordType(
          {{getIntId(), babelwires::DefaultIntType::getThisIdentifier()},
           {getOptIntId(), babelwires::DefaultIntType::getThisIdentifier(), Optionality::optionalDefaultInactive},
           {getArrayId(),
            babelwires::TypeRef{babelwires::ArrayTypeConstructor::getThisIdentifier(),
                                {{babelwires::DefaultIntType::getThisIdentifier()},
                                 {babelwires::IntValue(2), babelwires::IntValue(8), babelwires::IntValue(2)}}}},
           {getRecordId(), TestSimpleRecordType::getThisIdentifier()}}) {}

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
    : babelwires::Processor(context, testUtils::TestProcessorInputOutputType::getThisIdentifier(),
                                 testUtils::TestProcessorInputOutputType::getThisIdentifier()) {}

void testUtils::TestProcessor::processValue(babelwires::UserLogger& userLogger,
                                             const babelwires::Feature& inputFeature,
                                             babelwires::Feature& outputFeature) const {
    TestProcessorInputOutputType::ConstInstance input{inputFeature};
    TestProcessorInputOutputType::Instance output{outputFeature};

    const unsigned int inputValue = input.getInt().get();

    output.getInt().set(inputValue);
    output.getRecord()->setValue(input.getRecord()->getValue());
    if (input.tryGetOpInt()) {
        output.activateAndGetOpInt().set(input.tryGetOpInt()->get());
    } else {
        output.deactivateOpInt();
    }
    output.getArray().setSize(2 + inputValue);
    for (int i = 0; i < output.getArray().getSize(); ++i) {
        output.getArray().getEntry(i).set(input.getInt().get() + i);
    }
}
