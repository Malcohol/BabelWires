#include <gtest/gtest.h>

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testEnvironment.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Features/numericFeature.hpp"
#include "BabelWiresLib/Features/recordFeature.hpp"
#include "BabelWiresLib/Features/stringFeature.hpp"
#include "BabelWiresLib/Project/FeatureElements/sourceFileElementData.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElementData.hpp"
#include "BabelWiresLib/Project/FeatureElements/targetFileElement.hpp"
#include "BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifier.hpp"
#include "BabelWiresLib/Project/Modifiers/localModifier.hpp"
#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"
#include "BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifierData.hpp"

namespace {
    struct TestOwner : babelwires::FeatureElement {
        TestOwner()
            : FeatureElement(babelwires::SourceFileElementData(), 0) {}

        void doProcess(babelwires::UserLogger& userLogger) override {}
    };
} // namespace

TEST(ModifierTest, basicStuff) {
    babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/bb");

    auto intModData = std::make_unique<babelwires::IntValueAssignmentData>();
    intModData->m_pathToFeature = path;
    intModData->m_value = 198;

    babelwires::LocalModifier intMod(std::move(intModData));

    EXPECT_EQ(intMod.asConnectionModifier(), nullptr);
    EXPECT_EQ(intMod.getModifierData().m_pathToFeature, path);
    EXPECT_EQ(intMod.getPathToFeature(), path);
    EXPECT_NE(intMod.getModifierData().as<babelwires::IntValueAssignmentData>(), nullptr);
    EXPECT_EQ(static_cast<const babelwires::IntValueAssignmentData&>(intMod.getModifierData()).m_value, 198);

    EXPECT_EQ(const_cast<const babelwires::LocalModifier&>(intMod).getOwner(), nullptr);
    TestOwner owner;
    intMod.setOwner(&owner);
    EXPECT_EQ(const_cast<const babelwires::LocalModifier&>(intMod).getOwner(), &owner);
}

TEST(ModifierTest, clone) {
    babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/bb");

    auto intModData = std::make_unique<babelwires::IntValueAssignmentData>();
    intModData->m_pathToFeature = path;
    intModData->m_value = 198;

    babelwires::LocalModifier intMod(std::move(intModData));

    auto clone = intMod.clone();
    ASSERT_NE(clone, nullptr);
    EXPECT_EQ(clone->getModifierData().m_pathToFeature, path);
    EXPECT_NE(clone->getModifierData().as<babelwires::IntValueAssignmentData>(), nullptr);
    EXPECT_EQ(static_cast<const babelwires::IntValueAssignmentData&>(clone->getModifierData()).m_value, 198);
}

TEST(ModifierTest, localApplySuccess) {
    babelwires::RecordFeature recordFeature;

    babelwires::Identifier id0("aa");
    id0.setDiscriminator(1);
    babelwires::RecordFeature* childRecordFeature =
        recordFeature.addField(std::make_unique<babelwires::RecordFeature>(), id0);

    babelwires::Identifier id1("bb");
    id1.setDiscriminator(2);
    babelwires::IntFeature* intFeature = childRecordFeature->addField(std::make_unique<babelwires::IntFeature>(), id1);

    babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/bb");

    auto intModData = std::make_unique<babelwires::IntValueAssignmentData>();
    intModData->m_pathToFeature = path;
    intModData->m_value = 198;

    babelwires::LocalModifier intMod(std::move(intModData));

    testUtils::TestLog testLog;

    EXPECT_EQ(intFeature->get(), 0);
    intMod.applyIfLocal(testLog, &recordFeature);
    EXPECT_EQ(intFeature->get(), 198);
    EXPECT_FALSE(intMod.isFailed());
    EXPECT_EQ(intMod.getState(), babelwires::Modifier::State::Success);

    intMod.unapply(&recordFeature);
    EXPECT_EQ(intFeature->get(), 0);
}

TEST(ModifierTest, localApplyFailureWrongType) {
    babelwires::RecordFeature recordFeature;

    babelwires::Identifier id0("aa");
    id0.setDiscriminator(1);
    babelwires::RecordFeature* childRecordFeature =
        recordFeature.addField(std::make_unique<babelwires::RecordFeature>(), id0);

    babelwires::Identifier id1("bb");
    id1.setDiscriminator(2);
    babelwires::StringFeature* stringFeature =
        childRecordFeature->addField(std::make_unique<babelwires::StringFeature>(), id1);

    babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/bb");

    auto intModData = std::make_unique<babelwires::IntValueAssignmentData>();
    intModData->m_pathToFeature = path;
    intModData->m_value = 198;

    babelwires::LocalModifier intMod(std::move(intModData));

    TestOwner owner;
    intMod.setOwner(&owner);

    testUtils::TestLogWithListener testLog;

    intMod.applyIfLocal(testLog, &recordFeature);
    EXPECT_TRUE(intMod.isFailed());
    EXPECT_EQ(intMod.getState(), babelwires::Modifier::State::ApplicationFailed);
    EXPECT_TRUE(testLog.hasSubstringIgnoreCase("Failed to apply operation"));
}

