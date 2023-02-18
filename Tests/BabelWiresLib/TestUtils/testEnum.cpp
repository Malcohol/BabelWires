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

void testUtils::addTestEnumTypes(babelwires::TypeSystem& typeSystem) {
    typeSystem.addEntry<testUtils::TestEnum>();
    typeSystem.addEntry<testUtils::TestSubEnum>();
    typeSystem.addEntry<testUtils::TestSubSubEnum1>();
    typeSystem.addEntry<testUtils::TestSubSubEnum2>();

    typeSystem.addRelatedTypes(testUtils::TestEnum::getThisIdentifier(),
                               {{}, {testUtils::TestSubSubEnum1::getThisIdentifier()}});
    typeSystem.addRelatedTypes(testUtils::TestSubSubEnum2::getThisIdentifier(),
                               {{testUtils::TestEnum::getThisIdentifier()}, {}});
    typeSystem.addRelatedTypes(
        testUtils::TestSubEnum::getThisIdentifier(),
        {{testUtils::TestEnum::getThisIdentifier()},
         {testUtils::TestSubSubEnum1::getThisIdentifier(), testUtils::TestSubSubEnum2::getThisIdentifier()}});
}
