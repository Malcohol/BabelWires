#include <Domains/TestDomain/testGenericType.hpp>

#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/genericTypeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Record/recordTypeConstructor.hpp>

testDomain::TestGenericType::TestGenericType()
    : GenericType(
          // Note: We assume generic types are made using TypeRef expressions, so this cannot
          // use a registered record type. Similarly, the array cannot be a registered type.
          babelwires::RecordTypeConstructor::makeTypeRef(
              getIdOfX(), babelwires::TypeVariableTypeConstructor::makeTypeRef(), getIdOfY(),
              babelwires::TypeVariableTypeConstructor::makeTypeRef(1), getIdOfInt(),
              babelwires::DefaultIntType::getThisType(), getIdOfNestedGenericType(),
              babelwires::GenericTypeConstructor::makeTypeRef(babelwires::RecordTypeConstructor::makeTypeRef(
                  getIdOfNestedX(), babelwires::TypeVariableTypeConstructor::makeTypeRef(0, 1), getIdOfNestedZ(),
                  babelwires::TypeVariableTypeConstructor::makeTypeRef())),
              getIdOfArray(),
              babelwires::ArrayTypeConstructor::makeTypeRef(babelwires::TypeVariableTypeConstructor::makeTypeRef(), 0,
                                                            8, 1)),
          2) {}

babelwires::ShortId testDomain::TestGenericType::getIdOfX() {
    return BW_SHORT_ID("x", "x", "2bff1eef-8ca5-4988-85f7-7fb1f29e629c");
}

babelwires::ShortId testDomain::TestGenericType::getIdOfY() {
    return BW_SHORT_ID("y", "y", "ff0b9780-e3f3-46b6-a7ae-ac961425f9e8");
}
babelwires::ShortId testDomain::TestGenericType::getIdOfInt() {
    return BW_SHORT_ID("int", "int", "ffa6f718-4b8c-4797-8037-52216be06cde");
}
babelwires::ShortId testDomain::TestGenericType::getIdOfNestedGenericType() {
    return BW_SHORT_ID("nesgen", "nested generic", "c80d399e-25c3-4f24-96a4-ce908789ccdf");
}
babelwires::ShortId testDomain::TestGenericType::getIdOfNestedX() {
    return BW_SHORT_ID("x", "x", "df7c5f8c-8544-40b8-9123-c050d1c2ec8b");
}
babelwires::ShortId testDomain::TestGenericType::getIdOfNestedZ() {
    return BW_SHORT_ID("z", "z", "e455ef6c-9bf4-4da7-8308-d45501f7d43d");
}
babelwires::ShortId testDomain::TestGenericType::getIdOfArray() {
    return BW_SHORT_ID("array", "arr", "2c4d9dfc-100c-4a56-9323-ff8f420b91ce");
}

babelwires::Path testDomain::TestGenericType::getPathToVar0_member() {
    babelwires::Path path;
    path.pushStep(getStepToValue());
    path.pushStep(getIdOfX());
    return path;
}

babelwires::Path testDomain::TestGenericType::getPathToVar0_inNestedGenericType() {
    babelwires::Path path;
    path.pushStep(getStepToValue());
    path.pushStep(getIdOfNestedGenericType());
    path.pushStep(getStepToValue());
    path.pushStep(getIdOfNestedX());
    return path;
}

babelwires::Path testDomain::TestGenericType::getPathToArray() {
    babelwires::Path path;
    path.pushStep(getStepToValue());
    path.pushStep(getIdOfArray());
    return path;
}

babelwires::Path testDomain::TestGenericType::getPathToVar0_inArray() {
    babelwires::Path path = getPathToArray();
    path.pushStep(0);
    return path;
}

babelwires::Path testDomain::TestGenericType::getPathToVar1_member() {
    babelwires::Path path;
    path.pushStep(getStepToValue());
    path.pushStep(getIdOfY());
    return path;
}

babelwires::Path testDomain::TestGenericType::getPathToNestedVar0() {
    babelwires::Path path;
    path.pushStep(getStepToValue());
    path.pushStep(getIdOfNestedGenericType());
    path.pushStep(getStepToValue());
    path.pushStep(getIdOfNestedZ());
    return path;
}
