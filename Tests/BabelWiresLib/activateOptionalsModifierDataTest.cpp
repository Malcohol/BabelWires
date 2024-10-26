#include <gtest/gtest.h>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(ActivateOptionalsModifierDataTest, apply) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ValueTreeRoot valueFeature(testEnvironment.m_projectContext.m_typeSystem,
                                                testUtils::TestComplexRecordType::getThisIdentifier());
    valueFeature.setToDefault();
    const auto* type = valueFeature.getType().as<testUtils::TestComplexRecordType>();

    EXPECT_FALSE(type->isActivated(valueFeature.getValue(), testUtils::TestComplexRecordType::getOpIntId()));
    EXPECT_FALSE(type->isActivated(valueFeature.getValue(), testUtils::TestComplexRecordType::getOpRecId()));

    babelwires::ActivateOptionalsModifierData data;
    data.m_selectedOptionals.emplace_back(testUtils::TestComplexRecordType::getOpIntId());

    data.apply(&valueFeature);

    EXPECT_TRUE(type->isActivated(valueFeature.getValue(), testUtils::TestComplexRecordType::getOpIntId()));
    EXPECT_FALSE(type->isActivated(valueFeature.getValue(), testUtils::TestComplexRecordType::getOpRecId()));

    babelwires::ActivateOptionalsModifierData data2;
    data2.m_selectedOptionals.emplace_back(testUtils::TestComplexRecordType::getOpRecId());

    data2.apply(&valueFeature);

    EXPECT_FALSE(type->isActivated(valueFeature.getValue(), testUtils::TestComplexRecordType::getOpIntId()));
    EXPECT_TRUE(type->isActivated(valueFeature.getValue(), testUtils::TestComplexRecordType::getOpRecId()));
}

TEST(ActivateOptionalsModifierDataTest, failureNotOptionals) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ValueTreeRoot valueFeature(testEnvironment.m_projectContext.m_typeSystem,
                                                testUtils::TestComplexRecordType::getThisIdentifier());
    valueFeature.setToDefault();
    const auto* type = valueFeature.getType().as<testUtils::TestComplexRecordType>();

    babelwires::ValueHolder before = valueFeature.getValue();

    babelwires::ActivateOptionalsModifierData data;
    data.m_selectedOptionals.emplace_back(testUtils::TestComplexRecordType::getOpIntId());
    data.m_selectedOptionals.emplace_back("foo");

    EXPECT_THROW(data.apply(&valueFeature), babelwires::ModelException);

    EXPECT_EQ(before, valueFeature.getValue());
}

TEST(ActivateOptionalsModifierDataTest, failureNotARecordWithOptionals) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ValueTreeRoot valueFeature(testEnvironment.m_projectContext.m_typeSystem,
                                                testUtils::TestSimpleRecordType::getThisIdentifier());
    valueFeature.setToDefault();

    babelwires::ActivateOptionalsModifierData data;
    data.m_selectedOptionals.emplace_back("op");

    EXPECT_THROW(data.apply(&valueFeature), babelwires::ModelException);
}

TEST(ActivateOptionalsModifierDataTest, clone) {
    babelwires::ActivateOptionalsModifierData data;
    data.m_pathToFeature = babelwires::Path::deserializeFromString("foo/bar/boo");
    data.m_selectedOptionals.emplace_back("op0");
    data.m_selectedOptionals.emplace_back("op1");

    auto clonePtr = data.clone();
    ASSERT_NE(clonePtr, nullptr);
    EXPECT_EQ(clonePtr->m_pathToFeature, data.m_pathToFeature);
    EXPECT_TRUE(testUtils::areEqualSets(clonePtr->m_selectedOptionals, data.m_selectedOptionals));
}

TEST(ActivateOptionalsModifierDataTest, serialization) {
    std::string serializedContents;
    {
        babelwires::ActivateOptionalsModifierData data;
        data.m_pathToFeature = babelwires::Path::deserializeFromString("foo/bar/boo");
        data.m_selectedOptionals.emplace_back("op0");
        data.m_selectedOptionals.emplace_back("op1");

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::ActivateOptionalsModifierData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::Path::deserializeFromString("foo/bar/boo"));
    EXPECT_TRUE(testUtils::areEqualSets(dataPtr->m_selectedOptionals, {"op0", "op1"}));
}
