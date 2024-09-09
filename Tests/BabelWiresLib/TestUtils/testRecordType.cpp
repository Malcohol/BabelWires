#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>

testUtils::TestSimpleRecordType::TestSimpleRecordType()
    : RecordType({{getInt0Id(), babelwires::DefaultIntType::getThisIdentifier()},
                  {getInt1Id(), babelwires::DefaultIntType::getThisIdentifier()}}) {}

babelwires::ShortId testUtils::TestSimpleRecordType::getInt0Id() {
    return BW_SHORT_ID(s_int0IdInitializer, s_int0FieldName, s_int0Uuid);
}

babelwires::ShortId testUtils::TestSimpleRecordType::getInt1Id() {
    return BW_SHORT_ID(s_int1IdInitializer, s_int1FieldName, s_int1Uuid);
}

testUtils::TestComplexRecordType::TestComplexRecordType()
    : RecordType({{getInt0Id(), babelwires::DefaultIntType::getThisIdentifier()},
                  {getOpIntId(), babelwires::DefaultIntType::getThisIdentifier(), Optionality::optionalDefaultInactive},
                  {getSubrecordId(), TestSimpleRecordType::getThisIdentifier()},
                  {getInt1Id(), babelwires::DefaultIntType::getThisIdentifier()},
                  {getOpRecId(), TestSimpleRecordType::getThisIdentifier(), Optionality::optionalDefaultInactive},
                  {getStringId(), babelwires::StringType::getThisIdentifier()}}) {}

babelwires::ShortId testUtils::TestComplexRecordType::getInt0Id() {
    return BW_SHORT_ID("intR0", "Int0", "1aafde9a-fb39-4a2d-8a29-55fc9d6d093b");
}

babelwires::ShortId testUtils::TestComplexRecordType::getOpIntId() {
    return BW_SHORT_ID("opInt", "OptionalInt", "8d544864-ad75-4546-b523-5b4779e9120f");
}

babelwires::ShortId testUtils::TestComplexRecordType::getSubrecordId() {
    return BW_SHORT_ID("rec", "Subrecord", "b5855d2f-ad62-47c7-bbe1-af3d16b26d7d");
}

babelwires::ShortId testUtils::TestComplexRecordType::getInt1Id() {
    return BW_SHORT_ID("intR1", "Int1", "1d21e657-a292-40c2-8fbf-491c2eaa1f8e");
}

babelwires::ShortId testUtils::TestComplexRecordType::getOpRecId() {
    return BW_SHORT_ID("opRec", "OptionalRecord", "e0301eeb-1a6b-461a-aba4-ed3b85b88ebe");
}

babelwires::ShortId testUtils::TestComplexRecordType::getStringId() {
    return BW_SHORT_ID("string", "String", "0f5c8454-89ed-4b66-a3f1-0174a620234c");
}

testUtils::TestRecordElementData::TestRecordElementData()
    : babelwires::ValueElementData(TestSimpleRecordType::getThisIdentifier()) {}

babelwires::FeaturePath testUtils::TestRecordElementData::getPathToRecord() {
    babelwires::FeaturePath pathToArray;
    pathToArray.pushStep(babelwires::PathStep(babelwires::ValueElement::getStepToValue()));
    return pathToArray;
}

babelwires::FeaturePath testUtils::TestRecordElementData::getPathToRecordInt0() {
    babelwires::FeaturePath pathToArray_1 = getPathToRecord();
    pathToArray_1.pushStep(babelwires::PathStep(TestSimpleRecordType::getInt0Id()));
    return pathToArray_1;
}
