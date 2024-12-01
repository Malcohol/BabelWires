#include <gtest/gtest.h>

#include <BabelWiresLib/ProjectExtra/projectObserver.hpp>

#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>
#include <Tests/TestUtils/equalSets.hpp>

namespace {
    /// Capture the data from the callbacks, to make it easy to detect.
    /// Very roughly, this class is the inverse of the ProjectObserver.
    struct ObservedChanges {
        ObservedChanges(babelwires::ProjectObserver& observer) {
            m_nodesAddedSubscription = observer.m_nodeWasAdded.subscribe(
                [this](const babelwires::Node* node) { m_nodesAdded.emplace_back(node); });
            m_nodeWasRemovedSubscription = observer.m_nodeWasRemoved.subscribe(
                [this](const babelwires::NodeId elementId) { m_nodesRemoved.emplace_back(elementId); });
            m_nodeWasMovedSubscription = observer.m_nodeWasMoved.subscribe(
                [this](babelwires::NodeId elementId, const babelwires::UiPosition& position) {
                    m_nodesMoved.emplace_back(std::tuple(elementId, position));
                });
            m_nodeWasResizedSubscription = observer.m_nodeWasResized.subscribe(
                [this](babelwires::NodeId elementId, const babelwires::UiSize& size) {
                    m_nodesResized.emplace_back(std::tuple(elementId, size));
                });
            m_connectionWasAddedSubscription =
                observer.m_connectionWasAdded.subscribe([this](const babelwires::ConnectionDescription& connection) {
                    m_connectionsAdded.emplace_back(connection);
                });
            m_connectionWasRemovedSubscription =
                observer.m_connectionWasRemoved.subscribe([this](const babelwires::ConnectionDescription& connection) {
                    m_connectionsRemoved.emplace_back(connection);
                });
            m_contentWasChangedSubscription = observer.m_contentWasChanged.subscribe(
                [this](const babelwires::NodeId elementId) { m_contentsChanged.emplace_back(elementId); });
        }

        void clear() { m_nodesAdded.clear(); }

        babelwires::SignalSubscription m_nodesAddedSubscription;
        babelwires::SignalSubscription m_nodeWasRemovedSubscription;
        babelwires::SignalSubscription m_nodeWasMovedSubscription;
        babelwires::SignalSubscription m_nodeWasResizedSubscription;
        babelwires::SignalSubscription m_connectionWasAddedSubscription;
        babelwires::SignalSubscription m_connectionWasRemovedSubscription;
        babelwires::SignalSubscription m_contentWasChangedSubscription;

        std::vector<const babelwires::Node*> m_nodesAdded;
        std::vector<babelwires::NodeId> m_nodesRemoved;
        std::vector<std::tuple<babelwires::NodeId, babelwires::UiPosition>> m_nodesMoved;
        std::vector<std::tuple<babelwires::NodeId, babelwires::UiSize>> m_nodesResized;
        std::vector<babelwires::ConnectionDescription> m_connectionsAdded;
        std::vector<babelwires::ConnectionDescription> m_connectionsRemoved;
        std::vector<babelwires::NodeId> m_contentsChanged;
    };
} // namespace

namespace {
    void testNodeAdded(bool shouldIgnore) {
        testUtils::TestEnvironment testEnvironment;

        babelwires::ProjectObserver projectObserver(testEnvironment.m_project);
        ObservedChanges observedChanges(projectObserver);

        const babelwires::NodeId elementId =
            testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());

        if (shouldIgnore) {
            projectObserver.ignoreAddedNode(elementId);
        }

        testEnvironment.m_project.process();
        projectObserver.interpretChangesAndFireSignals();

        if (shouldIgnore) {
            EXPECT_TRUE(observedChanges.m_nodesAdded.empty());
        } else {
            ASSERT_EQ(observedChanges.m_nodesAdded.size(), 1);
            EXPECT_EQ(observedChanges.m_nodesAdded[0]->getNodeId(), elementId);
        }

