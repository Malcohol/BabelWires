#include <gtest/gtest.h>

#include <BabelWiresLib/Features/recordFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/SourceFileElement/sourceFileElementData.hpp>
#include <BabelWiresLib/Project/FeatureElements/TargetFileElement/targetFileElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Features/simpleValueFeature.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

namespace {
    struct TestOwner : babelwires::FeatureElement {
        TestOwner()
            : FeatureElement(babelwires::SourceFileElementData(), 0) {}

        void doProcess(babelwires::UserLogger& userLogger) override {}
    };
} // namespace

TEST(ModifierTest, basicStuff) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/bb");

    auto intModData = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(198));
    intModData->m_pathToFeature = path;

    babelwires::LocalModifier intMod(std::move(intModData));

    EXPECT_EQ(intMod.asConnectionModifier(), nullptr);
    EXPECT_EQ(intMod.getModifierData().m_pathToFeature, path);
    EXPECT_EQ(intMod.getPathToFeature(), path);
    EXPECT_NE(intMod.getModifierData().as<babelwires::ValueAssignmentData>(), nullptr);
    EXPECT_NE(static_cast<const babelwires::ValueAssignmentData&>(intMod.getModifierData())
                  .getValue()
                  ->as<babelwires::IntValue>(),
              nullptr);
    EXPECT_EQ(static_cast<const babelwires::ValueAssignmentData&>(intMod.getModifierData())
                  .getValue()
                  ->as<babelwires::IntValue>()
                  ->get(),
              198);

    EXPECT_EQ(const_cast<const babelwires::LocalModifier&>(intMod).getOwner(), nullptr);
    TestOwner owner;
    intMod.setOwner(&owner);
    EXPECT_EQ(const_cast<const babelwires::LocalModifier&>(intMod).getOwner(), &owner);
}

TEST(ModifierTest, clone) {
    babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/bb");

    auto intModData = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(198));
    intModData->m_pathToFeature = path;

    babelwires::LocalModifier intMod(std::move(intModData));

    auto clone = intMod.clone();
    ASSERT_NE(clone, nullptr);
    EXPECT_EQ(clone->getModifierData().m_pathToFeature, path);
    EXPECT_NE(clone->getModifierData().as<babelwires::ValueAssignmentData>(), nullptr);
    EXPECT_NE(static_cast<const babelwires::ValueAssignmentData&>(clone->getModifierData())
                  .getValue()
                  ->as<babelwires::IntValue>(),
              nullptr);
    EXPECT_EQ(static_cast<const babelwires::ValueAssignmentData&>(clone->getModifierData())
                  .getValue()
                  ->as<babelwires::IntValue>()
                  ->get(),
              198);
}

TEST(ModifierTest, localApplySuccess) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::SimpleValueFeature recordFeature{testEnvironment.m_projectContext.m_typeSystem,
                                                 testUtils::TestSimpleRecordType::getThisIdentifier()};
    recordFeature.setToDefault();

    babelwires::FeaturePath path;
    path.pushStep(babelwires::PathStep(testUtils::TestSimpleRecordType::getInt0Id()));

    auto intModData = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(198));
    intModData->m_pathToFeature = path;

    babelwires::LocalModifier intMod(std::move(intModData));

    testUtils::TestSimpleRecordType::ConstInstance instance{recordFeature};

    EXPECT_EQ(instance.getintR0().get(), 0);
    intMod.applyIfLocal(testEnvironment.m_log, &recordFeature);
    EXPECT_EQ(instance.getintR0().get(), 198);
    EXPECT_FALSE(intMod.isFailed());
    EXPECT_EQ(intMod.getState(), babelwires::Modifier::State::Success);

    intMod.unapply(&recordFeature);
    EXPECT_EQ(instance.getintR0().get(), 0);
}

TEST(ModifierTest, localApplyFailureWrongType) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::SimpleValueFeature recordFeature(testEnvironment.m_typeSystem,
                                                 testUtils::TestSimpleRecordType::getThisIdentifier());
    recordFeature.setToDefault();

    babelwires::FeaturePath path;
    path.pushStep(babelwires::PathStep{testUtils::TestSimpleRecordType::getInt0Id()});

    auto stringModData = std::make_unique<babelwires::ValueAssignmentData>(babelwires::StringValue("Hello"));
    stringModData->m_pathToFeature = path;

    babelwires::LocalModifier stringMod(std::move(stringModData));

    TestOwner owner;
    stringMod.setOwner(&owner);

    testUtils::TestLogWithListener testLog;

    stringMod.applyIfLocal(testLog, &recordFeature);
    EXPECT_TRUE(stringMod.isFailed());
    EXPECT_EQ(stringMod.getState(), babelwires::Modifier::State::ApplicationFailed);
    EXPECT_TRUE(testLog.hasSubstringIgnoreCase("Failed to apply operation"));
}

