#include <Domains/TestDomain/testRecordTypeHierarchy.hpp>

#include <BabelWiresLib/Types/String/stringType.hpp>

#include <Domains/TestDomain/testEnum.hpp>

namespace {
    babelwires::ShortId getIdForA() {
        return BW_SHORT_ID("A", "A", "31b7733a-555c-4970-9ebf-5b863272a86d");
    }
    babelwires::ShortId getIdForB() {
        return BW_SHORT_ID("B", "B", "da90cbcc-c2a6-48f2-9ef5-9f2e41c487b4");
    }
    babelwires::ShortId getIdForC() {
        return BW_SHORT_ID("C", "C", "607abbee-4f11-42ed-8a20-31fcdcebf1b8");
    }
    babelwires::ShortId getIdForOpt() {
        return BW_SHORT_ID("Opt", "Opt", "38c2245d-f9da-4146-b60a-35afcc7c9f0f");
    }
    babelwires::ShortId getIdForOpt2() {
        return BW_SHORT_ID("Opt2", "Opt2", "47afe67c-039e-4c73-b34b-4c6edf39688f");
    }
} // namespace

testDomain::RecordWithNoFields::RecordWithNoFields()
    : RecordType({}) {}

testDomain::RecordA0::RecordA0()
    : RecordType({{getIdForA(), TestSubEnum::getThisType()}}) {}

testDomain::RecordA1::RecordA1()
    : RecordType({{getIdForA(), TestSubEnum::getThisType()}}) {}

testDomain::RecordB::RecordB()
    : RecordType({{getIdForB(), TestSubEnum::getThisType()}}) {}

testDomain::RecordAB::RecordAB()
    : RecordType({{getIdForA(), TestSubEnum::getThisType()}, {getIdForB(), TestSubEnum::getThisType()}}) {}

testDomain::RecordAS::RecordAS()
    : RecordType({{getIdForA(), babelwires::StringType::getThisType()}}) {}

testDomain::RecordAOpt::RecordAOpt()
    : RecordType(
          {{getIdForA(), TestSubEnum::getThisType()},
           {getIdForOpt(), TestSubEnum::getThisType(), babelwires::RecordType::Optionality::optionalDefaultInactive}}) {
}

testDomain::RecordAOptFixed::RecordAOptFixed()
    : RecordType(
          {{getIdForA(), TestSubEnum::getThisType()},
           {getIdForOpt(), TestSubEnum::getThisType()}}) {
}

testDomain::RecordABOpt::RecordABOpt()
    : RecordType(
          {{getIdForA(), TestSubEnum::getThisType()},
           {getIdForB(), TestSubEnum::getThisType()},
           {getIdForOpt(), TestSubEnum::getThisType(), babelwires::RecordType::Optionality::optionalDefaultInactive}}) {
}

testDomain::RecordAOptS::RecordAOptS()
    : RecordType({{getIdForA(), TestSubEnum::getThisType()},
                  {getIdForOpt(), babelwires::StringType::getThisType(),
                   babelwires::RecordType::Optionality::optionalDefaultInactive}}) {}

testDomain::RecordABOptChild::RecordABOptChild(const babelwires::TypeSystem& typeSystem)
    : RecordType(RecordABOpt::getThisType().resolve(typeSystem)->is<babelwires::RecordType>(),
                 {{getIdForC(), TestSubEnum::getThisType()},
                  {getIdForOpt2(), TestSubEnum::getThisType(),
                   babelwires::RecordType::Optionality::optionalDefaultInactive}}) {}

testDomain::RecordAsub0::RecordAsub0()
    : RecordType({{getIdForA(), TestSubSubEnum1::getThisType()}}) {}

testDomain::RecordAsubBsup::RecordAsubBsup()
    : RecordType({{getIdForA(), TestSubSubEnum1::getThisType()}, {getIdForB(), TestEnum::getThisType()}}) {}

