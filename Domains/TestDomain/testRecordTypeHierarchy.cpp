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

testDomain::RecordWithNoFields::RecordWithNoFields(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), typeSystem, {}) {}

testDomain::RecordA0::RecordA0(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), typeSystem, {{getIdForA(), TestSubEnum::getThisIdentifier()}}) {}

testDomain::RecordA1::RecordA1(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), typeSystem, {{getIdForA(), TestSubEnum::getThisIdentifier()}}) {}

testDomain::RecordB::RecordB(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), typeSystem, {{getIdForB(), TestSubEnum::getThisIdentifier()}}) {}

testDomain::RecordAB::RecordAB(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), typeSystem, {{getIdForA(), TestSubEnum::getThisIdentifier()}, {getIdForB(), TestSubEnum::getThisIdentifier()}}) {}

testDomain::RecordAS::RecordAS(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), typeSystem, {{getIdForA(), babelwires::StringType::getThisIdentifier()}}) {}

testDomain::RecordAOpt::RecordAOpt(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), 
          typeSystem, {{getIdForA(), TestSubEnum::getThisIdentifier()},
           {getIdForOpt(), TestSubEnum::getThisIdentifier(), babelwires::RecordType::Optionality::optionalDefaultInactive}}) {
}

testDomain::RecordAOptFixed::RecordAOptFixed(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), 
          typeSystem, {{getIdForA(), TestSubEnum::getThisIdentifier()},
           {getIdForOpt(), TestSubEnum::getThisIdentifier()}}) {
}

testDomain::RecordABOpt::RecordABOpt(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), 
          typeSystem, {{getIdForA(), TestSubEnum::getThisIdentifier()},
           {getIdForB(), TestSubEnum::getThisIdentifier()},
           {getIdForOpt(), TestSubEnum::getThisIdentifier(), babelwires::RecordType::Optionality::optionalDefaultInactive}}) {
}

testDomain::RecordAOptS::RecordAOptS(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), typeSystem, {{getIdForA(), TestSubEnum::getThisIdentifier()},
                  {getIdForOpt(), babelwires::StringType::getThisIdentifier(),
                   babelwires::RecordType::Optionality::optionalDefaultInactive}}) {}

testDomain::RecordABOptChild::RecordABOptChild(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), typeSystem, babelwires::TypeExp(RecordABOpt::getThisIdentifier()).resolve(typeSystem)->as<babelwires::RecordType>(),
                 {{getIdForC(), TestSubEnum::getThisIdentifier()},
                  {getIdForOpt2(), TestSubEnum::getThisIdentifier(),
                   babelwires::RecordType::Optionality::optionalDefaultInactive}}) {}

testDomain::RecordAsub0::RecordAsub0(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), typeSystem, {{getIdForA(), TestSubSubEnum1::getThisIdentifier()}}) {}

testDomain::RecordAsubBsup::RecordAsubBsup(const babelwires::TypeSystem& typeSystem)
    : RecordType(getThisIdentifier(), typeSystem, {{getIdForA(), TestSubSubEnum1::getThisIdentifier()}, {getIdForB(), TestEnum::getThisIdentifier()}}) {}

