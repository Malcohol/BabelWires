#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>

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
    : RecordType(
          {{getInt0Id(), babelwires::DefaultIntType::getThisIdentifier()},
           {getOpIntId(), babelwires::DefaultIntType::getThisIdentifier(), Optionality::optionalDefaultInactive},
           {getSubrecordId(), TestSimpleRecordType::getThisIdentifier()},
           {getInt1Id(),
            babelwires::TypeRef(babelwires::IntTypeConstructor::getThisIdentifier(),
                                {{}, {babelwires::IntValue(-10), babelwires::IntValue(10), babelwires::IntValue(2)}})},
           {getOpRecId(), TestSimpleRecordType::getThisIdentifier(), Optionality::optionalDefaultInactive},
           {getStringId(), babelwires::StringType::getThisIdentifier()},
           {getArrayId(), testUtils::TestSimpleArrayType::getThisIdentifier()}}) {}

babelwires::ShortId testUtils::TestComplexRecordType::getInt0Id() {
    return BW_SHORT_ID(s_intIdInitializer, s_intFieldName, "1aafde9a-fb39-4a2d-8a29-55fc9d6d093b");
}

babelwires::ShortId testUtils::TestComplexRecordType::getOpIntId() {
    return BW_SHORT_ID(s_opIntIdInitializer, s_opIntFieldName, "8d544864-ad75-4546-b523-5b4779e9120f");
}

babelwires::ShortId testUtils::TestComplexRecordType::getSubrecordId() {
    return BW_SHORT_ID(s_subRecordIdInitializer, s_subRecordFieldName, "b5855d2f-ad62-47c7-bbe1-af3d16b26d7d");
}

babelwires::ShortId testUtils::TestComplexRecordType::getInt1Id() {
    return BW_SHORT_ID(s_int1IdInitializer, s_int1FieldName, "1d21e657-a292-40c2-8fbf-491c2eaa1f8e");
}

babelwires::ShortId testUtils::TestComplexRecordType::getOpRecId() {
    return BW_SHORT_ID(s_opRecIdInitializer, s_opRecFieldName, "e0301eeb-1a6b-461a-aba4-ed3b85b88ebe");
}

babelwires::ShortId testUtils::TestComplexRecordType::getStringId() {
    return BW_SHORT_ID(s_stringIdInitializer, s_stringFieldName, "0f5c8454-89ed-4b66-a3f1-0174a620234c");
}

babelwires::ShortId testUtils::TestComplexRecordType::getArrayId() {
    return BW_SHORT_ID(s_arrayIdInitializer, s_arrayFieldName, "5da653f4-44d5-4030-a956-771fc06fb769");
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

babelwires::FeaturePath testUtils::TestComplexRecordElementData::getPathToRecordSubrecordInt1() {
    babelwires::FeaturePath path = getPathToRecord();
    path.pushStep(babelwires::PathStep(TestComplexRecordType::getSubrecordId()));
    path.pushStep(babelwires::PathStep(TestSimpleRecordType::getInt1Id()));
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

testUtils::TestComplexRecordTypeFeatureInfo::TestComplexRecordTypeFeatureInfo(
    const babelwires::ValueFeature& testRecord)
    : m_record(testRecord)
    , m_intFeature(testRecord.getChildFromStep(babelwires::PathStep(testUtils::TestComplexRecordType::getInt0Id()))
                       .is<babelwires::ValueFeature>())
    , m_arrayFeature(testRecord.getChildFromStep(babelwires::PathStep(testUtils::TestComplexRecordType::getArrayId()))
                         .is<babelwires::ValueFeature>())
    , m_elem0(m_arrayFeature.getChildFromStep(babelwires::PathStep(0)).is<babelwires::ValueFeature>())
    , m_elem1(m_arrayFeature.getChildFromStep(babelwires::PathStep(1)).is<babelwires::ValueFeature>())
    , m_subRecordFeature(
          testRecord.getChildFromStep(babelwires::PathStep(testUtils::TestComplexRecordType::getSubrecordId()))
              .is<babelwires::ValueFeature>())
    , m_subRecordIntFeature(
          m_subRecordFeature.getChildFromStep(babelwires::PathStep(testUtils::TestSimpleRecordType::getInt0Id()))
              .is<babelwires::ValueFeature>())
    , m_pathToRecord(&testRecord)
    , m_pathToInt(&m_intFeature)
    , m_pathToArray(&m_arrayFeature)
    , m_pathToElem0(&m_elem0)
    , m_pathToElem1(&m_elem1)
    , m_pathToSubRecord(&m_subRecordFeature)
    , m_pathToSubRecordInt(&m_subRecordIntFeature) {}