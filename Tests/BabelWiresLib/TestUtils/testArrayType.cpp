#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>

#include <BabelWiresLib/Types/Int/intType.hpp>

testUtils::TestSimpleArrayType::TestSimpleArrayType()
    : babelwires::ArrayType(getExpectedEntryType(), s_minimumSize, s_maximumSize, s_defaultSize) {}

babelwires::TypeRef testUtils::TestSimpleArrayType::getExpectedEntryType() {
    return babelwires::DefaultIntType::getThisIdentifier();
}

testUtils::TestCompoundArrayType::TestCompoundArrayType()
    : babelwires::ArrayType(getExpectedEntryType(), s_minimumSize, s_maximumSize, s_defaultSize) {}

babelwires::TypeRef testUtils::TestCompoundArrayType::getExpectedEntryType() {
    return testUtils::TestSimpleArrayType::getThisIdentifier();
}