TEST(ModifierTest, localApplyFailureNoTarget) {
    babelwires::RecordFeature recordFeature;

    babelwires::Identifier id0("aa");
    id0.setDiscriminator(1);
    babelwires::RecordFeature* childRecordFeature =
        recordFeature.addField(std::make_unique<babelwires::RecordFeature>(), id0);

    babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/bb");

    auto intModData = std::make_unique<babelwires::IntValueAssignmentData>();
    intModData->m_pathToFeature = path;
    intModData->m_value = 198;

    babelwires::LocalModifier intMod(std::move(intModData));

    TestOwner owner;
    intMod.setOwner(&owner);

    testUtils::TestLogWithListener testLog;

    // An exception will try to print out a path, which will expect one of these singletons.
    babelwires::IdentifierRegistryScope identifierRegistry;

    intMod.applyIfLocal(testLog, &recordFeature);
    EXPECT_TRUE(intMod.isFailed());
    EXPECT_EQ(intMod.getState(), babelwires::Modifier::State::TargetMissing);
    EXPECT_TRUE(testLog.hasSubstringIgnoreCase("Failed to apply operation"));
}

TEST(ModifierTest, arraySizeModifierSuccess) {
    babelwires::RecordFeature recordFeature;

    babelwires::Identifier id0("aa");
    id0.setDiscriminator(1);
    babelwires::ArrayFeature* arrayFeature =
        recordFeature.addField(std::make_unique<babelwires::StandardArrayFeature<babelwires::IntFeature>>(), id0);

    babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa");

    auto arrayModData = std::make_unique<babelwires::ArraySizeModifierData>();
    arrayModData->m_pathToFeature = path;
    arrayModData->m_size = 3;

    babelwires::ArraySizeModifier arrayMod(std::move(arrayModData));

    TestOwner owner;
    arrayMod.setOwner(&owner);

    testUtils::TestLogWithListener testLog;

    // An exception will try to print out a path, which will expect one of these singletons.
    babelwires::IdentifierRegistryScope identifierRegistry;

    arrayMod.applyIfLocal(testLog, &recordFeature);
    EXPECT_FALSE(arrayMod.isFailed());
    EXPECT_EQ(arrayMod.getState(), babelwires::Modifier::State::Success);
    EXPECT_EQ(arrayFeature->getNumFeatures(), 3);

    arrayFeature->getFeature(0)->as<babelwires::IntFeature>()->set(10);
    arrayFeature->getFeature(1)->as<babelwires::IntFeature>()->set(11);
    arrayFeature->getFeature(2)->as<babelwires::IntFeature>()->set(12);

    EXPECT_TRUE(arrayMod.addArrayEntries(testLog, &recordFeature, 1, 2));

    EXPECT_EQ(arrayFeature->getNumFeatures(), 5);
    EXPECT_EQ(arrayFeature->getFeature(0)->as<babelwires::IntFeature>()->get(), 10);
    EXPECT_EQ(arrayFeature->getFeature(1)->as<babelwires::IntFeature>()->get(), 0);
    EXPECT_EQ(arrayFeature->getFeature(2)->as<babelwires::IntFeature>()->get(), 0);
    EXPECT_EQ(arrayFeature->getFeature(3)->as<babelwires::IntFeature>()->get(), 11);
    EXPECT_EQ(arrayFeature->getFeature(4)->as<babelwires::IntFeature>()->get(), 12);

    EXPECT_TRUE(arrayMod.removeArrayEntries(testLog, &recordFeature, 2, 2));

    EXPECT_EQ(arrayFeature->getNumFeatures(), 3);
    EXPECT_EQ(arrayFeature->getFeature(0)->as<babelwires::IntFeature>()->get(), 10);
    EXPECT_EQ(arrayFeature->getFeature(1)->as<babelwires::IntFeature>()->get(), 0);
    EXPECT_EQ(arrayFeature->getFeature(2)->as<babelwires::IntFeature>()->get(), 12);
}

