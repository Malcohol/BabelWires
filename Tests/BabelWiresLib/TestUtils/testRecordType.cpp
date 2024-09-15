#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

#include <BabelWiresLib/Types/Int/intType.hpp>

#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>

#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>

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
                  {getStringId(), babelwires::StringType::getThisIdentifier()},
                  {getArrayId(), testUtils::TestSimpleArrayType::getThisIdentifier()}}) {}

babelwires::ShortId testUtils::TestComplexRecordType::getInt0Id() {
    return BW_SHORT_ID(s_intIdInitializer, "Int0", "1aafde9a-fb39-4a2d-8a29-55fc9d6d093b");
}

babelwires::ShortId testUtils::TestComplexRecordType::getOpIntId() {
    return BW_SHORT_ID(s_opIntIdInitializer, "OptionalInt", "8d544864-ad75-4546-b523-5b4779e9120f");
}

babelwires::ShortId testUtils::TestComplexRecordType::getSubrecordId() {
    return BW_SHORT_ID(s_subRecordIdInitializer, "Subrecord", "b5855d2f-ad62-47c7-bbe1-af3d16b26d7d");
}

babelwires::ShortId testUtils::TestComplexRecordType::getInt1Id() {
    return BW_SHORT_ID(s_int1IdInitializer, "Int1", "1d21e657-a292-40c2-8fbf-491c2eaa1f8e");
}

babelwires::ShortId testUtils::TestComplexRecordType::getOpRecId() {
    return BW_SHORT_ID(s_opRecIdInitializer, "OptionalRecord", "e0301eeb-1a6b-461a-aba4-ed3b85b88ebe");
}

babelwires::ShortId testUtils::TestComplexRecordType::getStringId() {
    return BW_SHORT_ID(s_stringIdInitializer, "String", "0f5c8454-89ed-4b66-a3f1-0174a620234c");
}

babelwires::ShortId testUtils::TestComplexRecordType::getArrayId() {
    return BW_SHORT_ID(s_arrayIdInitializer, "Array", "5da653f4-44d5-4030-a956-771fc06fb769");
}

testUtils::TestSimpleRecordElementData::TestSimpleRecordElementData()
    : babelwires::ValueElementData(TestSimpleRecordType::getThisIdentifier()) {}

babelwires::FeaturePath testUtils::TestSimpleRecordElementData::getPathToRecord() {
    babelwires::FeaturePath path;
    path.pushStep(babelwires::PathStep(babelwires::ValueElement::getStepToValue()));
    return path;
}

babelwires::FeaturePath testUtils::TestSimpleRecordElementData::getPathToRecordInt0() {
    babelwires::FeaturePath path = getPathToRecord();
    path.pushStep(babelwires::PathStep(TestSimpleRecordType::getInt0Id()));
    return path;
}

testUtils::TestComplexRecordElementData::TestComplexRecordElementData()
    : babelwires::ValueElementData(TestComplexRecordType::getThisIdentifier()) {}

babelwires::FeaturePath testUtils::TestComplexRecordElementData::getPathToRecord() {
    babelwires::FeaturePath path;
    path.pushStep(babelwires::PathStep(babelwires::ValueElement::getStepToValue()));
    return path;
}

babelwires::FeaturePath testUtils::TestComplexRecordElementData::getPathToRecordInt0() {
    babelwires::FeaturePath path = getPathToRecord();
    path.pushStep(babelwires::PathStep(TestComplexRecordType::getInt0Id()));
    return path;
}

babelwires::FeaturePath testUtils::TestComplexRecordElementData::getPathToRecordOpInt() {
    babelwires::FeaturePath path = getPathToRecord();
    path.pushStep(babelwires::PathStep(TestComplexRecordType::getOpIntId()));
    return path;
}

babelwires::FeaturePath testUtils::TestComplexRecordElementData::getPathToRecordSubrecord() {
    babelwires::FeaturePath path = getPathToRecord();
    path.pushStep(babelwires::PathStep(TestComplexRecordType::getSubrecordId()));
    return path;
}

babelwires::FeaturePath testUtils::TestComplexRecordElementData::getPathToRecordInt1() {
    babelwires::FeaturePath path = getPathToRecord();
    path.pushStep(babelwires::PathStep(TestComplexRecordType::getInt1Id()));
    return path;
}

babelwires::FeaturePath testUtils::TestComplexRecordElementData::getPathToRecordOpRec() {
    babelwires::FeaturePath path = getPathToRecord();
    path.pushStep(babelwires::PathStep(TestComplexRecordType::getOpRecId()));
    return path;
}

babelwires::FeaturePath testUtils::TestComplexRecordElementData::getPathToRecordString() {
    babelwires::FeaturePath path = getPathToRecord();
    path.pushStep(babelwires::PathStep(TestComplexRecordType::getStringId()));
    return path;
}


babelwires::FeaturePath testUtils::TestComplexRecordElementData::getPathToRecordArray() {
    babelwires::FeaturePath path = getPathToRecord();
    path.pushStep(babelwires::PathStep(TestComplexRecordType::getArrayId()));
    return path;
}

babelwires::FeaturePath testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(unsigned int i) {
    babelwires::FeaturePath path = getPathToRecordArray();
    path.pushStep(babelwires::PathStep(i));
    return path;
}
