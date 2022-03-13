#include <gtest/gtest.h>

#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/processorElementData.hpp"
#include "BabelWiresLib/Project/FeatureElements/processorElement.hpp"
#include "BabelWiresLib/Features/numericFeature.hpp"
#include "BabelWiresLib/Features/rootFeature.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"

#include "Tests/BabelWiresLib/TestUtils/testProcessor.hpp"
#include "Tests/BabelWiresLib/TestUtils/testEnvironment.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

#include "Tests/TestUtils/tempFilePath.hpp"

TEST(ProcessorElementTest, sourceFileDataCreateElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    babelwires::ProcessorElementData data;
    data.m_factoryIdentifier = libTestUtils::TestProcessorFactory::getThisIdentifier();
    data.m_factoryVersion = 1;

    auto featureElement = data.createFeatureElement(context.m_projectContext, context.m_log, 10);
    ASSERT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    ASSERT_TRUE(featureElement->as<babelwires::ProcessorElement>());
    babelwires::ProcessorElement* processorElement = static_cast<babelwires::ProcessorElement*>(featureElement.get());

    const babelwires::RecordFeature* outputFeature = processorElement->getOutputFeature();
    ASSERT_TRUE(outputFeature->as<const libTestUtils::TestRootFeature>());
    const libTestUtils::TestRootFeature* outputTestRecordFeature =
        static_cast<const libTestUtils::TestRootFeature*>(outputFeature);
    EXPECT_EQ(outputTestRecordFeature->m_arrayFeature->getNumFeatures(), 2);

    const babelwires::FeaturePath arraySettingIntPath = libTestUtils::TestRootFeature::s_pathToInt;
    const babelwires::FeaturePath valueSettingIntPath = libTestUtils::TestRootFeature::s_pathToInt2;

    babelwires::IntValueAssignmentData valueSettingData;
    valueSettingData.m_pathToFeature = valueSettingIntPath;
    valueSettingData.m_value = 4;

    processorElement->clearChanges();
    processorElement->addModifier(context.m_log, valueSettingData);
    processorElement->process(context.m_log);

    // The default.
    EXPECT_EQ(outputTestRecordFeature->m_arrayFeature->getNumFeatures(), 2);
    EXPECT_FALSE(processorElement->isFailed());
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_FALSE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    babelwires::IntValueAssignmentData arraySettingData;
    arraySettingData.m_pathToFeature = arraySettingIntPath;
    arraySettingData.m_value = 4;

    processorElement->clearChanges();
    processorElement->addModifier(context.m_log, arraySettingData);
    processorElement->process(context.m_log);

    EXPECT_EQ(outputTestRecordFeature->m_arrayFeature->getNumFeatures(), 6);
    EXPECT_FALSE(processorElement->isFailed());
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    babelwires::IntValueAssignmentData badArraySettingData;
    badArraySettingData.m_pathToFeature = arraySettingIntPath;
    badArraySettingData.m_value = -1;

    processorElement->clearChanges();
    processorElement->removeModifier(processorElement->findModifier(arraySettingIntPath));
    processorElement->addModifier(context.m_log, badArraySettingData);
    processorElement->process(context.m_log);

    EXPECT_TRUE(processorElement->isFailed());
    EXPECT_EQ(outputTestRecordFeature->m_arrayFeature->getNumFeatures(), 2);
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementFailed));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    processorElement->clearChanges();
    processorElement->removeModifier(processorElement->findModifier(arraySettingIntPath));
    processorElement->addModifier(context.m_log, arraySettingData);
    processorElement->process(context.m_log);

    EXPECT_FALSE(processorElement->isFailed());
    EXPECT_EQ(outputTestRecordFeature->m_arrayFeature->getNumFeatures(), 6);
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementRecovered));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
}
