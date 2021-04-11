#include <gtest/gtest.h>

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Features/arrayFeature.hpp"
#include "BabelWires/Features/numericFeature.hpp"
#include "BabelWires/Features/stringFeature.hpp"
#include "BabelWires/Project/Modifiers/arraySizeModifier.hpp"
#include "BabelWires/Project/Modifiers/connectionModifier.hpp"
#include "BabelWires/Project/Modifiers/localModifier.hpp"
#include "BabelWires/Project/Modifiers/modifierData.hpp"

#include "Common/Serialization/XML/xmlDeserializer.hpp"
#include "Common/Serialization/XML/xmlSerializer.hpp"

#include "Tests/TestUtils/testLog.hpp"

TEST(ModifierDataTest, arrayInitializationApply) {
    babelwires::ArrayInitializationData data;
    data.m_size = 5;

    babelwires::StandardArrayFeature<babelwires::IntFeature> arrayFeature;
    EXPECT_EQ(arrayFeature.getNumFeatures(), 0);

    data.apply(&arrayFeature);
    EXPECT_EQ(arrayFeature.getNumFeatures(), 5);

    babelwires::IntFeature notArrayFeature;
    EXPECT_THROW(data.apply(&notArrayFeature), babelwires::ModelException);
}

TEST(ModifierDataTest, arrayInitializationClone) {
    babelwires::ArrayInitializationData data;
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
        babelwires::ArrayInitializationData data;
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
    auto dataPtr = deserializer.deserializeObject<babelwires::ArrayInitializationData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::FeaturePath::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_size, 5);
}

TEST(ModifierDataTest, intValueAssignmentApply) {
    babelwires::IntValueAssignmentData data;
    data.m_value = 198;

    babelwires::IntFeature intFeature;
    EXPECT_EQ(intFeature.get(), 0);

    data.apply(&intFeature);
    EXPECT_EQ(intFeature.get(), 198);

    babelwires::StringFeature notIntFeature;
    EXPECT_THROW(data.apply(&notIntFeature), babelwires::ModelException);
}

TEST(ModifierDataTest, intValueAssignmentClone) {
    babelwires::IntValueAssignmentData data;
    data.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("aa/bb/6");
    data.m_value = 198;
    auto dataPtr = data.clone();
    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::FeaturePath::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_value, 198);
}

TEST(ModifierDataTest, intValueAssignmentSerialization) {
    std::string serializedContents;
    {
        babelwires::IntValueAssignmentData data;
        data.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("aa/bb/6");
        data.m_value = 198;

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::IntValueAssignmentData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::FeaturePath::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_value, 198);
}

TEST(ModifierDataTest, rationalValueAssignmentApply) {
    babelwires::RationalValueAssignmentData data;
    data.m_value = babelwires::Rational(19 / 57);

    babelwires::RationalFeature ratFeature;
    EXPECT_EQ(ratFeature.get(), 0);

    data.apply(&ratFeature);
    EXPECT_EQ(ratFeature.get(), babelwires::Rational(19 / 57));

    babelwires::StringFeature notRatFeature;
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

TEST(ModifierDataTest, stringValueAssignmentApply) {
    babelwires::StringValueAssignmentData data;
    data.m_value = "Hello";

    babelwires::StringFeature stringFeature;
    EXPECT_EQ(stringFeature.get(), "");

    data.apply(&stringFeature);
    EXPECT_EQ(stringFeature.get(), "Hello");

    babelwires::IntFeature notStringFeature;
    EXPECT_THROW(data.apply(&notStringFeature), babelwires::ModelException);
}

TEST(ModifierDataTest, stringValueAssignmentClone) {
    babelwires::StringValueAssignmentData data;
    data.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("aa/bb/6");
    data.m_value = "Hello";
    auto dataPtr = data.clone();
    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::FeaturePath::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_value, "Hello");
}

TEST(ModifierDataTest, stringValueAssignmentSerialization) {
    std::string serializedContents;
    {
        babelwires::StringValueAssignmentData data;
        data.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("aa/bb/6");
        data.m_value = "Hello";

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::StringValueAssignmentData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::FeaturePath::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_value, "Hello");
}

TEST(ModifierDataTest, assignFromFeatureApply) {
    babelwires::AssignFromFeatureData data;

    babelwires::IntFeature srcIntFeature;
    srcIntFeature.set(14);

    babelwires::IntFeature targetIntFeature;
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

    babelwires::StringFeature notIntFeature;
    EXPECT_THROW(data.apply(&srcIntFeature, &notIntFeature, true), babelwires::ModelException);

    // An exception will try to print out a path, which will expect one of these singletons.
    babelwires::FieldNameRegistryScope fieldNameRegistryScope;

    babelwires::StandardArrayFeature<babelwires::IntFeature> notValueFeature;
    EXPECT_THROW(data.apply(&notValueFeature, &targetIntFeature, true), babelwires::ModelException);
    EXPECT_THROW(data.apply(&srcIntFeature, &notValueFeature, true), babelwires::ModelException);
}

TEST(ModifierDataTest, assignFromFeatureClone) {
    babelwires::AssignFromFeatureData data;
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
        babelwires::AssignFromFeatureData data;
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
    auto dataPtr = deserializer.deserializeObject<babelwires::AssignFromFeatureData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::FeaturePath::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_pathToSourceFeature, babelwires::FeaturePath::deserializeFromString("10/ee/ff"));
}

TEST(ModifierDataTest, createModifierMethods) {
    {
        babelwires::ArrayInitializationData data;
        ASSERT_NE(data.createModifier(), nullptr);
        EXPECT_NE(dynamic_cast<babelwires::ArraySizeModifier*>(data.createModifier().get()), nullptr);
    }
    {
        babelwires::IntValueAssignmentData data;
        ASSERT_NE(data.createModifier(), nullptr);
        EXPECT_NE(dynamic_cast<babelwires::LocalModifier*>(data.createModifier().get()), nullptr);
    }
    {
        babelwires::RationalValueAssignmentData data;
        ASSERT_NE(data.createModifier(), nullptr);
        EXPECT_NE(dynamic_cast<babelwires::LocalModifier*>(data.createModifier().get()), nullptr);
    }
    {
        babelwires::StringValueAssignmentData data;
        ASSERT_NE(data.createModifier(), nullptr);
        EXPECT_NE(dynamic_cast<babelwires::LocalModifier*>(data.createModifier().get()), nullptr);
    }
    {
        babelwires::AssignFromFeatureData data;
        ASSERT_NE(data.createModifier(), nullptr);
        EXPECT_NE(dynamic_cast<babelwires::ConnectionModifier*>(data.createModifier().get()), nullptr);
    }
}