TEST(ModifierTest, localApplyFailureNoTarget) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::SimpleValueFeature recordFeature{testEnvironment.m_projectContext.m_typeSystem,
                                                 testUtils::TestSimpleRecordType::getThisIdentifier()};
    recordFeature.setToDefault();

    babelwires::FeaturePath path;
    path.pushStep(babelwires::PathStep{testUtils::TestSimpleRecordType::getInt0Id()});
    path.pushStep(12);

    auto intModData = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(198));
    intModData->m_pathToFeature = path;

    babelwires::LocalModifier intMod(std::move(intModData));

    TestOwner owner;
    intMod.setOwner(&owner);

    testUtils::TestLogWithListener testLog;

    // An exception will try to print out a path, which will expect one of these singletons.

    intMod.applyIfLocal(testLog, &recordFeature);
    EXPECT_TRUE(intMod.isFailed());
    EXPECT_EQ(intMod.getState(), babelwires::Modifier::State::TargetMissing);
    EXPECT_TRUE(testLog.hasSubstringIgnoreCase("Failed to apply operation"));
}

TEST(ModifierTest, arraySizeModifierSuccess) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::SimpleValueFeature recordFeature(testEnvironment.m_typeSystem,
                                                 testUtils::TestComplexRecordType::getThisIdentifier());
    recordFeature.setToDefault();

    auto arrayModData = std::make_unique<babelwires::ArraySizeModifierData>();
    arrayModData->m_pathToFeature.pushStep(babelwires::PathStep(testUtils::TestComplexRecordType::getArrayId()));
    arrayModData->m_size = testUtils::TestSimpleArrayType::s_nonDefaultSize;

    babelwires::ArraySizeModifier arrayMod(std::move(arrayModData));

    TestOwner owner;
    arrayMod.setOwner(&owner);

    testUtils::TestComplexRecordType::ConstInstance record(recordFeature);
    EXPECT_EQ(record.getarray().getSize(), testUtils::TestSimpleArrayType::s_defaultSize);

    arrayMod.applyIfLocal(testEnvironment.m_log, &recordFeature);
    EXPECT_FALSE(arrayMod.isFailed());
    EXPECT_EQ(arrayMod.getState(), babelwires::Modifier::State::Success);

    EXPECT_EQ(record.getarray().getSize(), testUtils::TestSimpleArrayType::s_nonDefaultSize);

    EXPECT_TRUE(arrayMod.addArrayEntries(testEnvironment.m_log, &recordFeature, 1, 2));
    EXPECT_EQ(record.getarray().getSize(), testUtils::TestSimpleArrayType::s_nonDefaultSize + 2);

    EXPECT_TRUE(arrayMod.removeArrayEntries(testEnvironment.m_log, &recordFeature, 2, 2));
    EXPECT_EQ(record.getarray().getSize(), testUtils::TestSimpleArrayType::s_nonDefaultSize);
}

TEST(ModifierTest, arraySizeModifierFailure) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::SimpleValueFeature recordFeature(testEnvironment.m_typeSystem,
                                                 testUtils::TestComplexRecordType::getThisIdentifier());
    recordFeature.setToDefault();

    testUtils::TestComplexRecordType::ConstInstance record(recordFeature);
    EXPECT_EQ(record.getarray().getSize(), testUtils::TestSimpleArrayType::s_defaultSize);

    auto arrayModData = std::make_unique<babelwires::ArraySizeModifierData>();
    arrayModData->m_pathToFeature.pushStep(babelwires::PathStep(testUtils::TestComplexRecordType::getArrayId()));
    arrayModData->m_size = testUtils::TestSimpleArrayType::s_maximumSize + 1;

    babelwires::ArraySizeModifier arrayMod(std::move(arrayModData));

    TestOwner owner;
    arrayMod.setOwner(&owner);

    arrayMod.applyIfLocal(testEnvironment.m_log, &recordFeature);
    EXPECT_TRUE(arrayMod.isFailed());
    EXPECT_EQ(arrayMod.getState(), babelwires::Modifier::State::ApplicationFailed);

    EXPECT_EQ(record.getarray().getSize(), testUtils::TestSimpleArrayType::s_defaultSize);

    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("Failed to apply operation"));
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("array"));
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("size"));
}

