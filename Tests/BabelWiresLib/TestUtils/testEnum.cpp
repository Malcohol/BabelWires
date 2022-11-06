#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

const babelwires::Enum::EnumValues testUtils::g_enumValues = {
    testUtils::getTestRegisteredIdentifier("Foo", 1), testUtils::getTestRegisteredIdentifier("Bar", 2),
    testUtils::getTestRegisteredIdentifier("Erm", 3), testUtils::getTestRegisteredIdentifier("Oom", 4),
    testUtils::getTestRegisteredIdentifier("Boo", 5),
};

babelwires::LongIdentifier testUtils::TestEnum::getThisIdentifier() {
    return getTestRegisteredLongIdentifier("TestEnum");
}

testUtils::TestEnum::TestEnum()
    : babelwires::Enum(getThisIdentifier(), 1, g_enumValues, 1) {}

babelwires::LongIdentifier testUtils::TestSubEnum::getThisIdentifier() {
    return getTestRegisteredLongIdentifier("TestSubEnum");
}

testUtils::TestSubEnum::TestSubEnum()
    : babelwires::Enum(getThisIdentifier(), 1, babelwires::Enum::EnumValues{"Bar", "Erm", "Oom"}, 1,
                       testUtils::TestEnum::getThisIdentifier()) {}

babelwires::LongIdentifier testUtils::TestSubSubEnum1::getThisIdentifier() {
    return getTestRegisteredLongIdentifier("TestSubSubEnum1");
}

testUtils::TestSubSubEnum1::TestSubSubEnum1()
    : babelwires::Enum(getThisIdentifier(), 1, babelwires::Enum::EnumValues{"Bar", "Erm"}, 1,
                       testUtils::TestSubEnum::getThisIdentifier()) {}

babelwires::LongIdentifier testUtils::TestSubSubEnum2::getThisIdentifier() {
    return getTestRegisteredLongIdentifier("TestSubSubEnum2");
}

testUtils::TestSubSubEnum2::TestSubSubEnum2()
    : babelwires::Enum(getThisIdentifier(), 1, babelwires::Enum::EnumValues{"Erm", "Oom"}, 1,
                       testUtils::TestSubEnum::getThisIdentifier()) {}

void testUtils::addTestEnumTypes(babelwires::TypeSystem& typeSystem) {
    typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum1>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum2>());

    typeSystem.addRelatedTypes(testUtils::TestEnum::getThisIdentifier(),
                               {{}, {testUtils::TestSubSubEnum1::getThisIdentifier()}});
    typeSystem.addRelatedTypes(testUtils::TestSubSubEnum2::getThisIdentifier(),
                               {{testUtils::TestEnum::getThisIdentifier()}, {}});
    typeSystem.addRelatedTypes(
        testUtils::TestSubEnum::getThisIdentifier(),
        {{testUtils::TestEnum::getThisIdentifier()},
         {testUtils::TestSubSubEnum1::getThisIdentifier(), testUtils::TestSubSubEnum2::getThisIdentifier()}});
}