TEST(ModifierTest, arraySizeModifierFailure) {
    babelwires::RecordFeature recordFeature;

    babelwires::Identifier id0("aa");
    id0.setDiscriminator(1);
    babelwires::ArrayFeature* arrayFeature =
        recordFeature.addField(std::make_unique<babelwires::StandardArrayFeature<babelwires::IntFeature>>(), id0);

    babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa");

    auto arrayModData = std::make_unique<babelwires::ArraySizeModifierData>();
    arrayModData->m_pathToFeature = path;
    // The default max size range is 16.
    arrayModData->m_size = 17;

    babelwires::ArraySizeModifier arrayMod(std::move(arrayModData));

    TestOwner owner;
    arrayMod.setOwner(&owner);

    testUtils::TestLogWithListener testLog;

    // An exception will try to print out a path, which will expect one of these singletons.
    babelwires::IdentifierRegistryScope identifierRegistry;

    arrayMod.applyIfLocal(testLog, &recordFeature);
    EXPECT_TRUE(arrayMod.isFailed());
    EXPECT_EQ(arrayMod.getState(), babelwires::Modifier::State::ApplicationFailed);
    EXPECT_EQ(arrayFeature->getNumFeatures(), 0);
    EXPECT_TRUE(testLog.hasSubstringIgnoreCase("Failed to apply operation"));
    EXPECT_TRUE(testLog.hasSubstringIgnoreCase("array"));
    EXPECT_TRUE(testLog.hasSubstringIgnoreCase("size"));
}

TEST(ModifierTest, connectionModifierSuccess) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment projectContext;

    testUtils::TestFeatureElementData elementData;
    const babelwires::FeaturePath sourcePath = testUtils::TestRootFeature::s_pathToInt;
    babelwires::IntValueAssignmentData sourceData;
    sourceData.m_pathToFeature = sourcePath;
    sourceData.m_value = 100;
    elementData.m_modifiers.emplace_back(std::make_unique<babelwires::IntValueAssignmentData>(sourceData));

    const babelwires::ElementId sourceId = projectContext.m_project.addFeatureElement(elementData);

    babelwires::RecordFeature targetRecordFeature;
    babelwires::Identifier id1("bb");
    id1.setDiscriminator(1);
    babelwires::IntFeature* targetFeature =
        targetRecordFeature.addField(std::make_unique<babelwires::IntFeature>(), id1);

    const babelwires::FeaturePath targetPath = babelwires::FeaturePath::deserializeFromString("bb");

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_pathToFeature = targetPath;
    assignFromData->m_pathToSourceFeature = sourcePath;
    assignFromData->m_sourceId = sourceId;

    babelwires::ConnectionModifier connectionMod(std::move(assignFromData));
    TestOwner owner;
    connectionMod.setOwner(&owner);

    EXPECT_EQ(targetFeature->get(), 0);

    connectionMod.applyConnection(projectContext.m_project, projectContext.m_log, &targetRecordFeature);

    EXPECT_EQ(targetFeature->get(), 100);
}

TEST(ModifierTest, connectionModifierTargetPathFailure) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment projectContext;

    babelwires::RecordFeature targetRecordFeature;
    babelwires::Identifier id1("bb");
    id1.setDiscriminator(1);
    babelwires::IntFeature* targetFeature =
        targetRecordFeature.addField(std::make_unique<babelwires::IntFeature>(), id1);

    const babelwires::FeaturePath sourcePath = babelwires::FeaturePath::deserializeFromString("aa");
    const babelwires::FeaturePath targetPath = babelwires::FeaturePath::deserializeFromString("xx");

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_pathToFeature = targetPath;
    assignFromData->m_pathToSourceFeature = sourcePath;
    assignFromData->m_sourceId = 10;

    babelwires::ConnectionModifier connectionMod(std::move(assignFromData));
    TestOwner owner;
    connectionMod.setOwner(&owner);

    EXPECT_EQ(targetFeature->get(), 0);
    connectionMod.applyConnection(projectContext.m_project, projectContext.m_log, &targetRecordFeature);
    EXPECT_TRUE(connectionMod.isFailed());
    EXPECT_EQ(connectionMod.getState(), babelwires::Modifier::State::TargetMissing);
    EXPECT_EQ(targetFeature->get(), 0);
    EXPECT_TRUE(projectContext.m_log.hasSubstringIgnoreCase("Failed to apply operation"));
    EXPECT_TRUE(projectContext.m_log.hasSubstringIgnoreCase("xx"));
}

TEST(ModifierTest, connectionModifierSourceIdFailure) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment projectContext;

    babelwires::RecordFeature targetRecordFeature;
    babelwires::Identifier id1("bb");
    id1.setDiscriminator(1);
    babelwires::IntFeature* targetFeature =
        targetRecordFeature.addField(std::make_unique<babelwires::IntFeature>(), id1);

    const babelwires::FeaturePath sourcePath = babelwires::FeaturePath::deserializeFromString("aa");
    const babelwires::FeaturePath targetPath = babelwires::FeaturePath::deserializeFromString("bb");

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_pathToFeature = targetPath;
    assignFromData->m_pathToSourceFeature = sourcePath;
    assignFromData->m_sourceId = 99;

    babelwires::ConnectionModifier connectionMod(std::move(assignFromData));
    TestOwner owner;
    connectionMod.setOwner(&owner);

    EXPECT_EQ(targetFeature->get(), 0);
    connectionMod.applyConnection(projectContext.m_project, projectContext.m_log, &targetRecordFeature);
    EXPECT_TRUE(connectionMod.isFailed());
    EXPECT_EQ(connectionMod.getState(), babelwires::Modifier::State::SourceMissing);
    EXPECT_EQ(targetFeature->get(), 0);
    EXPECT_TRUE(projectContext.m_log.hasSubstringIgnoreCase("Failed to apply operation"));
    EXPECT_TRUE(projectContext.m_log.hasSubstringIgnoreCase("source"));
    EXPECT_TRUE(projectContext.m_log.hasSubstringIgnoreCase("99"));
}