        EXPECT_TRUE(observedChanges.m_nodesRemoved.empty());
        EXPECT_TRUE(observedChanges.m_nodesMoved.empty());
        EXPECT_TRUE(observedChanges.m_nodesResized.empty());
        EXPECT_TRUE(observedChanges.m_connectionsAdded.empty());
        EXPECT_TRUE(observedChanges.m_connectionsRemoved.empty());
        EXPECT_TRUE(observedChanges.m_contentsChanged.empty());
    }
} // namespace

TEST(ProjectObserverTest, nodeAdded) {
    testNodeAdded(false);
}

TEST(ProjectObserverTest, nodeAddedIgnore) {
    testNodeAdded(false);
}

namespace {
    void testNodeRemoved(bool shouldIgnore) {
            testUtils::TestEnvironment testEnvironment;

        const babelwires::NodeId elementId =
            testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());

        testEnvironment.m_project.process();
        testEnvironment.m_project.clearChanges();

        babelwires::ProjectObserver projectObserver(testEnvironment.m_project);
        ObservedChanges observedChanges(projectObserver);

        testEnvironment.m_project.removeNode(elementId);
        testEnvironment.m_project.process();

        if (shouldIgnore) {
            projectObserver.ignoreRemovedNode(elementId);
        }

        projectObserver.interpretChangesAndFireSignals();

        EXPECT_TRUE(observedChanges.m_nodesAdded.empty());

        if (shouldIgnore) {
            EXPECT_TRUE(observedChanges.m_nodesRemoved.empty());
        } else {
            ASSERT_EQ(observedChanges.m_nodesRemoved.size(), 1);
            EXPECT_EQ(observedChanges.m_nodesRemoved[0], elementId);
        }

        EXPECT_TRUE(observedChanges.m_nodesMoved.empty());
        EXPECT_TRUE(observedChanges.m_nodesResized.empty());
        EXPECT_TRUE(observedChanges.m_connectionsAdded.empty());
        EXPECT_TRUE(observedChanges.m_connectionsRemoved.empty());
        EXPECT_TRUE(observedChanges.m_contentsChanged.empty());
    }
} // namespace

TEST(ProjectObserverTest, nodeRemoved) {
    testNodeRemoved(false);
}

TEST(ProjectObserverTest, nodeRemovedIgnore) {
    testNodeRemoved(true);
}

namespace {
    void testNodeMoved(bool shouldIgnore) {
            testUtils::TestEnvironment testEnvironment;

        const babelwires::NodeId elementId =
            testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());

        testEnvironment.m_project.clearChanges();

        babelwires::ProjectObserver projectObserver(testEnvironment.m_project);
        ObservedChanges observedChanges(projectObserver);

        babelwires::UiPosition newPosition{15, 72};

        testEnvironment.m_project.setNodePosition(elementId, newPosition);
        testEnvironment.m_project.process();

        if (shouldIgnore) {
            projectObserver.ignoreMovedNode(elementId);
        }

        projectObserver.interpretChangesAndFireSignals();

        EXPECT_TRUE(observedChanges.m_nodesAdded.empty());
        EXPECT_TRUE(observedChanges.m_nodesRemoved.empty());

        if (shouldIgnore) {
            EXPECT_TRUE(observedChanges.m_nodesMoved.empty());
        } else {
            ASSERT_EQ(observedChanges.m_nodesMoved.size(), 1);
            EXPECT_EQ(observedChanges.m_nodesMoved[0], std::tuple(elementId, newPosition));
        }

        EXPECT_TRUE(observedChanges.m_nodesResized.empty());
        EXPECT_TRUE(observedChanges.m_connectionsAdded.empty());
        EXPECT_TRUE(observedChanges.m_connectionsRemoved.empty());
        EXPECT_TRUE(observedChanges.m_contentsChanged.empty());
    }
} // namespace

TEST(ProjectObserverTest, nodeMoved) {
    testNodeMoved(false);
}

TEST(ProjectObserverTest, nodeMovedIgnore) {
    testNodeMoved(true);
}

namespace {
    void testNodesResized(bool shouldIgnore) {
            testUtils::TestEnvironment testEnvironment;

        const babelwires::NodeId elementId =
            testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());

        testEnvironment.m_project.clearChanges();

