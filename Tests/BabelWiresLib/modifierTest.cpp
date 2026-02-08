#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNode.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Domains/TestDomain/testArrayType.hpp>
#include <Domains/TestDomain/testRecordType.hpp>

namespace {
    struct TestOwner : babelwires::Node {
        TestOwner()
            : Node(babelwires::SourceFileNodeData(), 0) {}

        void doProcess(babelwires::UserLogger& userLogger) override {}
    };
} // namespace

TEST(ModifierTest, basicStuff) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::Path path = *babelwires::Path::deserializeFromString("aa/bb");

    auto intModData = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(198));
    intModData->m_targetPath = path;

    babelwires::LocalModifier intMod(std::move(intModData));

    EXPECT_EQ(intMod.asConnectionModifier(), nullptr);
    EXPECT_EQ(intMod.getModifierData().m_targetPath, path);
    EXPECT_EQ(intMod.getTargetPath(), path);
    EXPECT_NE(intMod.getModifierData().tryAs<babelwires::ValueAssignmentData>(), nullptr);
    EXPECT_NE(static_cast<const babelwires::ValueAssignmentData&>(intMod.getModifierData())
                  .getValue()
                  ->tryAs<babelwires::IntValue>(),
              nullptr);
    EXPECT_EQ(static_cast<const babelwires::ValueAssignmentData&>(intMod.getModifierData())
                  .getValue()
                  ->tryAs<babelwires::IntValue>()
                  ->get(),
              198);

    EXPECT_EQ(const_cast<const babelwires::LocalModifier&>(intMod).getOwner(), nullptr);
    TestOwner owner;
    intMod.setOwner(&owner);
    EXPECT_EQ(const_cast<const babelwires::LocalModifier&>(intMod).getOwner(), &owner);
}

TEST(ModifierTest, clone) {
    babelwires::Path path = *babelwires::Path::deserializeFromString("aa/bb");

    auto intModData = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(198));
    intModData->m_targetPath = path;

    babelwires::LocalModifier intMod(std::move(intModData));

    auto clone = intMod.clone();
    ASSERT_NE(clone, nullptr);
    EXPECT_EQ(clone->getModifierData().m_targetPath, path);
    EXPECT_NE(clone->getModifierData().tryAs<babelwires::ValueAssignmentData>(), nullptr);
    EXPECT_NE(static_cast<const babelwires::ValueAssignmentData&>(clone->getModifierData())
                  .getValue()
                  ->tryAs<babelwires::IntValue>(),
              nullptr);
    EXPECT_EQ(static_cast<const babelwires::ValueAssignmentData&>(clone->getModifierData())
                  .getValue()
                  ->tryAs<babelwires::IntValue>()
                  ->get(),
              198);
}

TEST(ModifierTest, localApplySuccess) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot recordFeature{testEnvironment.m_projectContext.m_typeSystem,
                                                 testDomain::TestSimpleRecordType::getThisIdentifier()};
    recordFeature.setToDefault();

    babelwires::Path path;
    path.pushStep(testDomain::TestSimpleRecordType::getInt0Id());

    auto intModData = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(198));
    intModData->m_targetPath = path;

    babelwires::LocalModifier intMod(std::move(intModData));

    testDomain::TestSimpleRecordType::ConstInstance instance{recordFeature};

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

    babelwires::ValueTreeRoot recordFeature(testEnvironment.m_typeSystem,
                                                 testDomain::TestSimpleRecordType::getThisIdentifier());
    recordFeature.setToDefault();

    babelwires::Path path;
    path.pushStep(babelwires::PathStep{testDomain::TestSimpleRecordType::getInt0Id()});

    auto stringModData = std::make_unique<babelwires::ValueAssignmentData>(babelwires::StringValue("Hello"));
    stringModData->m_targetPath = path;

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

    babelwires::ValueTreeRoot recordFeature{testEnvironment.m_projectContext.m_typeSystem,
                                                 testDomain::TestSimpleRecordType::getThisIdentifier()};
    recordFeature.setToDefault();

    babelwires::Path path;
    path.pushStep(babelwires::PathStep{testDomain::TestSimpleRecordType::getInt0Id()});
    path.pushStep(12);

    auto intModData = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(198));
    intModData->m_targetPath = path;

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

    babelwires::ValueTreeRoot recordFeature(testEnvironment.m_typeSystem,
                                                 testDomain::TestComplexRecordType::getThisIdentifier());
    recordFeature.setToDefault();

    auto arrayModData = std::make_unique<babelwires::ArraySizeModifierData>();
    arrayModData->m_targetPath.pushStep(testDomain::TestComplexRecordType::getArrayId());
    arrayModData->m_size = testDomain::TestSimpleArrayType::s_nonDefaultSize;

    babelwires::ArraySizeModifier arrayMod(std::move(arrayModData));

    TestOwner owner;
    arrayMod.setOwner(&owner);

    testDomain::TestComplexRecordType::ConstInstance record(recordFeature);
    EXPECT_EQ(record.getarray().getSize(), testDomain::TestSimpleArrayType::s_defaultSize);

    arrayMod.applyIfLocal(testEnvironment.m_log, &recordFeature);
    EXPECT_FALSE(arrayMod.isFailed());
    EXPECT_EQ(arrayMod.getState(), babelwires::Modifier::State::Success);

    EXPECT_EQ(record.getarray().getSize(), testDomain::TestSimpleArrayType::s_nonDefaultSize);

    EXPECT_TRUE(arrayMod.addArrayEntries(testEnvironment.m_log, &recordFeature, 1, 2));
    EXPECT_EQ(record.getarray().getSize(), testDomain::TestSimpleArrayType::s_nonDefaultSize + 2);

    EXPECT_TRUE(arrayMod.removeArrayEntries(testEnvironment.m_log, &recordFeature, 2, 2));
    EXPECT_EQ(record.getarray().getSize(), testDomain::TestSimpleArrayType::s_nonDefaultSize);
}