TEST(ModifierTest, connectionModifierSourcePathFailure) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment projectContext;

    testUtils::TestFeatureElementData elementData;
    const babelwires::FeaturePath sourcePath = testUtils::TestRootFeature::s_pathToInt;
    babelwires::IntValueAssignmentData sourceData;
    sourceData.m_pathToFeature = sourcePath;
    sourceData.m_value = 100;
    elementData.m_modifiers.emplace_back(std::make_unique<babelwires::IntValueAssignmentData>(sourceData));

    const babelwires::ElementId sourceId = projectContext.m_project.addFeatureElement(elementData);

    babelwires::RecordFeature targetRecordFeature;
    babelwires::Identifier id1("bb");
    id1.setDiscriminator(1);
    babelwires::IntFeature* targetFeature =
        targetRecordFeature.addField(std::make_unique<babelwires::IntFeature>(), id1);

    const babelwires::FeaturePath targetPath = babelwires::FeaturePath::deserializeFromString("bb");

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_pathToFeature = targetPath;
    assignFromData->m_pathToSourceFeature = babelwires::FeaturePath::deserializeFromString("xx");
    assignFromData->m_sourceId = sourceId;

    babelwires::ConnectionModifier connectionMod(std::move(assignFromData));
    TestOwner owner;
    connectionMod.setOwner(&owner);

    EXPECT_EQ(targetFeature->get(), 0);
    connectionMod.applyConnection(projectContext.m_project, projectContext.m_log, &targetRecordFeature);
    EXPECT_TRUE(connectionMod.isFailed());
    EXPECT_EQ(connectionMod.getState(), babelwires::Modifier::State::SourceMissing);
    EXPECT_EQ(targetFeature->get(), 0);
    EXPECT_TRUE(projectContext.m_log.hasSubstringIgnoreCase("Failed to apply operation"));
    EXPECT_TRUE(projectContext.m_log.hasSubstringIgnoreCase("source"));
    EXPECT_TRUE(projectContext.m_log.hasSubstringIgnoreCase("xx"));
}

TEST(ModifierTest, connectionModifierApplicationFailure) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment projectContext;

    testUtils::TestFeatureElementData elementData;
    const babelwires::FeaturePath sourcePath = testUtils::TestRootFeature::s_pathToInt;
    babelwires::IntValueAssignmentData sourceData;
    sourceData.m_pathToFeature = sourcePath;
    sourceData.m_value = 100;
    elementData.m_modifiers.emplace_back(std::make_unique<babelwires::IntValueAssignmentData>(sourceData));

    const babelwires::ElementId sourceId = projectContext.m_project.addFeatureElement(elementData);

    babelwires::RecordFeature targetRecordFeature;
    babelwires::Identifier id1("bb");
    id1.setDiscriminator(1);
    babelwires::StringFeature* targetFeature =
        targetRecordFeature.addField(std::make_unique<babelwires::StringFeature>(), id1);
    targetFeature->set("Hello");

    const babelwires::FeaturePath targetPath = babelwires::FeaturePath::deserializeFromString("bb");

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_pathToFeature = targetPath;
    assignFromData->m_pathToSourceFeature = sourcePath;
    assignFromData->m_sourceId = sourceId;

    babelwires::ConnectionModifier connectionMod(std::move(assignFromData));
    TestOwner owner;
    connectionMod.setOwner(&owner);

    EXPECT_EQ(targetFeature->get(), "Hello");
    connectionMod.applyConnection(projectContext.m_project, projectContext.m_log, &targetRecordFeature);
    EXPECT_TRUE(connectionMod.isFailed());
    EXPECT_EQ(connectionMod.getState(), babelwires::Modifier::State::ApplicationFailed);
    // The string will now be default.
    EXPECT_TRUE(targetFeature->get().empty());
    EXPECT_TRUE(projectContext.m_log.hasSubstringIgnoreCase("Failed to apply operation"));
    EXPECT_TRUE(projectContext.m_log.hasSubstringIgnoreCase("incompatible"));
}