        babelwires::ProjectObserver projectObserver(testEnvironment.m_project);
        ObservedChanges observedChanges(projectObserver);

        babelwires::UiSize newSize{81};

        testEnvironment.m_project.setNodeContentsSize(elementId, newSize);
        testEnvironment.m_project.process();

        if (shouldIgnore) {
            projectObserver.ignoreResizedNode(elementId);
        }

        projectObserver.interpretChangesAndFireSignals();

        EXPECT_TRUE(observedChanges.m_nodesAdded.empty());
        EXPECT_TRUE(observedChanges.m_nodesRemoved.empty());
        EXPECT_TRUE(observedChanges.m_nodesMoved.empty());

        if (shouldIgnore) {
            EXPECT_TRUE(observedChanges.m_nodesResized.empty());
        } else {
            ASSERT_EQ(observedChanges.m_nodesResized.size(), 1);
            EXPECT_EQ(observedChanges.m_nodesResized[0], std::tuple(elementId, newSize));
        }

        EXPECT_TRUE(observedChanges.m_connectionsAdded.empty());
        EXPECT_TRUE(observedChanges.m_connectionsRemoved.empty());
        EXPECT_TRUE(observedChanges.m_contentsChanged.empty());
    }
} // namespace

TEST(ProjectObserverTest, nodeResized) {
    testNodesResized(false);
}

TEST(ProjectObserverTest, nodeResizedIgnore) {
    testNodesResized(true);
}

namespace {
    void testConnectionAdded(bool shouldIgnore, bool sourceRecordIsExpanded, bool targetArrayIsExpanded) {
        testUtils::TestEnvironment testEnvironment;

        testUtils::TestComplexRecordElementData sourceElementData;
        sourceElementData.m_expandedPaths.emplace_back(testUtils::TestComplexRecordElementData::getPathToRecord());
        if (sourceRecordIsExpanded) {
            sourceElementData.m_expandedPaths.emplace_back(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord());
        }
        const babelwires::NodeId sourceNodeId = testEnvironment.m_project.addNode(sourceElementData);

        testUtils::TestComplexRecordElementData targetElementData;
        targetElementData.m_expandedPaths.emplace_back(testUtils::TestComplexRecordElementData::getPathToRecord());
        if (targetArrayIsExpanded) {
            targetElementData.m_expandedPaths.emplace_back(testUtils::TestComplexRecordElementData::getPathToRecordArray());
        }
        const babelwires::NodeId targetNodeId = testEnvironment.m_project.addNode(targetElementData);

        testEnvironment.m_project.process();
        testEnvironment.m_project.clearChanges();

        babelwires::ProjectObserver projectObserver(testEnvironment.m_project);
        ObservedChanges observedChanges(projectObserver);

        // The connection we add.
        babelwires::ConnectionModifierData connectionData;
        connectionData.m_targetPath = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(1);
        connectionData.m_sourcePath = testUtils::TestComplexRecordElementData::getPathToRecordSubrecordInt1();
        connectionData.m_sourceId = sourceNodeId;

        // The connection we expect to observe.
        babelwires::ConnectionDescription connectionDescription(targetNodeId, connectionData);
        if (!sourceRecordIsExpanded) {
            connectionDescription.m_sourcePath.truncate(1);
        }
        if (!targetArrayIsExpanded) {
            connectionDescription.m_targetPath.truncate(1);
        }

        testEnvironment.m_project.addModifier(targetNodeId, connectionData);

        testEnvironment.m_project.process();

        if (shouldIgnore) {
            projectObserver.ignoreAddedConnection(connectionDescription);
        }

        projectObserver.interpretChangesAndFireSignals();

        EXPECT_TRUE(observedChanges.m_nodesAdded.empty());
        EXPECT_TRUE(observedChanges.m_nodesRemoved.empty());
        EXPECT_TRUE(observedChanges.m_nodesMoved.empty());
        EXPECT_TRUE(observedChanges.m_nodesResized.empty());

        if (shouldIgnore) {
            EXPECT_TRUE(observedChanges.m_connectionsAdded.empty());
        } else {
            ASSERT_EQ(observedChanges.m_connectionsAdded.size(), 1);
            EXPECT_EQ(observedChanges.m_connectionsAdded[0], connectionDescription);
        }

        EXPECT_TRUE(observedChanges.m_connectionsRemoved.empty());

        EXPECT_EQ(observedChanges.m_contentsChanged.size(), 1);
        EXPECT_EQ(observedChanges.m_contentsChanged[0], targetNodeId);
    }
} // namespace

