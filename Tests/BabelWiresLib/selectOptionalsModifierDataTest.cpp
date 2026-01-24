#include <gtest/gtest.h>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Project/Modifiers/selectOptionalsModifierData.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>
#include <BaseLib/Serialization/XML/xmlDeserializer.hpp>
#include <BaseLib/Serialization/XML/xmlSerializer.hpp>

#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(SelectOptionalsModifierDataTest, apply) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ValueTreeRoot valueFeature(testEnvironment.m_projectContext.m_typeSystem,
                                                testDomain::TestComplexRecordType::getThisIdentifier());
    valueFeature.setToDefault();
    const auto* type = valueFeature.getType()->tryAs<testDomain::TestComplexRecordType>();

    EXPECT_FALSE(type->isActivated(valueFeature.getValue(), testDomain::TestComplexRecordType::getOpIntId()));
    EXPECT_FALSE(type->isActivated(valueFeature.getValue(), testDomain::TestComplexRecordType::getOpRecId()));

    babelwires::SelectOptionalsModifierData data;
    data.setOptionalActivation(testDomain::TestComplexRecordType::getOpIntId(), true);

    data.apply(&valueFeature);

    EXPECT_TRUE(type->isActivated(valueFeature.getValue(), testDomain::TestComplexRecordType::getOpIntId()));
    EXPECT_FALSE(type->isActivated(valueFeature.getValue(), testDomain::TestComplexRecordType::getOpRecId()));

    babelwires::SelectOptionalsModifierData data2;
    data2.setOptionalActivation(testDomain::TestComplexRecordType::getOpRecId(), true);

    data2.apply(&valueFeature);

    EXPECT_FALSE(type->isActivated(valueFeature.getValue(), testDomain::TestComplexRecordType::getOpIntId()));
    EXPECT_TRUE(type->isActivated(valueFeature.getValue(), testDomain::TestComplexRecordType::getOpRecId()));
}

TEST(SelectOptionalsModifierDataTest, failureNotOptionals) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ValueTreeRoot valueFeature(testEnvironment.m_projectContext.m_typeSystem,
                                                testDomain::TestComplexRecordType::getThisIdentifier());
    valueFeature.setToDefault();
    const auto* type = valueFeature.getType()->tryAs<testDomain::TestComplexRecordType>();

    babelwires::ValueHolder before = valueFeature.getValue();

    babelwires::SelectOptionalsModifierData data;
    data.setOptionalActivation(testDomain::TestComplexRecordType::getOpIntId(), true);
    data.setOptionalActivation("foo", true);

    EXPECT_THROW(data.apply(&valueFeature), babelwires::ModelException);

    EXPECT_EQ(before, valueFeature.getValue());
}

TEST(SelectOptionalsModifierDataTest, failureNotARecordWithOptionals) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ValueTreeRoot valueFeature(testEnvironment.m_projectContext.m_typeSystem,
                                                testDomain::TestSimpleRecordType::getThisIdentifier());
    valueFeature.setToDefault();

    babelwires::SelectOptionalsModifierData data;
    data.setOptionalActivation("op", true);

    EXPECT_THROW(data.apply(&valueFeature), babelwires::ModelException);
}

TEST(SelectOptionalsModifierDataTest, clone) {
    babelwires::SelectOptionalsModifierData data;
    data.m_targetPath = babelwires::Path::deserializeFromString("foo/bar/boo");
    data.setOptionalActivation("op0", true);
    data.setOptionalActivation("op1", false);

    auto clonePtr = data.clone();
    ASSERT_NE(clonePtr, nullptr);
    EXPECT_EQ(clonePtr->m_targetPath, data.m_targetPath);

    EXPECT_TRUE(clonePtr->getOptionalActivationData().size() == 2);
    EXPECT_EQ(clonePtr->getOptionalActivationData().at("op0"), true);
    EXPECT_EQ(clonePtr->getOptionalActivationData().at("op1"), false);
}

TEST(SelectOptionalsModifierDataTest, serialization) {
    std::string serializedContents;
    {
        babelwires::SelectOptionalsModifierData data;
        data.m_targetPath = babelwires::Path::deserializeFromString("foo/bar/boo");
        data.setOptionalActivation("op0", true);
        data.setOptionalActivation("op1", false);

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::SelectOptionalsModifierData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_targetPath, babelwires::Path::deserializeFromString("foo/bar/boo"));
    EXPECT_TRUE(dataPtr->getOptionalActivationData().size() == 2);
    EXPECT_EQ(dataPtr->getOptionalActivationData().at("op0"), true);
    EXPECT_EQ(dataPtr->getOptionalActivationData().at("op1"), false);
}
