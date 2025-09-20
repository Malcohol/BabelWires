#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/valuePathUtils.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>

#include <Domains/TestDomain/testGenericType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <BabelWiresLib/Types/Int/intType.hpp>

namespace {
    void checkInstantiations(const babelwires::TypeSystem& typeSystem, const babelwires::GenericType& genericType,
                             const babelwires::ValueHolder& valueHolder, bool var0Assigned, bool var1Assigned) {
        {
            const auto var0memberValue = babelwires::tryFollowPath(
                typeSystem, genericType, testDomain::TestGenericType::getPathToX(), valueHolder);
            ASSERT_TRUE(var0memberValue);
            auto [var0memberType, var0memberValueHolder] = *var0memberValue;
            if (var0Assigned) {
                const auto* const intType = var0memberType.as<babelwires::IntType>();
                ASSERT_TRUE(intType);
                EXPECT_TRUE(intType->isValidValue(typeSystem, *var0memberValueHolder));
            } else {
                EXPECT_TRUE(var0memberType.as<babelwires::TypeVariableType>());
            }
        } // namespace
        {
            const auto var0inNestedGenericTypeValue = babelwires::tryFollowPath(
                typeSystem, genericType, testDomain::TestGenericType::getPathToNestedX(), valueHolder);
            ASSERT_TRUE(var0inNestedGenericTypeValue);
            auto [var0inNestedGenericTypeType, var0inNestedGenericTypeValueHolder] = *var0inNestedGenericTypeValue;
            if (var0Assigned) {
                const auto* const intType = var0inNestedGenericTypeType.as<babelwires::IntType>();
                ASSERT_TRUE(intType);
                EXPECT_TRUE(intType->isValidValue(typeSystem, *var0inNestedGenericTypeValueHolder));
            } else {
                EXPECT_TRUE(var0inNestedGenericTypeType.as<babelwires::TypeVariableType>());
            }
        }
        {
            const auto var0inArrayValue = babelwires::tryFollowPath(
                typeSystem, genericType, testDomain::TestGenericType::getPathToArray0(), valueHolder);
            ASSERT_TRUE(var0inArrayValue);
            auto [var0inArrayType, var0inArrayValueHolder] = *var0inArrayValue;
            if (var0Assigned) {
                const auto* const intType = var0inArrayType.as<babelwires::IntType>();
                ASSERT_TRUE(intType);
                EXPECT_TRUE(intType->isValidValue(typeSystem, *var0inArrayValueHolder));
            } else {
                EXPECT_TRUE(var0inArrayType.as<babelwires::TypeVariableType>());
            }
        }
        {
            const auto var1memberValue = babelwires::tryFollowPath(
                typeSystem, genericType, testDomain::TestGenericType::getPathToY(), valueHolder);
            ASSERT_TRUE(var1memberValue);
            auto [var1memberType, var1memberValueHolder] = *var1memberValue;
            if (var1Assigned) {
                const auto* const intType = var1memberType.as<babelwires::IntType>();
                ASSERT_TRUE(intType);
                EXPECT_TRUE(intType->isValidValue(typeSystem, *var1memberValueHolder));
            } else {
                EXPECT_TRUE(var1memberType.as<babelwires::TypeVariableType>());
            }
        }
    }
} // namespace

TEST(GenericTypeTest, createValue) {
    testUtils::TestEnvironment env;
    babelwires::TypeSystem& typeSystem = env.m_typeSystem;

    const babelwires::Type* const type = testDomain::TestGenericType::getThisType().tryResolve(typeSystem);
    ASSERT_NE(type, nullptr);

    const babelwires::GenericType* const genericType = type->as<babelwires::GenericType>();
    ASSERT_NE(genericType, nullptr);

    babelwires::ValueHolder valueHolder = genericType->createValue(typeSystem);
    ASSERT_TRUE(valueHolder);

    ASSERT_TRUE(genericType->isValidValue(typeSystem, *valueHolder));

    EXPECT_EQ(genericType->getNumVariables(), 2);

    // Initially, the type variable should be unassigned.
    EXPECT_FALSE(genericType->getTypeAssignment(valueHolder, 0));
    EXPECT_FALSE(genericType->getTypeAssignment(valueHolder, 1));
}