TEST(ModifierTest, connectionModifierSuccess) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestSimpleRecordElementData elementData;
    const babelwires::FeaturePath sourcePath = testUtils::TestSimpleRecordElementData::getPathToRecordInt0();
    babelwires::ValueAssignmentData sourceData(babelwires::IntValue(100));
    sourceData.m_pathToFeature = sourcePath;
    elementData.m_modifiers.emplace_back(std::make_unique<babelwires::ValueAssignmentData>(sourceData));

    const babelwires::ElementId sourceId = testEnvironment.m_project.addFeatureElement(elementData);

    babelwires::SimpleValueFeature targetRecordFeature(testEnvironment.m_projectContext.m_typeSystem,
                                                       testUtils::TestSimpleRecordType::getThisIdentifier());
    targetRecordFeature.setToDefault();

    babelwires::FeaturePath targetPath;
    targetPath.pushStep(babelwires::PathStep(testUtils::TestSimpleRecordType::getInt0Id()));

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_pathToFeature = targetPath;
    assignFromData->m_pathToSourceFeature = sourcePath;
    assignFromData->m_sourceId = sourceId;

    babelwires::ConnectionModifier connectionMod(std::move(assignFromData));
    TestOwner owner;
    connectionMod.setOwner(&owner);

    testUtils::TestSimpleRecordType::ConstInstance instance{targetRecordFeature};
    EXPECT_EQ(instance.getintR0().get(), 0);

    connectionMod.applyConnection(testEnvironment.m_project, testEnvironment.m_log, &targetRecordFeature);
    EXPECT_EQ(instance.getintR0().get(), 100);
}

TEST(ModifierTest, connectionModifierTargetPathFailure) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::SimpleValueFeature targetRecordFeature(testEnvironment.m_projectContext.m_typeSystem,
                                                       testUtils::TestSimpleRecordType::getThisIdentifier());
    targetRecordFeature.setToDefault();

    const babelwires::FeaturePath sourcePath = babelwires::FeaturePath::deserializeFromString("aa");
    const babelwires::FeaturePath targetPath = babelwires::FeaturePath::deserializeFromString("xx");

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_pathToFeature = targetPath;
    assignFromData->m_pathToSourceFeature = sourcePath;
    assignFromData->m_sourceId = 10;

    babelwires::ConnectionModifier connectionMod(std::move(assignFromData));
    TestOwner owner;
    connectionMod.setOwner(&owner);

    testUtils::TestSimpleRecordType::ConstInstance instance{targetRecordFeature};
    EXPECT_EQ(instance.getintR0().get(), 0);

    connectionMod.applyConnection(testEnvironment.m_project, testEnvironment.m_log, &targetRecordFeature);
    EXPECT_TRUE(connectionMod.isFailed());
    EXPECT_EQ(connectionMod.getState(), babelwires::Modifier::State::TargetMissing);
    EXPECT_EQ(instance.getintR0().get(), 0);
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("Failed to apply operation"));
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("xx"));
}

TEST(ModifierTest, connectionModifierSourceIdFailure) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::SimpleValueFeature targetRecordFeature(testEnvironment.m_projectContext.m_typeSystem,
                                                       testUtils::TestSimpleRecordType::getThisIdentifier());
    targetRecordFeature.setToDefault();

    const babelwires::FeaturePath sourcePath = babelwires::FeaturePath::deserializeFromString("aa");
    babelwires::FeaturePath targetPath;
    targetPath.pushStep(babelwires::PathStep(testUtils::TestSimpleRecordType::getInt0Id()));

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_pathToFeature = targetPath;
    assignFromData->m_pathToSourceFeature = sourcePath;
    assignFromData->m_sourceId = 99;

    babelwires::ConnectionModifier connectionMod(std::move(assignFromData));
    TestOwner owner;
    connectionMod.setOwner(&owner);

    testUtils::TestSimpleRecordType::ConstInstance instance{targetRecordFeature};
    EXPECT_EQ(instance.getintR0().get(), 0);

    connectionMod.applyConnection(testEnvironment.m_project, testEnvironment.m_log, &targetRecordFeature);
    EXPECT_TRUE(connectionMod.isFailed());
    EXPECT_EQ(connectionMod.getState(), babelwires::Modifier::State::SourceMissing);
    EXPECT_EQ(instance.getintR0().get(), 0);
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("Failed to apply operation"));
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("source"));
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("99"));
}