TEST(ModifierTest, arraySizeModifierFailure) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot recordFeature(testEnvironment.m_typeSystem,
                                                 testDomain::TestComplexRecordType::getThisIdentifier());
    recordFeature.setToDefault();

    testDomain::TestComplexRecordType::ConstInstance record(recordFeature);
    EXPECT_EQ(record.getarray().getSize(), testDomain::TestSimpleArrayType::s_defaultSize);

    auto arrayModData = std::make_unique<babelwires::ArraySizeModifierData>();
    arrayModData->m_targetPath.pushStep(testDomain::TestComplexRecordType::getArrayId());
    arrayModData->m_size = testDomain::TestSimpleArrayType::s_maximumSize + 1;

    babelwires::ArraySizeModifier arrayMod(std::move(arrayModData));

    TestOwner owner;
    arrayMod.setOwner(&owner);

    arrayMod.applyIfLocal(testEnvironment.m_log, &recordFeature);
    EXPECT_TRUE(arrayMod.isFailed());
    EXPECT_EQ(arrayMod.getState(), babelwires::Modifier::State::ApplicationFailed);

    EXPECT_EQ(record.getarray().getSize(), testDomain::TestSimpleArrayType::s_defaultSize);

    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("Failed to apply operation"));
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("array"));
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("size"));
}

TEST(ModifierTest, connectionModifierSuccess) {
    testUtils::TestEnvironment testEnvironment;

    testDomain::TestSimpleRecordElementData elementData;
    const babelwires::Path sourcePath = testDomain::TestSimpleRecordElementData::getPathToRecordInt0();
    babelwires::ValueAssignmentData sourceData(babelwires::IntValue(100));
    sourceData.m_targetPath = sourcePath;
    elementData.m_modifiers.emplace_back(std::make_unique<babelwires::ValueAssignmentData>(sourceData));

    const babelwires::NodeId sourceId = testEnvironment.m_project.addNode(elementData);

    babelwires::ValueTreeRoot targetRecordFeature(testEnvironment.m_projectContext.m_typeSystem,
                                                       testDomain::TestSimpleRecordType::getThisIdentifier());
    targetRecordFeature.setToDefault();

    babelwires::Path targetPath;
    targetPath.pushStep(testDomain::TestSimpleRecordType::getInt0Id());

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_targetPath = targetPath;
    assignFromData->m_sourcePath = sourcePath;
    assignFromData->m_sourceId = sourceId;

    babelwires::ConnectionModifier connectionMod(std::move(assignFromData));
    TestOwner owner;
    connectionMod.setOwner(&owner);

    testDomain::TestSimpleRecordType::ConstInstance instance{targetRecordFeature};
    EXPECT_EQ(instance.getintR0().get(), 0);

    connectionMod.applyConnection(testEnvironment.m_project, testEnvironment.m_log, &targetRecordFeature);
    EXPECT_EQ(instance.getintR0().get(), 100);
}

