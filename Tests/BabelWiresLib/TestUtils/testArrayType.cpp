#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>

testUtils::TestSimpleArrayType::TestSimpleArrayType()
    : babelwires::ArrayType(getEntryTypeStatic(), s_minimumSize, s_maximumSize, s_defaultSize) {}

babelwires::TypeRef testUtils::TestSimpleArrayType::getEntryTypeStatic() {
    return babelwires::DefaultIntType::getThisType();
}

testUtils::TestCompoundArrayType::TestCompoundArrayType()
    : babelwires::ArrayType(getEntryTypeStatic(), s_minimumSize, s_maximumSize, s_defaultSize) {}

babelwires::TypeRef testUtils::TestCompoundArrayType::getEntryTypeStatic() {
    return testUtils::TestSimpleArrayType::getThisType();
}

testUtils::TestArrayElementData::TestArrayElementData()
    : babelwires::ValueNodeData(TestSimpleArrayType::getThisType()) {}

babelwires::Path testUtils::TestArrayElementData::getPathToArray() {
    return babelwires::Path();
}


babelwires::Path testUtils::TestArrayElementData::getPathToArrayEntry(unsigned int i) {
    babelwires::Path pathToArray_1 = getPathToArray();
    pathToArray_1.pushStep(i);
    return pathToArray_1;
}

babelwires::Path testUtils::TestArrayElementData::getPathToArray_1() {
    return getPathToArrayEntry(1);
}

babelwires::Path testUtils::TestArrayElementData::getPathToArray_2() {
    return getPathToArrayEntry(2);
}
