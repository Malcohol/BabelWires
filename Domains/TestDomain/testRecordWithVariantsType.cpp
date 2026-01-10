#include <Domains/TestDomain/testRecordWithVariantsType.hpp>

#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>

#include <Domains/TestDomain/testRecordType.hpp>

testDomain::TestRecordWithVariantsType::TestRecordWithVariantsType(const babelwires::TypeSystem& typeSystem)
    : RecordWithVariantsType(getThisType(), typeSystem,
          {getTagAId(), getTagBId(), getTagCId(), getTagDId()},
          {{getFieldA0Id(), babelwires::DefaultIntType::getThisType(), {getTagAId(), getTagDId()}},
           {getFf0Id(), babelwires::DefaultIntType::getThisType()},
           {getFieldB0Id(), TestSimpleRecordType::getThisType(), {getTagBId()}},
           {getFieldABId(), babelwires::DefaultIntType::getThisType(), {getTagAId(), getTagBId(), getTagDId()}},
           {getFieldA1Id(), TestSimpleRecordType::getThisType(), {getTagAId(), getTagDId()}},
           {getFf1Id(), TestSimpleRecordType::getThisType()},
           {getFieldBCId(), babelwires::DefaultIntType::getThisType(), {getTagBId(), getTagCId()}}},
          1) {}

babelwires::ShortId testDomain::TestRecordWithVariantsType::getTagAId() {
    return BW_SHORT_ID("tagA", "Tag A", "942734bc-c048-43e0-8a2d-a428292b9d1d");
}

babelwires::ShortId testDomain::TestRecordWithVariantsType::getTagBId() {
    return BW_SHORT_ID("tagB", "Tag B", "dbd7f174-2b89-4b43-96ef-61e7a18aec02");
}

babelwires::ShortId testDomain::TestRecordWithVariantsType::getTagCId() {
    return BW_SHORT_ID("tagC", "Tag C", "ad6769b3-1cab-4afa-986e-ca8739264dc9");
}

babelwires::ShortId testDomain::TestRecordWithVariantsType::getTagDId() {
    return BW_SHORT_ID("tagD", "Tag D", "cfc32664-3a00-47f4-9102-87319bff0cb6");
}

babelwires::ShortId testDomain::TestRecordWithVariantsType::getFf0Id() {
    return BW_SHORT_ID("ff0", "Fixed Field 0", "6f4cdac6-f7dd-415b-a238-ea86aad9b689");
}

babelwires::ShortId testDomain::TestRecordWithVariantsType::getFf1Id() {
    return BW_SHORT_ID("ff1", "Fixed Field 1", "e10b92f9-4a04-44f6-bacd-09893701af05");
}

babelwires::ShortId testDomain::TestRecordWithVariantsType::getFieldA0Id() {
    return BW_SHORT_ID("A0", "Field A0", "e95a3fa2-7139-4722-b4b6-f8d4401dd5d0");
}

babelwires::ShortId testDomain::TestRecordWithVariantsType::getFieldA1Id() {
    return BW_SHORT_ID("A1", "Field A1", "d011c91e-cf7f-477a-b78b-e4ee274a73e3");
}

babelwires::ShortId testDomain::TestRecordWithVariantsType::getFieldB0Id() {
    return BW_SHORT_ID("B0", "Field B0", "f3a42eea-072b-4a47-ac96-6cbd396ee018");
}

babelwires::ShortId testDomain::TestRecordWithVariantsType::getFieldABId() {
    return BW_SHORT_ID("AB", "Field AB", "0a527151-ba53-43a3-aa33-c83b8d265294");
}

babelwires::ShortId testDomain::TestRecordWithVariantsType::getFieldBCId() {
    return BW_SHORT_ID("BC", "Field BC", "2fe76d96-014b-401c-af1e-35784c05c60a");
}

testDomain::TestRecordWithVariantsElementData::TestRecordWithVariantsElementData()
    : ValueNodeData(testDomain::TestRecordWithVariantsType::getThisType()) {}

babelwires::Path testDomain::TestRecordWithVariantsElementData::getPathToRecordWithVariants() {
    return babelwires::Path();
}

const babelwires::Path testDomain::TestRecordWithVariantsElementData::getPathToFieldA0() {
    return std::vector<babelwires::PathStep>{testDomain::TestRecordWithVariantsType::getFieldA0Id()};
}

const babelwires::Path testDomain::TestRecordWithVariantsElementData::getPathToFieldA1_Int0() {
    return std::vector<babelwires::PathStep>{testDomain::TestRecordWithVariantsType::getFieldA1Id(),
                                             testDomain::TestSimpleRecordType::getInt0Id()};
}

const babelwires::Path testDomain::TestRecordWithVariantsElementData::getPathToFieldAB() {
    return std::vector<babelwires::PathStep>{testDomain::TestRecordWithVariantsType::getFieldABId()};
}