TEST(ProjectObserverTest, connectionAddedNoTruncation) {
    testConnectionAdded(false, true, true);
}

TEST(ProjectObserverTest, connectionAddedTargetTruncated) {
    testConnectionAdded(false, true, false);
}

TEST(ProjectObserverTest, connectionAddedSourceTruncated) {
    testConnectionAdded(false, true, false);
}

TEST(ProjectObserverTest, connectionAddedBothTruncated) {
    testConnectionAdded(false, false, false);
}

TEST(ProjectObserverTest, connectionAddedNoTruncationIgnore) {
    testConnectionAdded(true, true, true);
}

TEST(ProjectObserverTest, connectionAddedTargetTruncatedIgnore) {
    testConnectionAdded(true, true, false);
}

TEST(ProjectObserverTest, connectionAddedSourceTruncatedIgnore) {
    testConnectionAdded(true, true, false);
}

TEST(ProjectObserverTest, connectionAddedBothTruncatedIgnore) {
    testConnectionAdded(true, false, false);
}

namespace {
    void testConnectionRemoved(bool shouldIgnore, bool sourceRecordIsExpanded, bool targetArrayIsExpanded) {
            testUtils::TestEnvironment testEnvironment;

        testUtils::TestComplexRecordElementData sourceElementData;
        sourceElementData.m_expandedPaths.emplace_back(testUtils::TestComplexRecordElementData::getPathToRecord());
        if (sourceRecordIsExpanded) {
            sourceElementData.m_expandedPaths.emplace_back(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord());
        }
        const babelwires::NodeId sourceNodeId = testEnvironment.m_project.addNode(sourceElementData);

        // The connection we will remove.
        babelwires::ConnectionModifierData connectionData;
        connectionData.m_targetPath = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(1);
        connectionData.m_sourcePath = testUtils::TestComplexRecordElementData::getPathToRecordSubrecordInt1();
        connectionData.m_sourceId = sourceNodeId;

        testUtils::TestComplexRecordElementData targetElementData;
        targetElementData.m_expandedPaths.emplace_back(testUtils::TestComplexRecordElementData::getPathToRecord());
        targetElementData.m_modifiers.emplace_back(connectionData.clone());
        if (targetArrayIsExpanded) {
            targetElementData.m_expandedPaths.emplace_back(testUtils::TestComplexRecordElementData::getPathToRecordArray());
        }
        const babelwires::NodeId targetNodeId = testEnvironment.m_project.addNode(targetElementData);

        testEnvironment.m_project.process();
        testEnvironment.m_project.clearChanges();

        babelwires::ProjectObserver projectObserver(testEnvironment.m_project);
        ObservedChanges observedChanges(projectObserver);

        // The connection we expect to observe.
        babelwires::ConnectionDescription connectionDescription(targetNodeId, connectionData);
        if (!sourceRecordIsExpanded) {
            connectionDescription.m_sourcePath.truncate(1);
        }
        if (!targetArrayIsExpanded) {
            connectionDescription.m_targetPath.truncate(1);
        }

        testEnvironment.m_project.removeModifier(targetNodeId, connectionData.m_targetPath);

        testEnvironment.m_project.process();

        if (shouldIgnore) {
            projectObserver.ignoreRemovedConnection(connectionDescription);
        }
        projectObserver.interpretChangesAndFireSignals();

        EXPECT_TRUE(observedChanges.m_nodesAdded.empty());
        EXPECT_TRUE(observedChanges.m_nodesRemoved.empty());
        EXPECT_TRUE(observedChanges.m_nodesMoved.empty());
        EXPECT_TRUE(observedChanges.m_nodesResized.empty());
        EXPECT_TRUE(observedChanges.m_connectionsAdded.empty());

        if (shouldIgnore) {
            EXPECT_TRUE(observedChanges.m_connectionsRemoved.empty());
        } else {
            ASSERT_EQ(observedChanges.m_connectionsRemoved.size(), 1);
            EXPECT_EQ(observedChanges.m_connectionsRemoved[0], connectionDescription);
        }

        EXPECT_EQ(observedChanges.m_contentsChanged.size(), 1);
        EXPECT_EQ(observedChanges.m_contentsChanged[0], targetNodeId);
    }
} // namespace

