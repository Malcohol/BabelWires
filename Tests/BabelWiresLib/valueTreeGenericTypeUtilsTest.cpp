#include <gtest/gtest.h>

#include <BabelWiresLib/ValueTree/valueTreeGenericTypeUtils.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Project/Modifiers/setTypeVariableModifierData.hpp>

#include <Domains/TestDomain/testGenericType.hpp>
#include <Domains/TestDomain/testRecordType.hpp>

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

TEST(ValueTreeGenericTypeUtilsTest, containsUnassignedTypeVariable) {
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
        return babelwires::containsUnassignedTypeVariable(variable);
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


TEST(ValueTreeGenericTypeUtilsTest, getMaximumHeightOfUnassignedGenericType) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ValueTreeRoot valueTree(testEnvironment.m_typeSystem, testDomain::TestGenericType::getThisType());
    valueTree.setToDefault();

    const testDomain::TestGenericType* const genericType = valueTree.getType().as<testDomain::TestGenericType>();

    babelwires::ValueTreeNode& nestedGenericTypeNode =
        babelwires::followPath(testDomain::TestGenericType::getPathToNestedGenericType(), valueTree);
    const babelwires::GenericType* const nestedGenericType =
        nestedGenericTypeNode.getType().as<babelwires::GenericType>();
    ASSERT_NE(nestedGenericType, nullptr);

    auto checkForVariable = [&](const babelwires::Path& pathToVariable, unsigned int maximumPossible) {
        const babelwires::ValueTreeNode& variable = babelwires::followPath(pathToVariable, valueTree);
        return babelwires::getMaximumHeightOfUnassignedGenericType(variable, maximumPossible);
    };

    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToWrappedType(), 0), 0);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToX(), 0), 0);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToY(), 0), 0);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToInt(), 0), -1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedGenericType(), 0), 0);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedWrappedType(), 1), 1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedX(), 1), 1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedZ(), 1), 0);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToArray(), 0), 0);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToArray0(), 0), 0);

    {
        babelwires::SetTypeVariableModifierData nestedData;
        nestedData.m_typeAssignments.resize(1);
        nestedData.m_typeAssignments[0] = babelwires::StringType::getThisType();
        nestedData.apply(&nestedGenericTypeNode);
    }

    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToWrappedType(), 0), 0);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToX(), 0), 0);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToY(), 0), 0);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToInt(), 0), -1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedGenericType(), 0), 0);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedWrappedType(), 1), 1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedX(), 1), 1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedZ(), 1), -1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToArray(), 0), 0);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToArray0(), 0), 0);

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

    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToWrappedType(), 0), 0);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToX(), 0), -1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToY(), 0), 0);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToInt(), 0), -1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedGenericType(), 0), -1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedWrappedType(), 1), -1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedX(), 1), -1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedZ(), 1), -1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToArray(), 0), -1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToArray0(), 0), -1);

    {
        babelwires::SetTypeVariableModifierData data;
        data.m_typeAssignments.resize(2);
        data.m_typeAssignments[0] = babelwires::StringType::getThisType();
        data.m_typeAssignments[1] = babelwires::StringType::getThisType();
        data.apply(&valueTree);

        // The nested generic type will be unassigned.
    }

    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedWrappedType(), 0), 0);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedX(), 0), -1);
    EXPECT_EQ(checkForVariable(testDomain::TestGenericType::getPathToNestedZ(), 0), 0);
}

TEST(ValueTreeGenericTypeUtilsTest, getTypeVariableAssignments) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ValueTreeRoot targetTree(testEnvironment.m_typeSystem, testDomain::TestGenericType::getThisType());
    targetTree.setToDefault();   

    babelwires::ValueTreeRoot sourceTree(testEnvironment.m_typeSystem, testDomain::TestSimpleCompoundType::getThisType());
    sourceTree.setToDefault();

    babelwires::ValueTreeNode& targetNode =
        babelwires::followPath(testDomain::TestGenericType::getPathToNestedWrappedType(), targetTree);

    const auto assignments =
        babelwires::getTypeVariableAssignments(sourceTree, targetNode);
    
    ASSERT_TRUE(assignments.has_value());
    ASSERT_EQ(assignments->size(), 2);
    {
        const auto it = assignments->find({babelwires::Path(), 0});
        ASSERT_NE(it, assignments->end());
        EXPECT_EQ(it->second, babelwires::DefaultIntType::getThisType());
    }
    {
        const auto it = assignments->find({testDomain::TestGenericType::getPathToNestedGenericType(), 0});
        ASSERT_NE(it, assignments->end());
        EXPECT_EQ(it->second, babelwires::StringType::getThisType());
    }
}

TEST(ValueTreeGenericTypeUtilsTest, getTypeVariableAssignmentsFail) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ValueTreeRoot targetTree(testEnvironment.m_typeSystem, testDomain::TestGenericType::getThisType());
    targetTree.setToDefault();   

    babelwires::ValueTreeRoot sourceTree(testEnvironment.m_typeSystem, testDomain::TestSimpleRecordType::getThisType());
    sourceTree.setToDefault();

    babelwires::ValueTreeNode& targetNode =
        babelwires::followPath(testDomain::TestGenericType::getPathToNestedWrappedType(), targetTree);

    const auto assignments =
        babelwires::getTypeVariableAssignments(sourceTree, targetNode);
    
    ASSERT_FALSE(assignments.has_value());
}