TEST(GenericTypeTest, instantiateTypeVariables) {
    testUtils::TestEnvironment env;
    babelwires::TypeSystem& typeSystem = env.m_typeSystem;

    const babelwires::Type* const type = testDomain::TestGenericType::getThisType().tryResolve(typeSystem);
    ASSERT_NE(type, nullptr);

    const babelwires::GenericType* const genericType = type->as<babelwires::GenericType>();
    ASSERT_NE(genericType, nullptr);

    babelwires::ValueHolder valueHolder = genericType->createValue(typeSystem);
    ASSERT_TRUE(valueHolder);
    ASSERT_TRUE(genericType->isValidValue(typeSystem, *valueHolder));
    ASSERT_EQ(genericType->getNumVariables(), 2);

    // Initially, the type variable should be unassigned.
    EXPECT_FALSE(genericType->getTypeAssignment(valueHolder, 0));
    EXPECT_FALSE(genericType->getTypeAssignment(valueHolder, 1));

    EXPECT_TRUE(genericType->isValidValue(typeSystem, *valueHolder));
    checkInstantiations(typeSystem, *genericType, valueHolder, false, false);

    std::vector<babelwires::TypeRef> typeAssignments(2);
    typeAssignments[1] = babelwires::DefaultIntType::getThisType();

    // Instantiate the type variable with 'int'.
    genericType->setTypeVariableAssignmentAndInstantiate(typeSystem, valueHolder, typeAssignments);
    EXPECT_FALSE(genericType->getTypeAssignment(valueHolder, 0));
    EXPECT_EQ(genericType->getTypeAssignment(valueHolder, 1), babelwires::DefaultIntType::getThisType());
    EXPECT_TRUE(genericType->isValidValue(typeSystem, *valueHolder));
    checkInstantiations(typeSystem, *genericType, valueHolder, false, true);

    typeAssignments[0] = babelwires::DefaultIntType::getThisType();

    genericType->setTypeVariableAssignmentAndInstantiate(typeSystem, valueHolder, typeAssignments);
    EXPECT_EQ(genericType->getTypeAssignment(valueHolder, 0), babelwires::DefaultIntType::getThisType());
    EXPECT_EQ(genericType->getTypeAssignment(valueHolder, 1), babelwires::DefaultIntType::getThisType());
    EXPECT_TRUE(genericType->isValidValue(typeSystem, *valueHolder));
    checkInstantiations(typeSystem, *genericType, valueHolder, true, true);

    typeAssignments[1] = babelwires::TypeRef();

    genericType->setTypeVariableAssignmentAndInstantiate(typeSystem, valueHolder, typeAssignments);
    EXPECT_EQ(genericType->getTypeAssignment(valueHolder, 0), babelwires::DefaultIntType::getThisType());
    EXPECT_FALSE(genericType->getTypeAssignment(valueHolder, 1));
    EXPECT_TRUE(genericType->isValidValue(typeSystem, *valueHolder));
    checkInstantiations(typeSystem, *genericType, valueHolder, true, false);

    typeAssignments[0] = babelwires::TypeRef();

    genericType->setTypeVariableAssignmentAndInstantiate(typeSystem, valueHolder, typeAssignments);
    EXPECT_FALSE(genericType->getTypeAssignment(valueHolder, 0));
    EXPECT_FALSE(genericType->getTypeAssignment(valueHolder, 1));
    EXPECT_TRUE(genericType->isValidValue(typeSystem, *valueHolder));
    checkInstantiations(typeSystem, *genericType, valueHolder, false, false);
}

