#include <gtest/gtest.h>

#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Types/Int/intFeature.hpp>
#include <BabelWiresLib/Types/Rational/rationalFeature.hpp>
#include <BabelWiresLib/Types/String/stringFeature.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRootedFeature.hpp>

#include <Tests/TestUtils/testLog.hpp>

TEST(ModifierDataTest, arrayInitializationApply) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ArraySizeModifierData data;
    data.m_size = 5;

    testUtils::RootedFeature<babelwires::StandardArrayFeature<babelwires::IntFeature>> rootFeature(
        testEnvironment.m_projectContext);
    babelwires::ArrayFeature& arrayFeature = rootFeature.getFeature();

    EXPECT_EQ(arrayFeature.getNumFeatures(), 0);

    data.apply(&arrayFeature);
    EXPECT_EQ(arrayFeature.getNumFeatures(), 5);

    babelwires::IntFeature notArrayFeature;
    EXPECT_THROW(data.apply(&notArrayFeature), babelwires::ModelException);
}

TEST(ModifierDataTest, arrayInitializationClone) {
    babelwires::ArraySizeModifierData data;
    data.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("aa/bb/6");
    data.m_size = 5;
    auto dataPtr = data.clone();
    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::FeaturePath::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_size, 5);
}

TEST(ModifierDataTest, arrayInitializationSerialization) {
    std::string serializedContents;
    {
        babelwires::ArraySizeModifierData data;
        data.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("aa/bb/6");
        data.m_size = 5;

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::ArraySizeModifierData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::FeaturePath::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_size, 5);
}

/*
TEST(ModifierDataTest, rationalValueAssignmentApply) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);
    babelwires::RationalFeature& ratFeature = *rootFeature.addField(std::make_unique<babelwires::RationalFeature>(),
                                                                    testUtils::getTestRegisteredIdentifier("foo"));

    babelwires::RationalValueAssignmentData data;
    data.m_value = babelwires::Rational(19 / 57);

    EXPECT_EQ(ratFeature.get(), 0);

    data.apply(&ratFeature);
    EXPECT_EQ(ratFeature.get(), babelwires::Rational(19 / 57));

    babelwires::IntFeature& notRatFeature = *rootFeature.addField(std::make_unique<babelwires::IntFeature>(),
                                                                  testUtils::getTestRegisteredIdentifier("bar"));
    EXPECT_THROW(data.apply(&notRatFeature), babelwires::ModelException);
}

TEST(ModifierDataTest, rationalValueAssignmentClone) {
    babelwires::RationalValueAssignmentData data;
    data.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("aa/bb/6");
    data.m_value = babelwires::Rational(19 / 57);
    auto dataPtr = data.clone();
    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::FeaturePath::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_value, babelwires::Rational(19 / 57));
}

TEST(ModifierDataTest, rationalValueAssignmentSerialization) {
    std::string serializedContents;
    {
        babelwires::RationalValueAssignmentData data;
        data.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("aa/bb/6");
        data.m_value = babelwires::Rational(19 / 57);

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::RationalValueAssignmentData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::FeaturePath::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_value, babelwires::Rational(19 / 57));
}
*/

TEST(ModifierDataTest, assignFromFeatureApply) {
    babelwires::ConnectionModifierData data;

    testUtils::TestEnvironment testEnvironment;

    babelwires::RootFeature srcRootFeature(testEnvironment.m_projectContext);
    babelwires::IntFeature& srcIntFeature = *srcRootFeature.addField(std::make_unique<babelwires::IntFeature>(),
                                                                     testUtils::getTestRegisteredIdentifier("foo"));

    babelwires::RootFeature targetRootFeature(testEnvironment.m_projectContext);
    babelwires::IntFeature& targetIntFeature = *targetRootFeature.addField(
        std::make_unique<babelwires::IntFeature>(), testUtils::getTestRegisteredIdentifier("foo"));

    srcIntFeature.set(14);
    targetIntFeature.set(0);
    EXPECT_EQ(targetIntFeature.get(), 0);

    data.apply(&srcIntFeature, &targetIntFeature, true);
    EXPECT_EQ(targetIntFeature.get(), 14);

    targetIntFeature.set(0);
    srcIntFeature.clearChanges();
    data.apply(&srcIntFeature, &targetIntFeature, false);
    EXPECT_EQ(targetIntFeature.get(), 0);

    srcIntFeature.set(28);
    data.apply(&srcIntFeature, &targetIntFeature, false);
    EXPECT_EQ(targetIntFeature.get(), 28);

    babelwires::StringFeature& notIntFeature = *srcRootFeature.addField(std::make_unique<babelwires::StringFeature>(),
                                                                        testUtils::getTestRegisteredIdentifier("bar"));
    EXPECT_THROW(data.apply(&srcIntFeature, &notIntFeature, true), babelwires::ModelException);

    // An exception will try to print out a path, which will expect one of these singletons.

    babelwires::StandardArrayFeature<babelwires::IntFeature> notValueFeature;
    EXPECT_THROW(data.apply(&notValueFeature, &targetIntFeature, true), babelwires::ModelException);
    EXPECT_THROW(data.apply(&srcIntFeature, &notValueFeature, true), babelwires::ModelException);
}

TEST(ModifierDataTest, assignFromFeatureClone) {
    babelwires::ConnectionModifierData data;
    data.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("aa/bb/6");
    data.m_pathToSourceFeature = babelwires::FeaturePath::deserializeFromString("10/ee/ff");
    auto dataPtr = data.clone();
    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::FeaturePath::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_pathToSourceFeature, babelwires::FeaturePath::deserializeFromString("10/ee/ff"));
}

TEST(ModifierDataTest, assignFromFeatureSerialization) {
    std::string serializedContents;
    {
        babelwires::ConnectionModifierData data;
        data.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("aa/bb/6");
        data.m_pathToSourceFeature = babelwires::FeaturePath::deserializeFromString("10/ee/ff");

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::ConnectionModifierData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::FeaturePath::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_pathToSourceFeature, babelwires::FeaturePath::deserializeFromString("10/ee/ff"));
}

TEST(ModifierDataTest, createModifierMethods) {
    {
        babelwires::ArraySizeModifierData data;
        ASSERT_NE(data.createModifier(), nullptr);
        EXPECT_NE(data.createModifier()->as<babelwires::ArraySizeModifier>(), nullptr);
    }
    {
        babelwires::ValueAssignmentData data(babelwires::IntValue(12));
        ASSERT_NE(data.createModifier(), nullptr);
        EXPECT_NE(data.createModifier()->as<babelwires::LocalModifier>(), nullptr);
    }
    {
        babelwires::ConnectionModifierData data;
        ASSERT_NE(data.createModifier(), nullptr);
        EXPECT_NE(data.createModifier()->as<babelwires::ConnectionModifier>(), nullptr);
    }
}
