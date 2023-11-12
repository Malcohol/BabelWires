#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>

#include <BabelWiresLib/Types/Int/intType.hpp>

testUtils::TestArrayType::TestArrayType()
    : babelwires::ArrayType(babelwires::DefaultIntType::getThisIdentifier(), 1, 10, 3) {}

testUtils::TestArrayType2::TestArrayType2()
    : babelwires::ArrayType(testUtils::TestArrayType::getThisIdentifier(), 1, 4, 2) {}
