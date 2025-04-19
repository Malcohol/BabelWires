#include <Domains/TestDomain/testRecordTypeHierarchy.hpp>

namespace {
    babelwires::ShortId getIdForA() { return BW_SHORT_ID("A", "A", "31b7733a-555c-4970-9ebf-5b863272a86d"); }
    babelwires::ShortId getIdForB() { return BW_SHORT_ID("B", "B", "da90cbcc-c2a6-48f2-9ef5-9f2e41c487b4"); }
    babelwires::ShortId getIdForOpt() { return BW_SHORT_ID("Opt", "Opt", "38c2245d-f9da-4146-b60a-35afcc7c9f0f"); }
}


testDomain::RecordWithNoFields::RecordWithNoFields()
    : RecordType({}) {}

testDomain::RecordA0::RecordA0()
    : RecordType({{getIdForA(), babelwires::DefaultIntType::getThisType()}}) {}

testDomain::RecordA1::RecordA1()
    : RecordType({{getIdForA(), babelwires::DefaultIntType::getThisType()}}) {}

testDomain::RecordB::RecordB()
    : RecordType({{getIdForB(), babelwires::DefaultIntType::getThisType()}}) {}

testDomain::RecordAB::RecordAB()
    : RecordType({{getIdForA(), babelwires::DefaultIntType::getThisType()},
                  {getIdForB(), babelwires::DefaultIntType::getThisType()}}) {}

testDomain::RecordAS::RecordAS()
    : RecordType({{getIdForA(), babelwires::StringType::getThisType()}}) {}

testDomain::RecordAOpt::RecordAOpt()
    : RecordType({{getIdForA(), babelwires::DefaultIntType::getThisType()},
                  {getIdForOpt(), babelwires::DefaultIntType::getThisType(),
                   babelwires::RecordType::Optionality::optionalDefaultInactive}}) {}

testDomain::RecordABOpt::RecordABOpt()
    : RecordType({{getIdForA(), babelwires::DefaultIntType::getThisType()},
                  {getIdForB(), babelwires::DefaultIntType::getThisType()},
                  {getIdForOpt(), babelwires::DefaultIntType::getThisType(),
                   babelwires::RecordType::Optionality::optionalDefaultInactive}}) {}

testDomain::RecordAOptS::RecordAOptS()
    : RecordType({{getIdForA(), babelwires::DefaultIntType::getThisType()},
                  {getIdForOpt(), babelwires::StringType::getThisType(),
                   babelwires::RecordType::Optionality::optionalDefaultInactive}}) {}
