#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

ENUM_DEFINE_ENUM_VALUE_SOURCE(testUtils::TestEnum, TESTENUM_VALUES);

const babelwires::EnumType::ValueSet testUtils::g_enumValues = {
    testUtils::getTestRegisteredIdentifier("Foo", 1), testUtils::getTestRegisteredIdentifier("Bar", 2),
    testUtils::getTestRegisteredIdentifier("Erm", 3), testUtils::getTestRegisteredIdentifier("Oom", 4),
    testUtils::getTestRegisteredIdentifier("Boo", 5),
};

testUtils::TestEnum::TestEnum()
    : babelwires::EnumType(g_enumValues, 1) {}

testUtils::TestSubEnum::TestSubEnum()
    : babelwires::EnumType(babelwires::EnumType::ValueSet{testUtils::getTestRegisteredIdentifier("Bar", 2),
                                                    testUtils::getTestRegisteredIdentifier("Erm", 3),
                                                    testUtils::getTestRegisteredIdentifier("Oom", 4)},
                       1) {}

testUtils::TestSubSubEnum1::TestSubSubEnum1()
    : babelwires::EnumType(babelwires::EnumType::ValueSet{testUtils::getTestRegisteredIdentifier("Bar", 2),
                                                    testUtils::getTestRegisteredIdentifier("Erm", 3)},
                       1) {}

testUtils::TestSubSubEnum2::TestSubSubEnum2()
    : babelwires::EnumType(babelwires::EnumType::ValueSet{testUtils::getTestRegisteredIdentifier("Erm", 3),
                                                    testUtils::getTestRegisteredIdentifier("Oom", 4)},
                       1) {}
