#include <gtest/gtest.h>

#include <BabelWiresLib/ValueTree/valueTreeGenericTypeUtils.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Project/Modifiers/setTypeVariableModifierData.hpp>

#include <Domains/TestDomain/testGenericType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(ValueTreeGenericTypeUtilsTest, tryGetGenericTypeFromVariable) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ValueTreeRoot valueTree(testEnvironment.m_typeSystem, testDomain::TestGenericType::getThisType());
    valueTree.setToDefault();

    auto checkForVariable = [&](const babelwires::Path& pathToGenericType, const babelwires::Path& pathToVariable) {
        const babelwires::ValueTreeNode* const genericTypeNode =
            babelwires::tryFollowPath(pathToGenericType, valueTree);
        ASSERT_NE(genericTypeNode, nullptr);

        const babelwires::ValueTreeNode* const variable = babelwires::tryFollowPath(pathToVariable, valueTree);
        ASSERT_NE(variable, nullptr);

        const babelwires::ValueTreeNode* const genericTypeFromVar =
            babelwires::tryGetGenericTypeFromVariable(*variable);
        ASSERT_NE(genericTypeFromVar, nullptr);
        EXPECT_EQ(genericTypeFromVar, genericTypeNode);
    };

    checkForVariable(babelwires::Path(), testDomain::TestGenericType::getPathToX());
    checkForVariable(babelwires::Path(), testDomain::TestGenericType::getPathToY());
    checkForVariable(babelwires::Path(), testDomain::TestGenericType::getPathToNestedX());
    checkForVariable(babelwires::Path(), testDomain::TestGenericType::getPathToArray0());
    checkForVariable(testDomain::TestGenericType::getPathToNestedGenericType(),
                     testDomain::TestGenericType::getPathToNestedZ());
}

TEST(ValueTreeGenericTypeUtilsTest, containsUnresolvedTypeVariable) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ValueTreeRoot valueTree(testEnvironment.m_typeSystem, testDomain::TestGenericType::getThisType());
    valueTree.setToDefault();

    const testDomain::TestGenericType* const genericType = valueTree.getType().as<testDomain::TestGenericType>();

    babelwires::ValueTreeNode& nestedGenericTypeNode =
        babelwires::followPath(testDomain::TestGenericType::getPathToNestedGenericType(), valueTree);
    const babelwires::GenericType* const nestedGenericType =
        nestedGenericTypeNode.getType().as<babelwires::GenericType>();
    ASSERT_NE(nestedGenericType, nullptr);

    auto checkForVariable = [&](const babelwires::Path& pathToVariable) {
        const babelwires::ValueTreeNode& variable = babelwires::followPath(pathToVariable, valueTree);
        return babelwires::containsUnresolvedTypeVariable(variable);
    };

    EXPECT_EQ(checkForVariable(babelwires::Path()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToWrappedType()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToX()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToY()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToInt()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedGenericType()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedWrappedType()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedX()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedZ()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToArray()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToArray0()), true);

    {
        babelwires::SetTypeVariableModifierData nestedData;
        nestedData.m_typeAssignments.resize(1);
        nestedData.m_typeAssignments[0] = babelwires::StringType::getThisType();
        nestedData.apply(&nestedGenericTypeNode);
    }

    EXPECT_EQ(checkForVariable(babelwires::Path()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToWrappedType()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToX()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToY()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToInt()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedGenericType()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedWrappedType()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedX()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedZ()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToArray()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToArray0()), true);

    {
        babelwires::SetTypeVariableModifierData data;
        data.m_typeAssignments.resize(2);
        data.m_typeAssignments[0] = babelwires::StringType::getThisType();
        data.apply(&valueTree);

        // Have to re-apply to the nested generic type, because the outer application will have overwritten it.
        babelwires::SetTypeVariableModifierData nestedData;
        nestedData.m_typeAssignments.resize(1);
        nestedData.m_typeAssignments[0] = babelwires::StringType::getThisType();
        nestedData.apply(&nestedGenericTypeNode);
    }

    EXPECT_EQ(checkForVariable(babelwires::Path()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToWrappedType()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToX()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToY()), true);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToInt()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedGenericType()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedWrappedType()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedX()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedZ()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToArray()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToArray0()), false);

    {
        babelwires::SetTypeVariableModifierData data;
        data.m_typeAssignments.resize(2);
        data.m_typeAssignments[0] = babelwires::StringType::getThisType();
        data.m_typeAssignments[1] = babelwires::StringType::getThisType();
        data.apply(&valueTree);

        // Have to re-apply to the nested generic type, because the outer application will have overwritten it.
        babelwires::SetTypeVariableModifierData nestedData;
        nestedData.m_typeAssignments.resize(1);
        nestedData.m_typeAssignments[0] = babelwires::StringType::getThisType();
        nestedData.apply(&nestedGenericTypeNode);
    }

    EXPECT_EQ(checkForVariable(babelwires::Path()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToWrappedType()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToX()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToY()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToInt()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedGenericType()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedWrappedType()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedX()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedZ()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToArray()), false);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToArray0()), false);
}
