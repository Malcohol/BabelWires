#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Modifiers/setTypeVariableModifierData.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/genericValue.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>

#include <BaseLib/Serialization/XML/xmlDeserializer.hpp>
#include <BaseLib/Serialization/XML/xmlSerializer.hpp>

#include <Domains/TestDomain/testGenericType.hpp>
#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(SetTypeVariableModifierDataTest, setSingleTypeVariable) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ValueTreeRoot valueTree(testEnvironment.m_typeSystem, testDomain::TestGenericType::getThisIdentifier());
    valueTree.setToDefault();

    const auto* const type = valueTree.getType()->tryAs<testDomain::TestGenericType>();
    ASSERT_NE(type, nullptr);

    const auto& value = valueTree.getValue();
    const auto* const genericValue = value->tryAs<babelwires::GenericValue>();
    ASSERT_NE(genericValue, nullptr);
    ASSERT_EQ(type->getNumVariables(), 2);

    // Initially, the type variable should be unassigned.
    EXPECT_FALSE(type->getTypeAssignment(value, 0));
    EXPECT_FALSE(type->getTypeAssignment(value, 1));

    babelwires::SetTypeVariableModifierData data;
    data.m_typeAssignments.resize(2);
    data.m_typeAssignments[0] = babelwires::StringType::getThisIdentifier();

    data.apply(&valueTree);

    const auto& valueAfterFirstApplication = valueTree.getValue();
    const auto* const genericValueAfterFirstApplication = valueAfterFirstApplication->tryAs<babelwires::GenericValue>();
    ASSERT_NE(genericValueAfterFirstApplication, nullptr);
    EXPECT_EQ(type->getTypeAssignment(valueAfterFirstApplication, 0), babelwires::StringType::getThisIdentifier());
    EXPECT_FALSE(type->getTypeAssignment(valueAfterFirstApplication, 1));

    data.m_typeAssignments[1] = babelwires::StringType::getThisIdentifier();

    data.apply(&valueTree);

    const auto& valueAfterSecondApplication = valueTree.getValue();
    const auto* const genericValueAfterSecondApplication = valueAfterSecondApplication->tryAs<babelwires::GenericValue>();
    ASSERT_NE(genericValueAfterSecondApplication, nullptr);
    EXPECT_EQ(type->getTypeAssignment(valueAfterSecondApplication, 0), babelwires::StringType::getThisIdentifier());
    EXPECT_EQ(type->getTypeAssignment(valueAfterSecondApplication, 1), babelwires::StringType::getThisIdentifier());

    data.m_typeAssignments[0] = babelwires::TypeExp();

    data.apply(&valueTree);

    const auto& valueAfterThirdApplication = valueTree.getValue();
    const auto* const genericValueAfterThirdApplication = valueAfterThirdApplication->tryAs<babelwires::GenericValue>();
    ASSERT_NE(genericValueAfterThirdApplication, nullptr);
    EXPECT_FALSE(type->getTypeAssignment(valueAfterThirdApplication, 0));
    EXPECT_EQ(type->getTypeAssignment(valueAfterThirdApplication, 1), babelwires::StringType::getThisIdentifier());
}

TEST(SetTypeVariableModifierDataTest, failureNotAGenericType) {
    testUtils::TestEnvironment testEnvironment;
    
    babelwires::SetTypeVariableModifierData data;
    data.m_typeAssignments.resize(1);
    data.m_typeAssignments[0] = babelwires::StringType::getThisIdentifier();

    babelwires::ValueTreeRoot valueTree(testEnvironment.m_typeSystem, testDomain::TestSimpleRecordType::getThisIdentifier());

    valueTree.setToDefault();

    EXPECT_THROW(data.apply(&valueTree), babelwires::ModelException);
}

TEST(SetTypeVariableModifierDataTest, failureTooFewTypeVariables) {
    testUtils::TestEnvironment testEnvironment;
    
    babelwires::SetTypeVariableModifierData data;
    data.m_typeAssignments.resize(1);
    data.m_typeAssignments[0] = babelwires::StringType::getThisIdentifier();

    babelwires::ValueTreeRoot valueTree(testEnvironment.m_typeSystem, testDomain::TestGenericType::getThisIdentifier());

    valueTree.setToDefault();

    EXPECT_THROW(data.apply(&valueTree), babelwires::ModelException);
}


TEST(SetTypeVariableModifierDataTest, failureTooManyTypeVariables) {
    testUtils::TestEnvironment testEnvironment;
    
    babelwires::SetTypeVariableModifierData data;
    data.m_typeAssignments.resize(3);
    data.m_typeAssignments[0] = babelwires::StringType::getThisIdentifier();
    data.m_typeAssignments[1] = babelwires::StringType::getThisIdentifier();
    data.m_typeAssignments[2] = babelwires::StringType::getThisIdentifier();

    babelwires::ValueTreeRoot valueTree(testEnvironment.m_typeSystem, testDomain::TestGenericType::getThisIdentifier());

    valueTree.setToDefault();

    EXPECT_THROW(data.apply(&valueTree), babelwires::ModelException);
}

TEST(SetTypeVariableModifierDataTest, clone) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::SetTypeVariableModifierData data;
    data.m_targetPath = babelwires::Path::deserializeFromString("foo/bar/boo");
    data.m_typeAssignments.resize(2);
    data.m_typeAssignments[0] = babelwires::StringType::getThisIdentifier();
    data.m_typeAssignments[1] = babelwires::DefaultIntType::getThisIdentifier();

    auto clonePtr = data.clone();
    ASSERT_NE(clonePtr, nullptr);
    EXPECT_EQ(clonePtr->m_targetPath, data.m_targetPath);
    ASSERT_EQ(clonePtr->m_typeAssignments.size(), data.m_typeAssignments.size());
    for (size_t i = 0; i < data.m_typeAssignments.size(); ++i) {
        EXPECT_EQ(clonePtr->m_typeAssignments[i], data.m_typeAssignments[i]);
    }
}

TEST(SetTypeVariableModifierDataTest, serialize) {
    std::string serializedContents;
    {
        testUtils::TestEnvironment testEnvironment;

        babelwires::SetTypeVariableModifierData data;
        data.m_targetPath = babelwires::Path::deserializeFromString("foo/bar/boo");
        data.m_typeAssignments.resize(2);
        data.m_typeAssignments[0] = babelwires::StringType::getThisIdentifier();
        data.m_typeAssignments[1] = babelwires::DefaultIntType::getThisIdentifier();

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::SetTypeVariableModifierData>();
    deserializer.finalize();

    EXPECT_EQ(dataPtr->m_targetPath, babelwires::Path::deserializeFromString("foo/bar/boo"));
    ASSERT_EQ(dataPtr->m_typeAssignments.size(), 2);
    EXPECT_EQ(dataPtr->m_typeAssignments[0], babelwires::StringType::getThisIdentifier());
    EXPECT_EQ(dataPtr->m_typeAssignments[1], babelwires::DefaultIntType::getThisIdentifier());
}
