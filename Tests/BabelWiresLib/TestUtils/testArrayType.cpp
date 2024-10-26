#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>

testUtils::TestSimpleArrayType::TestSimpleArrayType()
    : babelwires::ArrayType(getEntryTypeStatic(), s_minimumSize, s_maximumSize, s_defaultSize) {}

babelwires::TypeRef testUtils::TestSimpleArrayType::getEntryTypeStatic() {
    return babelwires::DefaultIntType::getThisIdentifier();
}

testUtils::TestCompoundArrayType::TestCompoundArrayType()
    : babelwires::ArrayType(getEntryTypeStatic(), s_minimumSize, s_maximumSize, s_defaultSize) {}

babelwires::TypeRef testUtils::TestCompoundArrayType::getEntryTypeStatic() {
    return testUtils::TestSimpleArrayType::getThisIdentifier();
}

testUtils::TestArrayElementData::TestArrayElementData()
    : babelwires::ValueElementData(TestSimpleArrayType::getThisIdentifier()) {}

babelwires::Path testUtils::TestArrayElementData::getPathToArray() {
    return babelwires::Path();
}


babelwires::Path testUtils::TestArrayElementData::getPathToArrayEntry(unsigned int i) {
    babelwires::Path pathToArray_1 = getPathToArray();
    pathToArray_1.pushStep(babelwires::PathStep(i));
    return pathToArray_1;
}

babelwires::Path testUtils::TestArrayElementData::getPathToArray_1() {
    return getPathToArrayEntry(1);
}

babelwires::Path testUtils::TestArrayElementData::getPathToArray_2() {
    return getPathToArrayEntry(2);
}
