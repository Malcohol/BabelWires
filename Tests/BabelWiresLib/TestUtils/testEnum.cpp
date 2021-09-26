#include "Tests/BabelWiresLib/TestUtils/testEnum.hpp"

babelwires::Enum::EnumValues testUtils::getTestEnumValues() {
    babelwires::Enum::EnumValues values = {"Foo", "Bar", "Erm"};
    // We need to set the discriminators to avoid the assertions about the entries not being registered.
    for (int i = 0; i < values.size(); ++i) {
        values[i].setDiscriminator(i + 1);
    }
    return values;
}

namespace {
    babelwires::Enum::EnumValues g_values = testUtils::getTestEnumValues();
}

testUtils::TestEnum::TestEnum()
    : babelwires::Enum("TestEnum", "Test Enum", 1, g_values, 1)
{}
