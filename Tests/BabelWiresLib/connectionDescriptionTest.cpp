#include <gtest/gtest.h>

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/ProjectExtra/connectionDescription.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>

#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(ConnectionDescriptionTest, equalityAndHash) {
    const babelwires::Path path0 = babelwires::Path::deserializeFromString("a/b");
    const babelwires::Path path1 = babelwires::Path::deserializeFromString("c/d");
    const babelwires::NodeId sourceId0 = 14;
    const babelwires::NodeId sourceId1 = 15;
    const babelwires::NodeId targetId0 = 4;
    const babelwires::NodeId targetId1 = 6;

    babelwires::ConnectionModifierData data0;
    data0.m_targetPath = path0;
    data0.m_sourcePath = path1;
    data0.m_sourceId = sourceId0;

    babelwires::ConnectionModifierData data1;
    data1.m_targetPath = path1;
    data1.m_sourcePath = path0;
    data1.m_sourceId = sourceId0;

    babelwires::ConnectionModifierData data2;
    data2.m_targetPath = path0;
    data2.m_sourcePath = path1;
    data2.m_sourceId = sourceId1;

    babelwires::ConnectionModifierData data3;
    data3.m_targetPath = path1;
    data3.m_sourcePath = path0;
    data3.m_sourceId = sourceId1;

    babelwires::ConnectionDescription descriptions[] = {
        babelwires::ConnectionDescription(targetId0, data0), babelwires::ConnectionDescription(targetId0, data1),
        babelwires::ConnectionDescription(targetId0, data2), babelwires::ConnectionDescription(targetId0, data3),
        babelwires::ConnectionDescription(targetId1, data0), babelwires::ConnectionDescription(targetId1, data1),
        babelwires::ConnectionDescription(targetId1, data2), babelwires::ConnectionDescription(targetId1, data3),
    };

    auto hasher = std::hash<babelwires::ConnectionDescription>();

    for (int i = 0; i < 7; ++i) {
        EXPECT_EQ(descriptions[i], descriptions[i]);
        EXPECT_EQ(hasher(descriptions[i]), hasher(descriptions[i]));

        for (int j = i + 1; j < 8; ++j) {
            EXPECT_NE(descriptions[i], descriptions[j]);
            // There's a statistical chance of a false positive here. If necessary, adjust values.
            EXPECT_NE(hasher(descriptions[i]), hasher(descriptions[j]));
        }
    }
}

TEST(ConnectionDescriptionTest, getCommands) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId sourceId = 12;
    const babelwires::NodeId targetId = 14;

    babelwires::ConnectionModifierData connectionData;
    connectionData.m_targetPath = testDomain::TestComplexRecordElementData::getPathToRecordArrayEntry(1);
    connectionData.m_sourcePath = testDomain::TestComplexRecordElementData::getPathToRecordSubrecordInt1();
    connectionData.m_sourceId = sourceId;

    babelwires::ConnectionDescription connection(14, connectionData);

    {
        testDomain::TestComplexRecordElementData sourceElementData;
        sourceElementData.m_id = sourceId;
        testEnvironment.m_project.addNode(sourceElementData);
    }
    {
        testDomain::TestComplexRecordElementData targetElementData;
        targetElementData.m_id = targetId;
        testEnvironment.m_project.addNode(targetElementData);
    }

    const babelwires::Node* targetElement = testEnvironment.m_project.getNode(targetId);
    ASSERT_NE(targetElement, nullptr);

    testEnvironment.m_project.process();

    auto connectionCommand = connection.getConnectionCommand();
    auto disconnectionCommand = connection.getDisconnectionCommand();

    const auto checkModifier = [&testEnvironment, targetElement, &connectionData](bool isAdded) {
        const babelwires::Modifier* modifier =
            targetElement->findModifier(testDomain::TestComplexRecordElementData::getPathToRecordArrayEntry(1));
        if (isAdded) {
            ASSERT_NE(modifier, nullptr);
            const babelwires::ConnectionModifier* connection = modifier->as<babelwires::ConnectionModifier>();
            ASSERT_NE(connection, nullptr);
            EXPECT_EQ(connection->getModifierData().m_targetPath, connectionData.m_targetPath);
            EXPECT_EQ(connection->getModifierData().m_sourcePath, connectionData.m_sourcePath);
            EXPECT_EQ(connection->getModifierData().m_sourceId, connectionData.m_sourceId);
        } else {
            EXPECT_EQ(modifier, nullptr);
        }
    };

    checkModifier(false);

    EXPECT_TRUE(connectionCommand->initializeAndExecute(testEnvironment.m_project));
    testEnvironment.m_project.process();

    checkModifier(true);

    EXPECT_TRUE(disconnectionCommand->initializeAndExecute(testEnvironment.m_project));
    testEnvironment.m_project.process();

    checkModifier(false);
}
