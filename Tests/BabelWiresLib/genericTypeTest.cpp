#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/valuePathUtils.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>

#include <Domains/TestDomain/testGenericType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <BabelWiresLib/Types/Int/intType.hpp>

namespace {
    void checkInstantiations(const babelwires::TypeSystem& typeSystem, const babelwires::GenericType& genericType,
                             const babelwires::ValueHolder& valueHolder, bool var0Assigned, bool var1Assigned) {
        {
            const auto var0memberValue = babelwires::tryFollowPath(
                typeSystem, genericType, testDomain::TestGenericType::getPathToVar0_member(), valueHolder);
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
                typeSystem, genericType, testDomain::TestGenericType::getPathToVar0_inNestedGenericType(), valueHolder);
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
                typeSystem, genericType, testDomain::TestGenericType::getPathToVar0_inArray(), valueHolder);
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
                typeSystem, genericType, testDomain::TestGenericType::getPathToVar1_member(), valueHolder);
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
    using namespace babelwires;

    testUtils::TestEnvironment env;
    babelwires::TypeSystem& typeSystem = env.m_typeSystem;

    babelwires::TypeRef testGenericTypeRef = testDomain::TestGenericType::getThisType();

    const babelwires::Type* const type = testGenericTypeRef.tryResolve(typeSystem);
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
    using namespace babelwires;

    testUtils::TestEnvironment env;
    babelwires::TypeSystem& typeSystem = env.m_typeSystem;

    babelwires::TypeRef testGenericTypeRef = testDomain::TestGenericType::getThisType();

    const babelwires::Type* const type = testGenericTypeRef.tryResolve(typeSystem);
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

    checkInstantiations(typeSystem, *genericType, valueHolder, false, false);

    std::vector<babelwires::TypeRef> typeAssignments(2);
    typeAssignments[1] = babelwires::DefaultIntType::getThisType();

    // Instantiate the type variable with 'int'.
    genericType->setTypeVariableAssignmentAndInstantiate(typeSystem, valueHolder, typeAssignments);
    EXPECT_FALSE(genericType->getTypeAssignment(valueHolder, 0));
    EXPECT_EQ(genericType->getTypeAssignment(valueHolder, 1), babelwires::DefaultIntType::getThisType());
    checkInstantiations(typeSystem, *genericType, valueHolder, false, true);

    typeAssignments[0] = babelwires::DefaultIntType::getThisType();

    genericType->setTypeVariableAssignmentAndInstantiate(typeSystem, valueHolder, typeAssignments);
    EXPECT_EQ(genericType->getTypeAssignment(valueHolder, 0), babelwires::DefaultIntType::getThisType());
    EXPECT_EQ(genericType->getTypeAssignment(valueHolder, 1), babelwires::DefaultIntType::getThisType());
    checkInstantiations(typeSystem, *genericType, valueHolder, true, true);

    typeAssignments[1] = babelwires::TypeRef();

    genericType->setTypeVariableAssignmentAndInstantiate(typeSystem, valueHolder, typeAssignments);
    EXPECT_EQ(genericType->getTypeAssignment(valueHolder, 0), babelwires::DefaultIntType::getThisType());
    EXPECT_FALSE(genericType->getTypeAssignment(valueHolder, 1));
    checkInstantiations(typeSystem, *genericType, valueHolder, true, false);

    typeAssignments[0] = babelwires::TypeRef();

    genericType->setTypeVariableAssignmentAndInstantiate(typeSystem, valueHolder, typeAssignments);
    EXPECT_FALSE(genericType->getTypeAssignment(valueHolder, 0));
    EXPECT_FALSE(genericType->getTypeAssignment(valueHolder, 1));
    checkInstantiations(typeSystem, *genericType, valueHolder, false, false);
}

TEST(GenericTypeTest, instantiateNestedTypeVariable) {
    using namespace babelwires;

    testUtils::TestEnvironment env;
    babelwires::TypeSystem& typeSystem = env.m_typeSystem;

    babelwires::TypeRef testGenericTypeRef = testDomain::TestGenericType::getThisType();

    const babelwires::Type* const type = testGenericTypeRef.tryResolve(typeSystem);
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
            typeSystem, *genericType, testDomain::TestGenericType::getPathToNestedVar0(), valueHolder);
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
