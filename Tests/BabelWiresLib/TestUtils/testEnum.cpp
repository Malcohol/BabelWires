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
    // Avoid registering the enum
    babelwires::LongIdentifier getEnumId() {
        babelwires::LongIdentifier longId("TestEnum");
        longId.setDiscriminator(1);
        return longId;
    }

    babelwires::Enum::EnumValues g_values = testUtils::getTestEnumValues();
}

testUtils::TestEnum::TestEnum()
    : babelwires::Enum(getEnumId(), 1, g_values, 1)
{}
