#include <gtest/gtest.h>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/recordWithOptionalsFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Types/Int/intFeature.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRootedFeature.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(ActivateOptionalsModifierDataTest, apply) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::RootedFeature<babelwires::RecordWithOptionalsFeature> rootedFeature(testEnvironment.m_projectContext);
    babelwires::RecordWithOptionalsFeature& recordFeature = rootedFeature.getFeature();

    babelwires::ActivateOptionalsModifierData data;
    data.m_selectedOptionals.emplace_back("op0");
    data.m_selectedOptionals.emplace_back("op1");

    babelwires::ShortId op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op0);

    babelwires::ShortId ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::ShortId op1("op1");
    op1.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature1 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op1);

    babelwires::ShortId op2("op2");
    op2.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature2 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op2);

    recordFeature.activateField("op1");
    recordFeature.activateField("op2");

    data.apply(&recordFeature);

    EXPECT_TRUE(recordFeature.isActivated("op0"));
    EXPECT_TRUE(recordFeature.isActivated("op1"));
    EXPECT_FALSE(recordFeature.isActivated("op2"));
}

TEST(ActivateOptionalsModifierDataTest, apply1) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::RootedFeature<babelwires::RecordWithOptionalsFeature> rootedFeature(testEnvironment.m_projectContext);
    babelwires::RecordWithOptionalsFeature& recordFeature = rootedFeature.getFeature();

    babelwires::ActivateOptionalsModifierData data;
    data.m_selectedOptionals.emplace_back("op0");
    data.m_selectedOptionals.emplace_back("op2");
    data.m_selectedOptionals.emplace_back("op5");

    babelwires::ShortId op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op0);

    babelwires::ShortId ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::ShortId op1("op1");
    op1.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature1 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op1);

    babelwires::ShortId op2("op2");
    op2.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature2 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op2);

    recordFeature.activateField("op0");
    recordFeature.activateField("op1");

    data.apply(&recordFeature);

    EXPECT_TRUE(recordFeature.isActivated("op0"));
    EXPECT_FALSE(recordFeature.isActivated("op1"));
    EXPECT_TRUE(recordFeature.isActivated("op2"));
}

TEST(ActivateOptionalsModifierDataTest, failureNotOptionals) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::RootedFeature<babelwires::RecordWithOptionalsFeature> rootedFeature(testEnvironment.m_projectContext);
    babelwires::RecordWithOptionalsFeature& recordFeature = rootedFeature.getFeature();

    babelwires::ActivateOptionalsModifierData data;
    data.m_selectedOptionals.emplace_back("op5");
    data.m_selectedOptionals.emplace_back("op6");

    babelwires::ShortId op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op0);

    babelwires::ShortId ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    EXPECT_THROW(data.apply(&recordFeature), babelwires::ModelException);
}

TEST(ActivateOptionalsModifierDataTest, failureNotARecordWithOptionals) {
    babelwires::ActivateOptionalsModifierData data;
    data.m_selectedOptionals.emplace_back("op");

    babelwires::IntFeature notARecordWithOptionals;

    EXPECT_THROW(data.apply(&notARecordWithOptionals), babelwires::ModelException);
}

TEST(ActivateOptionalsModifierDataTest, clone) {
    babelwires::ActivateOptionalsModifierData data;
    data.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("foo/bar/boo");
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
        data.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("foo/bar/boo");
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
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::FeaturePath::deserializeFromString("foo/bar/boo"));
    EXPECT_TRUE(testUtils::areEqualSets(dataPtr->m_selectedOptionals, {"op0", "op1"}));
}
