#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

ENUM_DEFINE_ENUM_VALUE_SOURCE(testUtils::TestEnum, TESTENUM_VALUES);

testUtils::TestEnum::TestEnum()
    : babelwires::EnumType(getStaticValueSet(), 1) {}

testUtils::TestSubEnum::TestSubEnum()
    : babelwires::EnumType({TestEnum::getIdentifierFromValue(TestEnum::Value::Bar),
                            TestEnum::getIdentifierFromValue(TestEnum::Value::Erm),
                            TestEnum::getIdentifierFromValue(TestEnum::Value::Oom)},
                           1) {}

testUtils::TestSubSubEnum1::TestSubSubEnum1()
    : babelwires::EnumType({TestEnum::getIdentifierFromValue(TestEnum::Value::Bar),
                            TestEnum::getIdentifierFromValue(TestEnum::Value::Erm)},
                           1) {}

testUtils::TestSubSubEnum2::TestSubSubEnum2()
    : babelwires::EnumType({TestEnum::getIdentifierFromValue(TestEnum::Value::Erm),
                            TestEnum::getIdentifierFromValue(TestEnum::Value::Oom)},
                           1) {}
