#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Identifiers/registeredIdentifier.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecord.hpp>
#include <Tests/TestUtils/testLog.hpp>

struct FeatureElementConnectionTest : ::testing::Test {
  protected:
    void SetUp() override {
        testUtils::TestFeatureElementData featureElementData;
        featureElementData.m_intValueLimit = 255;

        auto featureElement = featureElementData.createFeatureElement(m_context.m_projectContext, m_context.m_log, 66);
        m_featureElement = std::unique_ptr<testUtils::TestFeatureElement>(
            featureElement.release()->as<testUtils::TestFeatureElement>());
        ASSERT_TRUE(m_featureElement);
        m_featureElement->clearChanges();

        m_arrayPath =
            babelwires::FeaturePath::deserializeFromString(testUtils::TestRecordFeature::s_arrayIdInitializer);
        m_arrayElemPath = babelwires::FeaturePath::deserializeFromString(
            std::string(testUtils::TestRecordFeature::s_arrayIdInitializer) + "/3");
        m_arrayElemPath2 = babelwires::FeaturePath::deserializeFromString(
            std::string(testUtils::TestRecordFeature::s_arrayIdInitializer) + "/1");

        // Add a feature element to the project to which connections can be made.
        testUtils::TestFeatureElementData sourceElementData;
        // The ints in the source can carry higher values than those in the target.
        sourceElementData.m_intValueLimit = 1000;
        m_sourceId = m_context.m_project.addFeatureElement(sourceElementData);
        m_sourceElement = m_context.m_project.getFeatureElement(m_sourceId)->as<testUtils::TestFeatureElement>();
        ASSERT_TRUE(m_sourceElement);

        m_arrayInitData.m_pathToFeature = m_arrayPath;
        m_arrayInitData.m_size = 5;

        m_arrayElemData = babelwires::ValueAssignmentData(babelwires::IntValue(16));
        m_arrayElemData.m_pathToFeature = m_arrayElemPath;

        m_arrayElemDataHigh = babelwires::ValueAssignmentData(babelwires::IntValue(700));
        m_arrayElemDataHigh.m_pathToFeature = m_arrayElemPath;

        m_assignData.m_pathToFeature = m_arrayElemPath2;
        m_assignData.m_pathToSourceFeature = m_arrayElemPath;
        m_assignData.m_sourceId = m_sourceId;
    }

    void TearDown() override { m_context.m_project.removeElement(m_sourceId); }

    /// If a non-zero sourceValue is provided, a modifier is added at the source feature.
    babelwires::ConnectionModifier* setUpConnectionModifier(int sourceValue) {
        m_sourceElement->addModifier(m_context.m_log, m_arrayInitData);
        if (sourceValue != 0) {
            babelwires::ValueAssignmentData sourceData{babelwires::IntValue(sourceValue)};
            sourceData.m_pathToFeature = m_arrayElemPath;
            m_sourceElement->addModifier(m_context.m_log, sourceData);
        }

        babelwires::Modifier* modifier = m_featureElement->addModifier(m_context.m_log, m_assignData);
        EXPECT_TRUE(modifier->asConnectionModifier());
        babelwires::Feature* inputFeature = m_featureElement->getInputFeatureNonConst(modifier->getPathToFeature());
        modifier->asConnectionModifier()->applyConnection(m_context.m_project, m_context.m_log, inputFeature);

        m_featureElement->clearChanges();
        m_sourceElement->clearChanges();

        return modifier->asConnectionModifier();
    }

    testUtils::TestEnvironment m_context;
    babelwires::FeaturePath m_arrayPath;
    babelwires::FeaturePath m_arrayElemPath;
    babelwires::FeaturePath m_arrayElemPath2;

    babelwires::ElementId m_sourceId;
    std::unique_ptr<testUtils::TestFeatureElement> m_featureElement;
    testUtils::TestFeatureElement* m_sourceElement;

    babelwires::ConnectionModifierData m_assignData;
    babelwires::ArraySizeModifierData m_arrayInitData;
    babelwires::ValueAssignmentData m_arrayElemData;
    babelwires::ValueAssignmentData m_arrayElemDataHigh;
};

