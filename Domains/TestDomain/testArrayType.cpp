#include <Domains/TestDomain/testArrayType.hpp>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>

testDomain::TestSimpleArrayType::TestSimpleArrayType()
    : babelwires::ArrayType(getEntryTypeStatic(), s_minimumSize, s_maximumSize, s_defaultSize) {}

babelwires::TypeRef testDomain::TestSimpleArrayType::getEntryTypeStatic() {
    return babelwires::DefaultIntType::getThisType();
}

testDomain::TestCompoundArrayType::TestCompoundArrayType()
    : babelwires::ArrayType(getEntryTypeStatic(), s_minimumSize, s_maximumSize, s_defaultSize) {}

babelwires::TypeRef testDomain::TestCompoundArrayType::getEntryTypeStatic() {
    return testDomain::TestSimpleArrayType::getThisType();
}

testDomain::TestArrayElementData::TestArrayElementData()
    : babelwires::ValueNodeData(TestSimpleArrayType::getThisType()) {}

babelwires::Path testDomain::TestArrayElementData::getPathToArray() {
    return babelwires::Path();
}


babelwires::Path testDomain::TestArrayElementData::getPathToArrayEntry(unsigned int i) {
    babelwires::Path pathToArray_1 = getPathToArray();
    pathToArray_1.pushStep(i);
    return pathToArray_1;
}

babelwires::Path testDomain::TestArrayElementData::getPathToArray_1() {
    return getPathToArrayEntry(1);
}

babelwires::Path testDomain::TestArrayElementData::getPathToArray_2() {
    return getPathToArrayEntry(2);
}