TEST(GenericTypeTest, instantiateNestedTypeVariable) {
    testUtils::TestEnvironment env;
    babelwires::TypeSystem& typeSystem = env.m_typeSystem;

    const babelwires::Type* const type = testDomain::TestGenericType::getThisType().tryResolve(typeSystem);
    ASSERT_NE(type, nullptr);

    const babelwires::GenericType* const genericType = type->as<babelwires::GenericType>();
    ASSERT_NE(genericType, nullptr);

    babelwires::ValueHolder valueHolder = genericType->createValue(typeSystem);
    ASSERT_TRUE(valueHolder);

    auto [nestedType, nestedValue] = babelwires::followPathNonConst(
        typeSystem, *genericType, testDomain::TestGenericType::getPathToNestedGenericType(), valueHolder);
    
    const babelwires::GenericType* const nestedGenericType = nestedType.as<babelwires::GenericType>();
    ASSERT_NE(nestedGenericType, nullptr);

    auto checkNestedInstantiation = [&](bool isInstantiated)  {
        const auto nestedVarMemberValue = babelwires::tryFollowPath(
            typeSystem, *genericType, testDomain::TestGenericType::getPathToNestedZ(), valueHolder);
        ASSERT_TRUE(nestedVarMemberValue);
        auto [nestedVar0Type, nestedVar0ValueHolder] = *nestedVarMemberValue;
        if (isInstantiated) {
            const auto* const intType = nestedVar0Type.as<babelwires::IntType>();
            ASSERT_TRUE(intType);
            EXPECT_TRUE(intType->isValidValue(typeSystem, *nestedVar0ValueHolder));
        } else {
            EXPECT_TRUE(nestedVar0Type.as<babelwires::TypeVariableType>());
        }
    };

    checkNestedInstantiation(false);
    checkInstantiations(typeSystem, *genericType, valueHolder, false, false);

    std::vector<babelwires::TypeRef> typeAssignments(1);
    typeAssignments[0] = babelwires::DefaultIntType::getThisType();
    nestedGenericType->setTypeVariableAssignmentAndInstantiate(typeSystem, nestedValue, typeAssignments);

    checkNestedInstantiation(true);
    checkInstantiations(typeSystem, *genericType, valueHolder, false, false);
}

TEST(GenericTypeTest, childTypeAndTypeNames) {
    testUtils::TestEnvironment env;
    babelwires::TypeSystem& typeSystem = env.m_typeSystem;

    const babelwires::Type* const type = testDomain::TestGenericType::getThisType().tryResolve(typeSystem);
    ASSERT_NE(type, nullptr);

    const babelwires::GenericType* const genericType = type->as<babelwires::GenericType>();
    ASSERT_NE(genericType, nullptr);

    babelwires::ValueHolder valueHolder = genericType->createValue(typeSystem);
    ASSERT_TRUE(valueHolder);

    EXPECT_EQ(genericType->valueToString(typeSystem, valueHolder), "<T, U>");

    auto checkNames = [&](bool var0Assigned, bool var1Assigned)  {
        auto [childValue, step, childTypeRef] = genericType->getChild(valueHolder, 0);
        const babelwires::Type* const childType = childTypeRef.tryResolve(typeSystem);
        EXPECT_NE(childType, nullptr);
        EXPECT_EQ(step, babelwires::GenericType::getStepToValue());

        // "Record{x, y, int, nested generic, arr : TVar(0,0,-), TVar(1,0,-), Integer, Gen<1>{Record{x, z : TVar(0,1,-), TVar(0,0,-)}}, Array<TVar(0,0,-)>[0..8]}"
        const std::string childTypeName = childType->getName();
        EXPECT_NE(childTypeName.find("TVar(0,0,-)"), std::string::npos);
        if (var1Assigned) {
            EXPECT_NE(childTypeName.find("TVar(1,0,String)"), std::string::npos);
        } else {
            EXPECT_NE(childTypeName.find("TVar(1,0,-)"), std::string::npos);
        }
        EXPECT_EQ(childTypeName.find("Record{"), 0);
        EXPECT_NE(childTypeName.find("Gen<1>{Record{"), std::string::npos);
        if (var0Assigned) {
            EXPECT_EQ(childTypeName.find("TVar(0,1,String)"), std::string::npos);
            EXPECT_NE(childTypeName.find("Array<TVar(0,0,String)>["), std::string::npos);
        } else {
            EXPECT_NE(childTypeName.find("TVar(0,1,-)"), std::string::npos);
            EXPECT_NE(childTypeName.find("Array<TVar(0,0,-)>["), std::string::npos);
        }
    };

    checkNames(false, false);

    std::vector<babelwires::TypeRef> typeAssignments(2);
    typeAssignments[1] = babelwires::StringType::getThisType();

    genericType->setTypeVariableAssignmentAndInstantiate(typeSystem, valueHolder, typeAssignments);

    EXPECT_EQ(genericType->valueToString(typeSystem, valueHolder), "<T, String>");
    checkNames(false, true);
}