TEST_F(FeatureElementConnectionTest, addAConnection) {
    m_featureElement->clearChanges();
    m_sourceElement->addModifier(m_context.m_log, m_arrayInitData);
    m_sourceElement->addModifier(m_context.m_log, m_arrayElemData);

    babelwires::Modifier* modifier = m_featureElement->addModifier(m_context.m_log, m_assignData);
    ASSERT_TRUE(modifier->asConnectionModifier());
    babelwires::Feature* inputFeature = m_featureElement->getInputFeatureNonConst(modifier->getPathToFeature());
    modifier->asConnectionModifier()->applyConnection(m_context.m_project, m_context.m_log, inputFeature);
    EXPECT_FALSE(modifier->isFailed());
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierAdded));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
}

TEST_F(FeatureElementConnectionTest, changeSourceValueFromDefault) {
    auto modifier = setUpConnectionModifier(0);

    m_sourceElement->addModifier(m_context.m_log, m_arrayElemData);
    babelwires::Feature* inputFeature = m_featureElement->getInputFeatureNonConst(modifier->getPathToFeature());
    modifier->asConnectionModifier()->applyConnection(m_context.m_project, m_context.m_log, inputFeature);

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
    auto modifier = setUpConnectionModifier(1);

    m_sourceElement->removeModifier(m_sourceElement->findModifier(m_arrayElemPath));
    babelwires::Feature* inputFeature = m_featureElement->getInputFeatureNonConst(modifier->getPathToFeature());
    modifier->asConnectionModifier()->applyConnection(m_context.m_project, m_context.m_log, inputFeature);

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
    auto modifier = setUpConnectionModifier(1);

    m_featureElement->clearChanges();
    m_sourceElement->clearChanges();
    m_sourceElement->removeModifier(m_sourceElement->findModifier(m_arrayElemPath));
    m_sourceElement->removeModifier(m_sourceElement->findModifier(m_arrayPath));
    babelwires::Feature* inputFeature = m_featureElement->getInputFeatureNonConst(modifier->getPathToFeature());
    modifier->asConnectionModifier()->applyConnection(m_context.m_project, m_context.m_log, inputFeature);

    EXPECT_TRUE(modifier->isFailed());
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierFailed));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierConnected));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierDisconnected));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    m_featureElement->clearChanges();
    m_sourceElement->clearChanges();
    m_sourceElement->addModifier(m_context.m_log, m_arrayInitData);
    m_sourceElement->addModifier(m_context.m_log, m_arrayElemData);
    inputFeature = m_featureElement->getInputFeatureNonConst(modifier->getPathToFeature());
    modifier->asConnectionModifier()->applyConnection(m_context.m_project, m_context.m_log, inputFeature);

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
    auto modifier = setUpConnectionModifier(10);

    m_sourceElement->removeModifier(m_sourceElement->findModifier(m_arrayElemPath));
    m_sourceElement->addModifier(m_context.m_log, m_arrayElemDataHigh);
    babelwires::Feature* inputFeature = m_featureElement->getInputFeatureNonConst(modifier->getPathToFeature());
    modifier->asConnectionModifier()->applyConnection(m_context.m_project, m_context.m_log, inputFeature);

    EXPECT_TRUE(modifier->isFailed());
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierFailed));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierConnected));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierDisconnected));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    m_featureElement->clearChanges();
    m_sourceElement->clearChanges();
    m_sourceElement->removeModifier(m_sourceElement->findModifier(m_arrayElemPath));
    m_sourceElement->addModifier(m_context.m_log, m_arrayElemData);
    inputFeature = m_featureElement->getInputFeatureNonConst(modifier->getPathToFeature());
    modifier->asConnectionModifier()->applyConnection(m_context.m_project, m_context.m_log, inputFeature);

    EXPECT_FALSE(modifier->isFailed());
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierRecovered));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierConnected));
    EXPECT_FALSE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierDisconnected));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(m_featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
}