#include <gtest/gtest.h>

#include "BabelWiresLib/ProjectExtra/projectObserver.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifierData.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"
#include "Tests/TestUtils/equalSets.hpp"

namespace {
    /// Capture the data from the callbacks, to make it easy to detect.
    /// Very roughly, this class is the inverse of the ProjectObserver.
    struct ObservedChanges {
        ObservedChanges(babelwires::ProjectObserver& observer) {
            m_featureElementsAddedSubscription = observer.m_featureElementWasAdded.subscribe(
                [this](const babelwires::FeatureElement* element) { m_featureElementsAdded.emplace_back(element); });
            m_featureElementWasRemovedSubscription = observer.m_featureElementWasRemoved.subscribe(
                [this](const babelwires::ElementId elementId) { m_featureElementsRemoved.emplace_back(elementId); });
            m_featureElementWasMovedSubscription = observer.m_featureElementWasMoved.subscribe(
                [this](babelwires::ElementId elementId, const babelwires::UiPosition& position) {
                    m_featureElementsMoved.emplace_back(std::tuple(elementId, position));
                });
            m_featureElementWasResizedSubscription = observer.m_featureElementWasResized.subscribe(
                [this](babelwires::ElementId elementId, const babelwires::UiSize& size) {
                    m_featureElementsResized.emplace_back(std::tuple(elementId, size));
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
                [this](const babelwires::ElementId elementId) { m_contentsChanged.emplace_back(elementId); });
        }

        void clear() { m_featureElementsAdded.clear(); }

        babelwires::SignalSubscription m_featureElementsAddedSubscription;
        babelwires::SignalSubscription m_featureElementWasRemovedSubscription;
        babelwires::SignalSubscription m_featureElementWasMovedSubscription;
        babelwires::SignalSubscription m_featureElementWasResizedSubscription;
        babelwires::SignalSubscription m_connectionWasAddedSubscription;
        babelwires::SignalSubscription m_connectionWasRemovedSubscription;
        babelwires::SignalSubscription m_contentWasChangedSubscription;

        std::vector<const babelwires::FeatureElement*> m_featureElementsAdded;
        std::vector<babelwires::ElementId> m_featureElementsRemoved;
        std::vector<std::tuple<babelwires::ElementId, babelwires::UiPosition>> m_featureElementsMoved;
        std::vector<std::tuple<babelwires::ElementId, babelwires::UiSize>> m_featureElementsResized;
        std::vector<babelwires::ConnectionDescription> m_connectionsAdded;
        std::vector<babelwires::ConnectionDescription> m_connectionsRemoved;
        std::vector<babelwires::ElementId> m_contentsChanged;
    };
} // namespace

namespace {
    void testFeatureElementAdded(bool shouldIgnore) {
        babelwires::IdentifierRegistryScope identifierRegistry;
        libTestUtils::TestProjectContext context;

        babelwires::ProjectObserver projectObserver(context.m_project);
        ObservedChanges observedChanges(projectObserver);

        const babelwires::ElementId elementId =
            context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

        if (shouldIgnore) {
            projectObserver.ignoreAddedElement(elementId);
        }

        context.m_project.process();
        projectObserver.interpretChangesAndFireSignals();

        if (shouldIgnore) {
            EXPECT_TRUE(observedChanges.m_featureElementsAdded.empty());
        } else {
            ASSERT_EQ(observedChanges.m_featureElementsAdded.size(), 1);
            EXPECT_EQ(observedChanges.m_featureElementsAdded[0]->getElementId(), elementId);
        }

        EXPECT_TRUE(observedChanges.m_featureElementsRemoved.empty());
        EXPECT_TRUE(observedChanges.m_featureElementsMoved.empty());
        EXPECT_TRUE(observedChanges.m_featureElementsResized.empty());
        EXPECT_TRUE(observedChanges.m_connectionsAdded.empty());
        EXPECT_TRUE(observedChanges.m_connectionsRemoved.empty());
        EXPECT_TRUE(observedChanges.m_contentsChanged.empty());
    }
} // namespace

TEST(ProjectObserverTest, featureElementAdded) {
    testFeatureElementAdded(false);
}

TEST(ProjectObserverTest, featureElementAddedIgnore) {
    testFeatureElementAdded(false);
}

namespace {
    void testFeatureElementRemoved(bool shouldIgnore) {
        babelwires::IdentifierRegistryScope identifierRegistry;
        libTestUtils::TestProjectContext context;

        const babelwires::ElementId elementId =
            context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

        context.m_project.process();
        context.m_project.clearChanges();

        babelwires::ProjectObserver projectObserver(context.m_project);
        ObservedChanges observedChanges(projectObserver);

        context.m_project.removeElement(elementId);
        context.m_project.process();

        if (shouldIgnore) {
            projectObserver.ignoreRemovedElement(elementId);
        }

        projectObserver.interpretChangesAndFireSignals();

        EXPECT_TRUE(observedChanges.m_featureElementsAdded.empty());

        if (shouldIgnore) {
            EXPECT_TRUE(observedChanges.m_featureElementsRemoved.empty());
        } else {
            ASSERT_EQ(observedChanges.m_featureElementsRemoved.size(), 1);
            EXPECT_EQ(observedChanges.m_featureElementsRemoved[0], elementId);
        }

        EXPECT_TRUE(observedChanges.m_featureElementsMoved.empty());
        EXPECT_TRUE(observedChanges.m_featureElementsResized.empty());
        EXPECT_TRUE(observedChanges.m_connectionsAdded.empty());
        EXPECT_TRUE(observedChanges.m_connectionsRemoved.empty());
        EXPECT_TRUE(observedChanges.m_contentsChanged.empty());
    }
} // namespace

TEST(ProjectObserverTest, featureElementRemoved) {
    testFeatureElementRemoved(false);
}

TEST(ProjectObserverTest, featureElementRemovedIgnore) {
    testFeatureElementRemoved(true);
}

namespace {
    void testFeatureElementMoved(bool shouldIgnore) {
        babelwires::IdentifierRegistryScope identifierRegistry;
        libTestUtils::TestProjectContext context;

        const babelwires::ElementId elementId =
            context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

        context.m_project.clearChanges();

        babelwires::ProjectObserver projectObserver(context.m_project);
        ObservedChanges observedChanges(projectObserver);

        babelwires::UiPosition newPosition{15, 72};

        context.m_project.setElementPosition(elementId, newPosition);
        context.m_project.process();

        if (shouldIgnore) {
            projectObserver.ignoreMovedElement(elementId);
        }

        projectObserver.interpretChangesAndFireSignals();

        EXPECT_TRUE(observedChanges.m_featureElementsAdded.empty());
        EXPECT_TRUE(observedChanges.m_featureElementsRemoved.empty());

        if (shouldIgnore) {
            EXPECT_TRUE(observedChanges.m_featureElementsMoved.empty());
        } else {
            ASSERT_EQ(observedChanges.m_featureElementsMoved.size(), 1);
            EXPECT_EQ(observedChanges.m_featureElementsMoved[0], std::tuple(elementId, newPosition));
        }

        EXPECT_TRUE(observedChanges.m_featureElementsResized.empty());
        EXPECT_TRUE(observedChanges.m_connectionsAdded.empty());
        EXPECT_TRUE(observedChanges.m_connectionsRemoved.empty());
        EXPECT_TRUE(observedChanges.m_contentsChanged.empty());
    }
} // namespace

TEST(ProjectObserverTest, featureElementMoved) {
    testFeatureElementMoved(false);
}

TEST(ProjectObserverTest, featureElementMovedIgnore) {
    testFeatureElementMoved(true);
}

namespace {
    void testFeatureElementsResized(bool shouldIgnore) {
        babelwires::IdentifierRegistryScope identifierRegistry;
        libTestUtils::TestProjectContext context;

        const babelwires::ElementId elementId =
            context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

        context.m_project.clearChanges();

        babelwires::ProjectObserver projectObserver(context.m_project);
        ObservedChanges observedChanges(projectObserver);

        babelwires::UiSize newSize{81};

        context.m_project.setElementContentsSize(elementId, newSize);
        context.m_project.process();

        if (shouldIgnore) {
            projectObserver.ignoreResizedElement(elementId);
        }

        projectObserver.interpretChangesAndFireSignals();

        EXPECT_TRUE(observedChanges.m_featureElementsAdded.empty());
        EXPECT_TRUE(observedChanges.m_featureElementsRemoved.empty());
        EXPECT_TRUE(observedChanges.m_featureElementsMoved.empty());

        if (shouldIgnore) {
            EXPECT_TRUE(observedChanges.m_featureElementsResized.empty());
        } else {
            ASSERT_EQ(observedChanges.m_featureElementsResized.size(), 1);
            EXPECT_EQ(observedChanges.m_featureElementsResized[0], std::tuple(elementId, newSize));
        }

        EXPECT_TRUE(observedChanges.m_connectionsAdded.empty());
        EXPECT_TRUE(observedChanges.m_connectionsRemoved.empty());
        EXPECT_TRUE(observedChanges.m_contentsChanged.empty());
    }
} // namespace

TEST(ProjectObserverTest, featureElementResized) {
    testFeatureElementsResized(false);
}

TEST(ProjectObserverTest, featureElementResizedIgnore) {
    testFeatureElementsResized(true);
}

namespace {
    void testConnectionAdded(bool shouldIgnore, bool sourceRecordIsExpanded, bool targetArrayIsExpanded) {
        babelwires::IdentifierRegistryScope identifierRegistry;
        libTestUtils::TestProjectContext context;

        libTestUtils::TestFeatureElementData sourceElementData;
        if (sourceRecordIsExpanded) {
            sourceElementData.m_expandedPaths.emplace_back(libTestUtils::TestRootFeature::s_pathToRecord);
        }
        const babelwires::ElementId sourceElementId = context.m_project.addFeatureElement(sourceElementData);

        libTestUtils::TestFeatureElementData targetElementData;
        if (targetArrayIsExpanded) {
            targetElementData.m_expandedPaths.emplace_back(libTestUtils::TestRootFeature::s_pathToArray);
        }
        const babelwires::ElementId targetElementId = context.m_project.addFeatureElement(targetElementData);

        context.m_project.process();
        context.m_project.clearChanges();

        babelwires::ProjectObserver projectObserver(context.m_project);
        ObservedChanges observedChanges(projectObserver);

        // The connection we add.
        babelwires::ConnectionModifierData connectionData;
        connectionData.m_pathToFeature = libTestUtils::TestRootFeature::s_pathToArray_1;
        connectionData.m_pathToSourceFeature = libTestUtils::TestRootFeature::s_pathToInt2;
        connectionData.m_sourceId = sourceElementId;

        // The connection we expect to observe.
        babelwires::ConnectionDescription connectionDescription(targetElementId, connectionData);
        if (!sourceRecordIsExpanded) {
            connectionDescription.m_pathToSourceFeature.truncate(1);
        }
        if (!targetArrayIsExpanded) {
            connectionDescription.m_pathToTargetFeature.truncate(1);
        }

        context.m_project.addModifier(targetElementId, connectionData);

        context.m_project.process();

        if (shouldIgnore) {
            projectObserver.ignoreAddedConnection(connectionDescription);
        }

        projectObserver.interpretChangesAndFireSignals();

        EXPECT_TRUE(observedChanges.m_featureElementsAdded.empty());
        EXPECT_TRUE(observedChanges.m_featureElementsRemoved.empty());
        EXPECT_TRUE(observedChanges.m_featureElementsMoved.empty());
        EXPECT_TRUE(observedChanges.m_featureElementsResized.empty());

        if (shouldIgnore) {
            EXPECT_TRUE(observedChanges.m_connectionsAdded.empty());
        } else {
            ASSERT_EQ(observedChanges.m_connectionsAdded.size(), 1);
            EXPECT_EQ(observedChanges.m_connectionsAdded[0], connectionDescription);
        }

        EXPECT_TRUE(observedChanges.m_connectionsRemoved.empty());

        EXPECT_EQ(observedChanges.m_contentsChanged.size(), 1);
        EXPECT_EQ(observedChanges.m_contentsChanged[0], targetElementId);
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
        babelwires::IdentifierRegistryScope identifierRegistry;
        libTestUtils::TestProjectContext context;

        libTestUtils::TestFeatureElementData sourceElementData;
        if (sourceRecordIsExpanded) {
            sourceElementData.m_expandedPaths.emplace_back(libTestUtils::TestRootFeature::s_pathToRecord);
        }
        const babelwires::ElementId sourceElementId = context.m_project.addFeatureElement(sourceElementData);

        // The connection we will remove.
        babelwires::ConnectionModifierData connectionData;
        connectionData.m_pathToFeature = libTestUtils::TestRootFeature::s_pathToArray_1;
        connectionData.m_pathToSourceFeature = libTestUtils::TestRootFeature::s_pathToInt2;
        connectionData.m_sourceId = sourceElementId;

        libTestUtils::TestFeatureElementData targetElementData;
        targetElementData.m_modifiers.emplace_back(connectionData.clone());
        if (targetArrayIsExpanded) {
            targetElementData.m_expandedPaths.emplace_back(libTestUtils::TestRootFeature::s_pathToArray);
        }
        const babelwires::ElementId targetElementId = context.m_project.addFeatureElement(targetElementData);

        context.m_project.process();
        context.m_project.clearChanges();

        babelwires::ProjectObserver projectObserver(context.m_project);
        ObservedChanges observedChanges(projectObserver);

        // The connection we expect to observe.
        babelwires::ConnectionDescription connectionDescription(targetElementId, connectionData);
        if (!sourceRecordIsExpanded) {
            connectionDescription.m_pathToSourceFeature.truncate(1);
        }
        if (!targetArrayIsExpanded) {
            connectionDescription.m_pathToTargetFeature.truncate(1);
        }

        context.m_project.removeModifier(targetElementId, connectionData.m_pathToFeature);

        context.m_project.process();

        if (shouldIgnore) {
            projectObserver.ignoreRemovedConnection(connectionDescription);
        }
        projectObserver.interpretChangesAndFireSignals();

        EXPECT_TRUE(observedChanges.m_featureElementsAdded.empty());
        EXPECT_TRUE(observedChanges.m_featureElementsRemoved.empty());
        EXPECT_TRUE(observedChanges.m_featureElementsMoved.empty());
        EXPECT_TRUE(observedChanges.m_featureElementsResized.empty());
        EXPECT_TRUE(observedChanges.m_connectionsAdded.empty());

        if (shouldIgnore) {
            EXPECT_TRUE(observedChanges.m_connectionsRemoved.empty());
        } else {
            ASSERT_EQ(observedChanges.m_connectionsRemoved.size(), 1);
            EXPECT_EQ(observedChanges.m_connectionsRemoved[0], connectionDescription);
        }

        EXPECT_EQ(observedChanges.m_contentsChanged.size(), 1);
        EXPECT_EQ(observedChanges.m_contentsChanged[0], targetElementId);
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

TEST(ProjectObserverTest, featureElementContentsChanged) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestFeatureElementData sourceElementData;
    const babelwires::ElementId sourceElementId = context.m_project.addFeatureElement(sourceElementData);

    babelwires::ConnectionModifierData connectionData;
    connectionData.m_pathToFeature = libTestUtils::TestRootFeature::s_pathToArray_1;
    connectionData.m_pathToSourceFeature = libTestUtils::TestRootFeature::s_pathToInt2;
    connectionData.m_sourceId = sourceElementId;

    libTestUtils::TestFeatureElementData targetElementData;
    targetElementData.m_modifiers.emplace_back(connectionData.clone());
    const babelwires::ElementId targetElementId = context.m_project.addFeatureElement(targetElementData);

    context.m_project.process();
    context.m_project.clearChanges();

    babelwires::ProjectObserver projectObserver(context.m_project);
    ObservedChanges observedChanges(projectObserver);

    babelwires::IntValueAssignmentData intData;
    intData.m_pathToFeature = libTestUtils::TestRootFeature::s_pathToInt2;
    intData.m_value = 14;
    context.m_project.addModifier(sourceElementId, intData);

    context.m_project.process();

    projectObserver.interpretChangesAndFireSignals();

    EXPECT_TRUE(observedChanges.m_featureElementsAdded.empty());
    EXPECT_TRUE(observedChanges.m_featureElementsRemoved.empty());
    EXPECT_TRUE(observedChanges.m_featureElementsMoved.empty());
    EXPECT_TRUE(observedChanges.m_featureElementsResized.empty());
    EXPECT_TRUE(observedChanges.m_connectionsAdded.empty());
    EXPECT_TRUE(observedChanges.m_connectionsRemoved.empty());

    EXPECT_EQ(observedChanges.m_contentsChanged.size(), 2);

    // The source is changed directly by the new modifier.
    // The target is changed indirectly via the connection.
    std::vector<babelwires::ElementId> expectedChanges = {sourceElementId, targetElementId};

    EXPECT_TRUE(testUtils::areEqualSets(observedChanges.m_contentsChanged, expectedChanges));
}
