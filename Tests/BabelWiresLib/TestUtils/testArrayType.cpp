#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>

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

testUtils::TestArrayElementData::TestArrayElementData()
    : babelwires::ValueElementData(TestSimpleArrayType::getThisIdentifier()) {}

babelwires::FeaturePath testUtils::TestArrayElementData::getPathToArray() {
    babelwires::FeaturePath pathToArray;
    pathToArray.pushStep(babelwires::PathStep(babelwires::ValueElement::getStepToValue()));
    return pathToArray;
}

babelwires::FeaturePath testUtils::TestArrayElementData::getPathToArray_1() {
    babelwires::FeaturePath pathToArray_1 = getPathToArray();
    pathToArray_1.pushStep(babelwires::PathStep(1));
    return pathToArray_1;
}

babelwires::FeaturePath testUtils::TestArrayElementData::getPathToArray_2() {
    babelwires::FeaturePath pathToArray_2 = getPathToArray();
    pathToArray_2.pushStep(babelwires::PathStep(2));
    return pathToArray_2;
}
