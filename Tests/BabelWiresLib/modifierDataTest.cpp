#include <gtest/gtest.h>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>
#include <BaseLib/Serialization/XML/xmlDeserializer.hpp>
#include <BaseLib/Serialization/XML/xmlSerializer.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Domains/TestDomain/testRecordType.hpp>
#include <Domains/TestDomain/testArrayType.hpp>

#include <Tests/TestUtils/testLog.hpp>

TEST(ModifierDataTest, arrayInitializationApply) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ArraySizeModifierData data;
    data.m_size = testDomain::TestSimpleArrayType::s_nonDefaultSize;

    babelwires::ValueTreeRoot arrayFeature(testEnvironment.m_typeSystem, testDomain::TestSimpleArrayType::getThisIdentifier());
    arrayFeature.setToDefault();

    EXPECT_EQ(arrayFeature.getNumChildren(), testDomain::TestSimpleArrayType::s_defaultSize);

    data.apply(&arrayFeature);
    EXPECT_EQ(arrayFeature.getNumChildren(), testDomain::TestSimpleArrayType::s_nonDefaultSize);

    babelwires::ValueTreeRoot notArrayFeature(testEnvironment.m_typeSystem, babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_THROW(data.apply(&notArrayFeature), babelwires::ModelException);
}

TEST(ModifierDataTest, arrayInitializationClone) {
    babelwires::ArraySizeModifierData data;
    data.m_targetPath = *babelwires::Path::deserializeFromString("aa/bb/6");
    data.m_size = 5;
    auto dataPtr = data.clone();
    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_targetPath, *babelwires::Path::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_size, 5);
}

TEST(ModifierDataTest, arrayInitializationSerialization) {
    std::string serializedContents;
    {
        babelwires::ArraySizeModifierData data;
        data.m_targetPath = *babelwires::Path::deserializeFromString("aa/bb/6");
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
    auto dataPtrResult = deserializer.deserializeObject<babelwires::ArraySizeModifierData>();
    ASSERT_TRUE(dataPtrResult);
    auto dataPtr = std::move(*dataPtrResult);
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_targetPath, *babelwires::Path::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_size, 5);
}

TEST(ModifierDataTest, assignFromFeatureApply) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot srcFeature{testEnvironment.m_typeSystem,
                                              babelwires::DefaultIntType::getThisIdentifier()};
    babelwires::ValueTreeRoot targetFeature{testEnvironment.m_typeSystem,
                                                 babelwires::DefaultIntType::getThisIdentifier()};

    srcFeature.setToDefault();
    targetFeature.setToDefault();

    babelwires::Instance<babelwires::IntType> srcInstance{srcFeature};
    babelwires::Instance<babelwires::IntType> targetInstance{targetFeature};
    srcInstance.set(14);
    targetInstance.set(0);

    // This test is not about following paths, so the paths can be left default.
    babelwires::ConnectionModifierData data;

    EXPECT_EQ(targetInstance.get(), 0);

    data.apply(&srcFeature, &targetFeature, false);
    EXPECT_EQ(targetInstance.get(), 14);

    // Test that the target only gets updated when the source has changes.
    targetInstance.set(0);
    srcFeature.clearChanges();
    data.apply(&srcFeature, &targetFeature, false);
    EXPECT_EQ(targetInstance.get(), 0);

    // Unless the third argument is true.
    data.apply(&srcFeature, &targetFeature, true);
    EXPECT_EQ(targetInstance.get(), 14);

    // Respond to changes as normal.
    targetInstance.set(28);
    data.apply(&srcFeature, &targetFeature, false);
    EXPECT_EQ(targetInstance.get(), 28);
}

TEST(ModifierDataTest, assignFromFeatureBadConnectionApply) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot srcFeature{testEnvironment.m_typeSystem,
                                              babelwires::DefaultIntType::getThisIdentifier()};
    babelwires::ValueTreeRoot targetFeature{testEnvironment.m_typeSystem,
                                                 babelwires::StringType::getThisIdentifier()};

    srcFeature.setToDefault();
    targetFeature.setToDefault();

    babelwires::ConnectionModifierData data;

    EXPECT_THROW(data.apply(&srcFeature, &targetFeature, false), babelwires::ModelException);
}

TEST(ModifierDataTest, assignFromFeatureClone) {
    babelwires::ConnectionModifierData data;
    data.m_targetPath = *babelwires::Path::deserializeFromString("aa/bb/6");
    data.m_sourcePath = *babelwires::Path::deserializeFromString("10/ee/ff");
    auto dataPtr = data.clone();
    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_targetPath, *babelwires::Path::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_sourcePath, *babelwires::Path::deserializeFromString("10/ee/ff"));
}

TEST(ModifierDataTest, assignFromFeatureSerialization) {
    std::string serializedContents;
    {
        babelwires::ConnectionModifierData data;
        data.m_targetPath = *babelwires::Path::deserializeFromString("aa/bb/6");
        data.m_sourcePath = *babelwires::Path::deserializeFromString("10/ee/ff");

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtrResult = deserializer.deserializeObject<babelwires::ConnectionModifierData>();
    ASSERT_TRUE(dataPtrResult);
    auto dataPtr = std::move(*dataPtrResult);
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_targetPath, *babelwires::Path::deserializeFromString("aa/bb/6"));
    EXPECT_EQ(dataPtr->m_sourcePath, *babelwires::Path::deserializeFromString("10/ee/ff"));
}

TEST(ModifierDataTest, createModifierMethods) {
    {
        babelwires::ArraySizeModifierData data;
        ASSERT_NE(data.createModifier(), nullptr);
        EXPECT_NE(data.createModifier()->tryAs<babelwires::ArraySizeModifier>(), nullptr);
    }
    {
        babelwires::ValueAssignmentData data(babelwires::IntValue(12));
        ASSERT_NE(data.createModifier(), nullptr);
        EXPECT_NE(data.createModifier()->tryAs<babelwires::LocalModifier>(), nullptr);
    }
    {
        babelwires::ConnectionModifierData data;
        ASSERT_NE(data.createModifier(), nullptr);
        EXPECT_NE(data.createModifier()->tryAs<babelwires::ConnectionModifier>(), nullptr);
    }
}
