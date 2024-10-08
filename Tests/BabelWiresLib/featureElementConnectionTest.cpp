#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>
#include <BabelWiresLib/Features/simpleValueFeature.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Identifiers/registeredIdentifier.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>
#include <Tests/TestUtils/testLog.hpp>
#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>

struct FeatureElementConnectionTest : ::testing::Test {
  protected:
    void SetUp() override {
        testUtils::TestComplexRecordElementData featureElementData;

        m_elementId = m_context.m_project.addFeatureElement(featureElementData);
        m_featureElement = m_context.m_project.getFeatureElement(m_elementId);

        static_assert(testUtils::TestSimpleArrayType::s_nonDefaultSize > testUtils::TestSimpleArrayType::s_defaultSize);
        static_assert(testUtils::TestSimpleArrayType::s_defaultSize > 0);

        m_arrayPath = testUtils::TestComplexRecordElementData::getPathToRecordArray();
        m_arrayElemPath = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(testUtils::TestSimpleArrayType::s_nonDefaultSize - 1);
        m_targetPath = testUtils::TestComplexRecordElementData::getPathToRecordInt1();

        // Add a feature element to the project to which connections can be made.
        testUtils::TestComplexRecordElementData sourceElementData;

        m_sourceId = m_context.m_project.addFeatureElement(sourceElementData);
        m_sourceElement = m_context.m_project.getFeatureElement(m_sourceId);
        ASSERT_TRUE(m_sourceElement);

        m_arrayInitData.m_pathToFeature = m_arrayPath;
        m_arrayInitData.m_size = testUtils::TestSimpleArrayType::s_nonDefaultSize;

        // The target int has a limit range, but this value is within the range.
        m_arrayElemData = babelwires::ValueAssignmentData(babelwires::IntValue(4));
        m_arrayElemData.m_pathToFeature = m_arrayElemPath;

        // The target int has a limit range, and this exceeds that range.
        m_arrayElemDataHigh = babelwires::ValueAssignmentData(babelwires::IntValue(700));
        m_arrayElemDataHigh.m_pathToFeature = m_arrayElemPath;

        m_assignData.m_pathToFeature = m_targetPath;
        m_assignData.m_pathToSourceFeature = m_arrayElemPath;
        m_assignData.m_sourceId = m_sourceId;
    }

    void TearDown() override { 
        m_context.m_project.removeElement(m_elementId);
        m_context.m_project.removeElement(m_sourceId);
    }

    /// If a non-zero sourceValue is provided, a modifier is added at the source feature.
    babelwires::ConnectionModifier* setUpConnectionModifier(int sourceValue) {
        m_context.m_project.addModifier(m_sourceId, m_arrayInitData);
        if (sourceValue != 0) {
            babelwires::ValueAssignmentData sourceData{babelwires::IntValue(sourceValue)};
            sourceData.m_pathToFeature = m_arrayElemPath;
            m_context.m_project.addModifier(m_sourceId, sourceData);
        }

        m_context.m_project.addModifier(m_elementId, m_assignData);
        return m_featureElement->findModifier(m_assignData.m_pathToFeature)->asConnectionModifier(); 
    }

    testUtils::TestEnvironment m_context;
    babelwires::FeaturePath m_arrayPath;
    babelwires::FeaturePath m_arrayElemPath;
    babelwires::FeaturePath m_targetPath;

    babelwires::ElementId m_elementId;
    babelwires::FeatureElement* m_featureElement;
    babelwires::ElementId m_sourceId;
    babelwires::FeatureElement* m_sourceElement;

    babelwires::ConnectionModifierData m_assignData;
    babelwires::ArraySizeModifierData m_arrayInitData;
    babelwires::ValueAssignmentData m_arrayElemData;
    babelwires::ValueAssignmentData m_arrayElemDataHigh;
};