TEST(ModifierTest, connectionModifierSourcePathFailure) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestSimpleRecordElementData elementData;
    const babelwires::FeaturePath sourcePath = testUtils::TestSimpleRecordElementData::getPathToRecordInt0();
    babelwires::ValueAssignmentData sourceData(babelwires::IntValue(100));
    sourceData.m_pathToFeature = sourcePath;
    elementData.m_modifiers.emplace_back(std::make_unique<babelwires::ValueAssignmentData>(sourceData));

    const babelwires::ElementId sourceId = testEnvironment.m_project.addFeatureElement(elementData);

    babelwires::SimpleValueFeature targetRecordFeature(testEnvironment.m_projectContext.m_typeSystem,
                                                       testUtils::TestSimpleRecordType::getThisIdentifier());
    targetRecordFeature.setToDefault();

    babelwires::FeaturePath targetPath;
    targetPath.pushStep(babelwires::PathStep(testUtils::TestSimpleRecordType::getInt0Id()));

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_pathToFeature = targetPath;
    assignFromData->m_pathToSourceFeature = babelwires::FeaturePath::deserializeFromString("xx");
    assignFromData->m_sourceId = sourceId;

    babelwires::ConnectionModifier connectionMod(std::move(assignFromData));
    TestOwner owner;
    connectionMod.setOwner(&owner);

    testUtils::TestSimpleRecordType::ConstInstance instance{targetRecordFeature};
    EXPECT_EQ(instance.getintR0().get(), 0);

    connectionMod.applyConnection(testEnvironment.m_project, testEnvironment.m_log, &targetRecordFeature);
    EXPECT_TRUE(connectionMod.isFailed());
    EXPECT_EQ(connectionMod.getState(), babelwires::Modifier::State::SourceMissing);
    EXPECT_EQ(instance.getintR0().get(), 0);
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("Failed to apply operation"));
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("source"));
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("xx"));
}

TEST(ModifierTest, connectionModifierApplicationFailure) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueElementData elementData{testUtils::TestComplexRecordType::getThisIdentifier()};

    const babelwires::FeaturePath sourcePath{
        std::vector<babelwires::PathStep>{babelwires::PathStep(testUtils::TestComplexRecordType::getSubrecordId()),
                                          babelwires::PathStep(testUtils::TestSimpleRecordType::getInt0Id())}};

    babelwires::ValueAssignmentData sourceData(babelwires::IntValue(100));
    sourceData.m_pathToFeature = sourcePath;
    elementData.m_modifiers.emplace_back(std::make_unique<babelwires::ValueAssignmentData>(sourceData));

    const babelwires::ElementId sourceId = testEnvironment.m_project.addFeatureElement(elementData);

    babelwires::SimpleValueFeature targetRecordFeature(testEnvironment.m_typeSystem,
                                                       testUtils::TestComplexRecordType::getThisIdentifier());
    targetRecordFeature.setToDefault();
    testUtils::TestComplexRecordType::Instance targetInstance{targetRecordFeature};
    targetInstance.getstring().set("Hello");

    const babelwires::FeaturePath targetPath{
        std::vector<babelwires::PathStep>{babelwires::PathStep(testUtils::TestComplexRecordType::getStringId())}};

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_pathToFeature = targetPath;
    assignFromData->m_pathToSourceFeature = sourcePath;
    assignFromData->m_sourceId = sourceId;

    babelwires::ConnectionModifier connectionMod(std::move(assignFromData));
    TestOwner owner;
    connectionMod.setOwner(&owner);

    EXPECT_EQ(targetInstance.getstring().get(), "Hello");
    connectionMod.applyConnection(testEnvironment.m_project, testEnvironment.m_log, &targetRecordFeature);
    EXPECT_TRUE(connectionMod.isFailed());
    EXPECT_EQ(connectionMod.getState(), babelwires::Modifier::State::ApplicationFailed);
    // The string will now be default.
    EXPECT_TRUE(targetInstance.getstring().get().empty());
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("Failed to apply operation"));
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("incompatible"));
}
