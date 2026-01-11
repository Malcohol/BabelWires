#include <Domains/TestDomain/testRecordType.hpp>

#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <Domains/TestDomain/testArrayType.hpp>

testDomain::TestSimpleRecordType::TestSimpleRecordType(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), typeSystem, {{getInt0Id(), babelwires::DefaultIntType::getThisIdentifier()},
                  {getInt1Id(), babelwires::DefaultIntType::getThisIdentifier()}}) {}

babelwires::ShortId testDomain::TestSimpleRecordType::getInt0Id() {
    return BW_SHORT_ID(s_int0IdInitializer, s_int0FieldName, s_int0Uuid);
}

babelwires::ShortId testDomain::TestSimpleRecordType::getInt1Id() {
    return BW_SHORT_ID(s_int1IdInitializer, s_int1FieldName, s_int1Uuid);
}

testDomain::TestComplexRecordType::TestComplexRecordType(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), typeSystem, {{getInt0Id(), babelwires::DefaultIntType::getThisIdentifier()},
                  {getOpIntId(), babelwires::DefaultIntType::getThisIdentifier(), Optionality::optionalDefaultInactive},
                  {getSubrecordId(), TestSimpleRecordType::getThisIdentifier()},
                  {getInt1Id(), babelwires::IntTypeConstructor::makeTypeExp(c_int1min, c_int1max, c_int1default)},
                  {getOpRecId(), TestSimpleRecordType::getThisIdentifier(), Optionality::optionalDefaultInactive},
                  {getStringId(), babelwires::StringType::getThisIdentifier()},
                  {getArrayId(), testDomain::TestSimpleArrayType::getThisIdentifier()},
                  {getOnOptId(), babelwires::DefaultIntType::getThisIdentifier(), Optionality::optionalDefaultActive }}) {}

babelwires::ShortId testDomain::TestComplexRecordType::getInt0Id() {
    return BW_SHORT_ID(s_intIdInitializer, s_intFieldName, "1aafde9a-fb39-4a2d-8a29-55fc9d6d093b");
}

babelwires::ShortId testDomain::TestComplexRecordType::getOpIntId() {
    return BW_SHORT_ID(s_opIntIdInitializer, s_opIntFieldName, "8d544864-ad75-4546-b523-5b4779e9120f");
}

babelwires::ShortId testDomain::TestComplexRecordType::getSubrecordId() {
    return BW_SHORT_ID(s_subRecordIdInitializer, s_subRecordFieldName, "b5855d2f-ad62-47c7-bbe1-af3d16b26d7d");
}

babelwires::ShortId testDomain::TestComplexRecordType::getInt1Id() {
    return BW_SHORT_ID(s_int1IdInitializer, s_int1FieldName, "1d21e657-a292-40c2-8fbf-491c2eaa1f8e");
}

babelwires::ShortId testDomain::TestComplexRecordType::getOpRecId() {
    return BW_SHORT_ID(s_opRecIdInitializer, s_opRecFieldName, "e0301eeb-1a6b-461a-aba4-ed3b85b88ebe");
}

babelwires::ShortId testDomain::TestComplexRecordType::getStringId() {
    return BW_SHORT_ID(s_stringIdInitializer, s_stringFieldName, "0f5c8454-89ed-4b66-a3f1-0174a620234c");
}

babelwires::ShortId testDomain::TestComplexRecordType::getArrayId() {
    return BW_SHORT_ID(s_arrayIdInitializer, s_arrayFieldName, "5da653f4-44d5-4030-a956-771fc06fb769");
}

babelwires::ShortId testDomain::TestComplexRecordType::getOnOptId() {
    return BW_SHORT_ID(s_onOptIdInitializer, s_onOptFieldName, "cb4e25af-9107-405a-bf02-18a95f56eae4");
}

testDomain::TestSimpleRecordElementData::TestSimpleRecordElementData()
    : babelwires::ValueNodeData(TestSimpleRecordType::getThisIdentifier()) {}

babelwires::Path testDomain::TestSimpleRecordElementData::getPathToRecord() {
    return babelwires::Path();
}

babelwires::Path testDomain::TestSimpleRecordElementData::getPathToRecordInt0() {
    babelwires::Path path = getPathToRecord();
    path.pushStep(TestSimpleRecordType::getInt0Id());
    return path;
}

babelwires::Path testDomain::TestSimpleRecordElementData::getPathToRecordInt1() {
    babelwires::Path path = getPathToRecord();
    path.pushStep(TestSimpleRecordType::getInt1Id());
    return path;
}