TEST_F(FeatureElementConnectionTest, addAConnection) {
    m_context.m_project.addModifier(m_sourceId, m_arrayInitData);
    m_context.m_project.addModifier(m_sourceId, m_arrayElemData);
    m_context.m_project.process();

    m_context.m_project.clearChanges();
    m_context.m_project.addModifier(m_elementId, m_assignData);
    m_context.m_project.process();

    babelwires::Modifier* modifier = m_featureElement->findModifier(m_assignData.m_pathToFeature);
    EXPECT_FALSE(modifier->isFailed());
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierAdded));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
}

TEST_F(FeatureElementConnectionTest, changeSourceValueFromDefault) {
    m_context.m_project.addModifier(m_sourceId, m_arrayInitData);
    m_context.m_project.addModifier(m_elementId, m_assignData);
    m_context.m_project.process();

    m_context.m_project.clearChanges();
    m_context.m_project.addModifier(m_sourceId, m_arrayElemData);
    m_context.m_project.process();

    babelwires::Modifier* modifier = m_featureElement->findModifier(m_assignData.m_pathToFeature);
    EXPECT_FALSE(modifier->isFailed());
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierFailed));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierConnected));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierDisconnected));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
}

TEST_F(FeatureElementConnectionTest, changeSourceValueToDefault) {
    m_context.m_project.addModifier(m_sourceId, m_arrayInitData);
    m_context.m_project.addModifier(m_sourceId, m_arrayElemData);
    m_context.m_project.addModifier(m_elementId, m_assignData);
    m_context.m_project.process();

    m_context.m_project.clearChanges();
    m_context.m_project.removeModifier(m_sourceId, m_arrayElemPath);
    m_context.m_project.process();
    
    babelwires::Modifier* modifier = m_featureElement->findModifier(m_assignData.m_pathToFeature);
    EXPECT_FALSE(modifier->isFailed());
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierFailed));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierConnected));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierDisconnected));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
}

TEST_F(FeatureElementConnectionTest, removedAndRestoreSourceFeature) {
    m_context.m_project.addModifier(m_sourceId, m_arrayInitData);
    m_context.m_project.addModifier(m_sourceId, m_arrayElemData);
    m_context.m_project.addModifier(m_elementId, m_assignData);
    m_context.m_project.process();

    m_context.m_project.clearChanges();
    m_context.m_project.removeModifier(m_sourceId, m_arrayElemPath);
    m_context.m_project.removeModifier(m_sourceId, m_arrayPath);
    m_context.m_project.process();

    babelwires::Modifier* modifier = m_featureElement->findModifier(m_assignData.m_pathToFeature);
    EXPECT_TRUE(modifier->isFailed());
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierFailed));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierConnected));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierDisconnected));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    m_context.m_project.clearChanges();
    m_context.m_project.addModifier(m_sourceId, m_arrayInitData);
    m_context.m_project.addModifier(m_sourceId, m_arrayElemData);
    m_context.m_project.process();

    EXPECT_FALSE(modifier->isFailed());
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierRecovered));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierConnected));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierDisconnected));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
}

TEST_F(FeatureElementConnectionTest, failedButStillConnected) {
    m_context.m_project.addModifier(m_sourceId, m_arrayInitData);
    m_context.m_project.addModifier(m_sourceId, m_arrayElemDataHigh);
    m_context.m_project.process();

    m_context.m_project.clearChanges();
    m_context.m_project.addModifier(m_elementId, m_assignData);
    m_context.m_project.process();

    babelwires::Modifier* modifier = m_featureElement->findModifier(m_assignData.m_pathToFeature);
    EXPECT_TRUE(modifier->isFailed());
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierFailed));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierConnected));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierDisconnected));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    m_context.m_project.clearChanges();
    m_context.m_project.removeModifier(m_sourceId, m_arrayElemPath);
    m_context.m_project.addModifier(m_sourceId, m_arrayElemData);
    m_context.m_project.process();

    EXPECT_FALSE(modifier->isFailed());
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierRecovered));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierConnected));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierDisconnected));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
}