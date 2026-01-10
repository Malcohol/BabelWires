#include <Domains/TestDomain/testRecordWithVariantsTypeHierarchy.hpp>

namespace {
    babelwires::ShortId getIdForDefaultTag() {
        return BW_SHORT_ID("dtag", "dtag", "32b9b442-b802-40be-bc46-18e5c22682ea");
    }
    babelwires::ShortId getIdForTag0() {
        return BW_SHORT_ID("tag0", "tag0", "5ad43c59-358f-463b-8b1c-0dd8482c4463");
    }
    babelwires::ShortId getIdForTag1() {
        return BW_SHORT_ID("tag1", "tag1", "612d1fbe-efd0-4ee5-bb20-4694f1f757b4");
    }
    babelwires::ShortId getIdForA() {
        return BW_SHORT_ID("A", "A", "b30a8d72-a330-48e7-8f43-cdea54264c7b");
    }
    babelwires::ShortId getIdForB() {
        return BW_SHORT_ID("B", "B", "cb04cb2e-fd79-4837-8bf6-2ca9cd7d48f0");
    }
    babelwires::ShortId getIdFor0() {
        return BW_SHORT_ID("f0", "0", "5ed9b1c5-6337-47cf-bb32-324038b48f14");
    }
    babelwires::ShortId getIdFor1() {
        return BW_SHORT_ID("f1", "1", "19caf7f7-0bca-422d-857e-9205e962daee");
    }
} // namespace

testDomain::RecordVWithNoFields::RecordVWithNoFields(const babelwires::TypeSystem& typeSystem)
    : RecordWithVariantsType(getThisType(), typeSystem, {getIdForDefaultTag()}, {}) {}

testDomain::RecordVA0::RecordVA0(const babelwires::TypeSystem& typeSystem)
    : RecordWithVariantsType(getThisType(), typeSystem, {getIdForDefaultTag()}, {{getIdForA(), babelwires::DefaultIntType::getThisType()}}) {}

testDomain::RecordVA1::RecordVA1(const babelwires::TypeSystem& typeSystem)
    : RecordWithVariantsType(getThisType(), typeSystem, {getIdForDefaultTag()}, {{getIdForA(), babelwires::DefaultIntType::getThisType()}}) {}

testDomain::RecordVB::RecordVB(const babelwires::TypeSystem& typeSystem)
    : RecordWithVariantsType(getThisType(), typeSystem, {getIdForDefaultTag()}, {{getIdForB(), babelwires::DefaultIntType::getThisType()}}) {}

testDomain::RecordVAB::RecordVAB(const babelwires::TypeSystem& typeSystem)
    : RecordWithVariantsType(getThisType(), typeSystem, {getIdForDefaultTag()}, {{getIdForA(), babelwires::DefaultIntType::getThisType()},
                                                      {getIdForB(), babelwires::DefaultIntType::getThisType()}}) {}

testDomain::RecordVAS::RecordVAS(const babelwires::TypeSystem& typeSystem)
    : RecordWithVariantsType(getThisType(), typeSystem, {getIdForDefaultTag()}, {{getIdForA(), babelwires::StringType::getThisType()}}) {}

testDomain::RecordVAV0::RecordVAV0(const babelwires::TypeSystem& typeSystem)
    : RecordWithVariantsType(getThisType(), typeSystem, {getIdForDefaultTag(), getIdForTag0()},
                             {{getIdForA(), babelwires::DefaultIntType::getThisType()},
                              {getIdFor0(), babelwires::DefaultIntType::getThisType(), {getIdForTag0()}}}) {}

testDomain::RecordVABV0::RecordVABV0(const babelwires::TypeSystem& typeSystem)
    : RecordWithVariantsType(getThisType(), typeSystem, {getIdForDefaultTag(), getIdForTag0()},
                             {{getIdForA(), babelwires::DefaultIntType::getThisType()},
                              {getIdFor0(), babelwires::DefaultIntType::getThisType(), {getIdForTag0()}},
                              {getIdForB(), babelwires::DefaultIntType::getThisType()}}) {}

testDomain::RecordVABV1::RecordVABV1(const babelwires::TypeSystem& typeSystem)
    : RecordWithVariantsType(getThisType(), typeSystem, {getIdForDefaultTag(), getIdForTag0(), getIdForTag1()},
                             {{getIdForA(), babelwires::DefaultIntType::getThisType()},
                              {getIdFor1(), babelwires::DefaultIntType::getThisType(), {getIdForTag1()}},
                              {getIdForB(), babelwires::DefaultIntType::getThisType()}}) {}

testDomain::RecordVABV01::RecordVABV01(const babelwires::TypeSystem& typeSystem)
    : RecordWithVariantsType(getThisType(), typeSystem, {getIdForDefaultTag(), getIdForTag0()},
                             {{getIdForA(), babelwires::DefaultIntType::getThisType()},
                              {getIdFor0(), babelwires::DefaultIntType::getThisType(), {getIdForTag0()}},
                              {getIdForB(), babelwires::DefaultIntType::getThisType()},
                              {getIdFor1(), babelwires::DefaultIntType::getThisType(), {getIdForTag0()}}}) {}

testDomain::RecordVAVB::RecordVAVB(const babelwires::TypeSystem& typeSystem)
    : RecordWithVariantsType(getThisType(), typeSystem, {getIdForDefaultTag(), getIdForTag0()},
                             {{getIdForA(), babelwires::DefaultIntType::getThisType()},
                              {getIdForB(), babelwires::DefaultIntType::getThisType(), {getIdForDefaultTag()}}}) {}
