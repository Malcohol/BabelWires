#include <Domains/TestDomain/testEnum.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

ENUM_DEFINE_ENUM_VALUE_SOURCE(testDomain::TestEnum, TESTENUM_VALUES);

testDomain::TestEnum::TestEnum()
    : babelwires::EnumType(getStaticValueSet(), 1) {}

testDomain::TestSubEnum::TestSubEnum()
    : babelwires::EnumType({TestEnum::getIdentifierFromValue(TestEnum::Value::Bar),
                            TestEnum::getIdentifierFromValue(TestEnum::Value::Erm),
                            TestEnum::getIdentifierFromValue(TestEnum::Value::Oom)},
                           1) {}

testDomain::TestSubSubEnum1::TestSubSubEnum1()
    : babelwires::EnumType({TestEnum::getIdentifierFromValue(TestEnum::Value::Bar),
                            TestEnum::getIdentifierFromValue(TestEnum::Value::Erm)},
                           1) {}

testDomain::TestSubSubEnum2::TestSubSubEnum2()
    : babelwires::EnumType({TestEnum::getIdentifierFromValue(TestEnum::Value::Erm),
                            TestEnum::getIdentifierFromValue(TestEnum::Value::Oom)},
                           1) {}