TEST(ModifierTest, connectionModifierTargetPathFailure) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueTreeRoot targetRecordFeature(testEnvironment.m_projectContext.m_typeSystem,
                                                       testDomain::TestSimpleRecordType::getThisIdentifier());
    targetRecordFeature.setToDefault();

    const babelwires::Path sourcePath = *babelwires::Path::deserializeFromString("aa");
    const babelwires::Path targetPath = *babelwires::Path::deserializeFromString("xx");

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_targetPath = targetPath;
    assignFromData->m_sourcePath = sourcePath;
    assignFromData->m_sourceId = 10;

    babelwires::ConnectionModifier connectionMod(std::move(assignFromData));
    TestOwner owner;
    connectionMod.setOwner(&owner);

    testDomain::TestSimpleRecordType::ConstInstance instance{targetRecordFeature};
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

    babelwires::ValueTreeRoot targetRecordFeature(testEnvironment.m_projectContext.m_typeSystem,
                                                       testDomain::TestSimpleRecordType::getThisIdentifier());
    targetRecordFeature.setToDefault();

    const babelwires::Path sourcePath = *babelwires::Path::deserializeFromString("aa");
    babelwires::Path targetPath;
    targetPath.pushStep(testDomain::TestSimpleRecordType::getInt0Id());

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_targetPath = targetPath;
    assignFromData->m_sourcePath = sourcePath;
    assignFromData->m_sourceId = 99;

    babelwires::ConnectionModifier connectionMod(std::move(assignFromData));
    TestOwner owner;
    connectionMod.setOwner(&owner);

    testDomain::TestSimpleRecordType::ConstInstance instance{targetRecordFeature};
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

    testDomain::TestSimpleRecordElementData elementData;
    const babelwires::Path sourcePath = testDomain::TestSimpleRecordElementData::getPathToRecordInt0();
    babelwires::ValueAssignmentData sourceData(babelwires::IntValue(100));
    sourceData.m_targetPath = sourcePath;
    elementData.m_modifiers.emplace_back(std::make_unique<babelwires::ValueAssignmentData>(sourceData));

    const babelwires::NodeId sourceId = testEnvironment.m_project.addNode(elementData);

    babelwires::ValueTreeRoot targetRecordFeature(testEnvironment.m_projectContext.m_typeSystem,
                                                       testDomain::TestSimpleRecordType::getThisIdentifier());
    targetRecordFeature.setToDefault();

    babelwires::Path targetPath;
    targetPath.pushStep(testDomain::TestSimpleRecordType::getInt0Id());

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_targetPath = targetPath;
    assignFromData->m_sourcePath = *babelwires::Path::deserializeFromString("xx");
    assignFromData->m_sourceId = sourceId;

    babelwires::ConnectionModifier connectionMod(std::move(assignFromData));
    TestOwner owner;
    connectionMod.setOwner(&owner);

    testDomain::TestSimpleRecordType::ConstInstance instance{targetRecordFeature};
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

    babelwires::ValueNodeData elementData{testDomain::TestComplexRecordType::getThisIdentifier()};

    const babelwires::Path sourcePath{
        std::vector<babelwires::PathStep>{testDomain::TestComplexRecordType::getSubrecordId(),
                                          testDomain::TestSimpleRecordType::getInt0Id()}};

    babelwires::ValueAssignmentData sourceData(babelwires::IntValue(100));
    sourceData.m_targetPath = sourcePath;
    elementData.m_modifiers.emplace_back(std::make_unique<babelwires::ValueAssignmentData>(sourceData));

    const babelwires::NodeId sourceId = testEnvironment.m_project.addNode(elementData);

    babelwires::ValueTreeRoot targetRecordFeature(testEnvironment.m_typeSystem,
                                                       testDomain::TestComplexRecordType::getThisIdentifier());
    targetRecordFeature.setToDefault();
    testDomain::TestComplexRecordType::Instance targetInstance{targetRecordFeature};
    targetInstance.getstring().set("Hello");

    const babelwires::Path targetPath{
        std::vector<babelwires::PathStep>{testDomain::TestComplexRecordType::getStringId()}};

    auto assignFromData = std::make_unique<babelwires::ConnectionModifierData>();
    assignFromData->m_targetPath = targetPath;
    assignFromData->m_sourcePath = sourcePath;
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
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase("not a valid instance"));
}