TEST(ProjectObserverTest, connectionRemovedNoTruncation) {
    testConnectionRemoved(false, true, true);
}

TEST(ProjectObserverTest, connectionRemovedTargetTruncated) {
    testConnectionRemoved(false, true, false);
}

TEST(ProjectObserverTest, connectionRemovedSourceTruncated) {
    testConnectionRemoved(false, true, false);
}

TEST(ProjectObserverTest, connectionRemovedBothTruncated) {
    testConnectionRemoved(false, false, false);
}

TEST(ProjectObserverTest, connectionRemovedNoTruncationIgnore) {
    testConnectionRemoved(true, true, true);
}

TEST(ProjectObserverTest, connectionRemovedTargetTruncatedIgnore) {
    testConnectionRemoved(true, true, false);
}

TEST(ProjectObserverTest, connectionRemovedSourceTruncatedIgnore) {
    testConnectionRemoved(true, true, false);
}

TEST(ProjectObserverTest, connectionRemovedBothTruncatedIgnore) {
    testConnectionRemoved(true, false, false);
}

TEST(ProjectObserverTest, nodeContentsChanged) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestComplexRecordElementData sourceElementData;
    sourceElementData.m_expandedPaths.emplace_back(testUtils::TestComplexRecordElementData::getPathToRecord());
    const babelwires::NodeId sourceNodeId = testEnvironment.m_project.addNode(sourceElementData);

    babelwires::ConnectionModifierData connectionData;
    connectionData.m_targetPath = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(1);
    connectionData.m_sourcePath = testUtils::TestComplexRecordElementData::getPathToRecordSubrecordInt1();
    connectionData.m_sourceId = sourceNodeId;

    testUtils::TestComplexRecordElementData targetElementData;
    targetElementData.m_expandedPaths.emplace_back(testUtils::TestComplexRecordElementData::getPathToRecord());
    targetElementData.m_modifiers.emplace_back(connectionData.clone());
    const babelwires::NodeId targetNodeId = testEnvironment.m_project.addNode(targetElementData);

    testEnvironment.m_project.process();
    testEnvironment.m_project.clearChanges();

    babelwires::ProjectObserver projectObserver(testEnvironment.m_project);
    ObservedChanges observedChanges(projectObserver);

    babelwires::ValueAssignmentData intData(babelwires::IntValue(14));
    intData.m_targetPath = testUtils::TestComplexRecordElementData::getPathToRecordSubrecordInt1();
    testEnvironment.m_project.addModifier(sourceNodeId, intData);

    testEnvironment.m_project.process();

    projectObserver.interpretChangesAndFireSignals();

    EXPECT_TRUE(observedChanges.m_nodesAdded.empty());
    EXPECT_TRUE(observedChanges.m_nodesRemoved.empty());
    EXPECT_TRUE(observedChanges.m_nodesMoved.empty());
    EXPECT_TRUE(observedChanges.m_nodesResized.empty());
    EXPECT_TRUE(observedChanges.m_connectionsAdded.empty());
    EXPECT_TRUE(observedChanges.m_connectionsRemoved.empty());

    EXPECT_EQ(observedChanges.m_contentsChanged.size(), 2);

    // The source is changed directly by the new modifier.
    // The target is changed indirectly via the connection.
    std::vector<babelwires::NodeId> expectedChanges = {sourceNodeId, targetNodeId};

    EXPECT_TRUE(testUtils::areEqualSets(observedChanges.m_contentsChanged, expectedChanges));
}