testDomain::TestComplexRecordElementData::TestComplexRecordElementData()
    : babelwires::ValueNodeData(TestComplexRecordType::getThisIdentifier()) {}

babelwires::Path testDomain::TestComplexRecordElementData::getPathToRecord() {
    return babelwires::Path();
}

babelwires::Path testDomain::TestComplexRecordElementData::getPathToRecordInt0() {
    babelwires::Path path = getPathToRecord();
    path.pushStep(TestComplexRecordType::getInt0Id());
    return path;
}

babelwires::Path testDomain::TestComplexRecordElementData::getPathToRecordOpInt() {
    babelwires::Path path = getPathToRecord();
    path.pushStep(TestComplexRecordType::getOpIntId());
    return path;
}

babelwires::Path testDomain::TestComplexRecordElementData::getPathToRecordSubrecord() {
    babelwires::Path path = getPathToRecord();
    path.pushStep(TestComplexRecordType::getSubrecordId());
    return path;
}

babelwires::Path testDomain::TestComplexRecordElementData::getPathToRecordSubrecordInt0() {
    babelwires::Path path = getPathToRecordSubrecord();
    path.pushStep(TestSimpleRecordType::getInt0Id());
    return path;
}

babelwires::Path testDomain::TestComplexRecordElementData::getPathToRecordSubrecordInt1() {
    babelwires::Path path = getPathToRecordSubrecord();
    path.pushStep(TestSimpleRecordType::getInt1Id());
    return path;
}

babelwires::Path testDomain::TestComplexRecordElementData::getPathToRecordInt1() {
    babelwires::Path path = getPathToRecord();
    path.pushStep(TestComplexRecordType::getInt1Id());
    return path;
}

babelwires::Path testDomain::TestComplexRecordElementData::getPathToRecordOpRec() {
    babelwires::Path path = getPathToRecord();
    path.pushStep(TestComplexRecordType::getOpRecId());
    return path;
}

babelwires::Path testDomain::TestComplexRecordElementData::getPathToRecordString() {
    babelwires::Path path = getPathToRecord();
    path.pushStep(TestComplexRecordType::getStringId());
    return path;
}

babelwires::Path testDomain::TestComplexRecordElementData::getPathToRecordArray() {
    babelwires::Path path = getPathToRecord();
    path.pushStep(TestComplexRecordType::getArrayId());
    return path;
}

babelwires::Path testDomain::TestComplexRecordElementData::getPathToRecordArrayEntry(unsigned int i) {
    babelwires::Path path = getPathToRecordArray();
    path.pushStep(i);
    return path;
}

babelwires::Path testDomain::TestComplexRecordElementData::getPathToRecordOpIntOn() {
    babelwires::Path path = getPathToRecord();
    path.pushStep(TestComplexRecordType::getOnOptId());
    return path;
}

testDomain::TestComplexRecordTypeFeatureInfo::TestComplexRecordTypeFeatureInfo(
    const babelwires::ValueTreeNode& testRecord)
    : m_record(testRecord)
    , m_int(testRecord.getChildFromStep(testDomain::TestComplexRecordType::getInt0Id()).as<babelwires::ValueTreeNode>())
    , m_array(
          testRecord.getChildFromStep(testDomain::TestComplexRecordType::getArrayId()).as<babelwires::ValueTreeNode>())
    , m_elem0(m_array.getChildFromStep(0).as<babelwires::ValueTreeNode>())
    , m_elem1(m_array.getChildFromStep(1).as<babelwires::ValueTreeNode>())
    , m_subRecord(testRecord.getChildFromStep(testDomain::TestComplexRecordType::getSubrecordId())
                      .as<babelwires::ValueTreeNode>())
    , m_subRecordInt(
          m_subRecord.getChildFromStep(testDomain::TestSimpleRecordType::getInt0Id()).as<babelwires::ValueTreeNode>())
    , m_pathToRecord(babelwires::getPathTo(&testRecord))
    , m_pathToInt(babelwires::getPathTo(&m_int))
    , m_pathToArray(babelwires::getPathTo(&m_array))
    , m_pathToElem0(babelwires::getPathTo(&m_elem0))
    , m_pathToElem1(babelwires::getPathTo(&m_elem1))
    , m_pathToSubRecord(babelwires::getPathTo(&m_subRecord))
    , m_pathToSubRecordInt(babelwires::getPathTo(&m_subRecordInt)) {}