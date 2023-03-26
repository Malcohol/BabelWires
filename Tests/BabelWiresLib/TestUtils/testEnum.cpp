#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

const babelwires::Enum::EnumValues testUtils::g_enumValues = {
    testUtils::getTestRegisteredIdentifier("Foo", 1), testUtils::getTestRegisteredIdentifier("Bar", 2),
    testUtils::getTestRegisteredIdentifier("Erm", 3), testUtils::getTestRegisteredIdentifier("Oom", 4),
    testUtils::getTestRegisteredIdentifier("Boo", 5),
};

testUtils::TestEnum::TestEnum()
    : babelwires::Enum(g_enumValues, 1) {}

testUtils::TestSubEnum::TestSubEnum()
    : babelwires::Enum(babelwires::Enum::EnumValues{testUtils::getTestRegisteredIdentifier("Bar", 2),
                                                    testUtils::getTestRegisteredIdentifier("Erm", 3),
                                                    testUtils::getTestRegisteredIdentifier("Oom", 4)},
                       1) {}

testUtils::TestSubSubEnum1::TestSubSubEnum1()
    : babelwires::Enum(babelwires::Enum::EnumValues{testUtils::getTestRegisteredIdentifier("Bar", 2),
                                                    testUtils::getTestRegisteredIdentifier("Erm", 3)},
                       1) {}

testUtils::TestSubSubEnum2::TestSubSubEnum2()
    : babelwires::Enum(babelwires::Enum::EnumValues{testUtils::getTestRegisteredIdentifier("Erm", 3),
                                                    testUtils::getTestRegisteredIdentifier("Oom", 4)},
                       1) {}
