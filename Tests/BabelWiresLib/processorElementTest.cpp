#include <gtest/gtest.h>

#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/ProcessorElement/processorElementData.hpp>
#include <BabelWiresLib/Project/FeatureElements/ProcessorElement/processorElement.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testProcessor.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/tempFilePath.hpp>

TEST(ProcessorElementTest, sourceFileDataCreateElement) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ProcessorElementData data;
    data.m_factoryIdentifier = testUtils::TestProcessor::getFactoryIdentifier();
    data.m_factoryVersion = 1;

    auto featureElement = data.createFeatureElement(testEnvironment.m_projectContext, testEnvironment.m_log, 10);
    ASSERT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    ASSERT_TRUE(featureElement->as<babelwires::ProcessorElement>());
    babelwires::ProcessorElement* processorElement = static_cast<babelwires::ProcessorElement*>(featureElement.get());

    auto& inputFeature = processorElement->getInputFeature()->is<babelwires::ValueFeature>();
    ASSERT_TRUE(inputFeature.getType().as<const testUtils::TestProcessorInputOutputType>());

    const auto& outputFeature = processorElement->getOutputFeature()->is<babelwires::ValueFeature>();
    ASSERT_TRUE(outputFeature.getType().as<const testUtils::TestProcessorInputOutputType>());

    testUtils::TestProcessorInputOutputType::ConstInstance input{inputFeature};
    testUtils::TestProcessorInputOutputType::ConstInstance output{outputFeature};

    EXPECT_EQ(output.getArray().getSize(), 2);

    babelwires::ValueAssignmentData valueSettingData(babelwires::IntValue(4));
    valueSettingData.m_pathToFeature = babelwires::FeaturePath{ &*input.getRecord().getintR0() };

    processorElement->clearChanges();
    processorElement->addModifier(testEnvironment.m_log, valueSettingData);
    processorElement->process(testEnvironment.m_project, testEnvironment.m_log);

    // The default.
    EXPECT_EQ(output.getArray().getSize(), 2);
    EXPECT_FALSE(processorElement->isFailed());
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_FALSE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    // The processor sets the output array size based on this input.
    const babelwires::FeaturePath arraySettingIntPath{ &*input.getInt() };

    babelwires::ValueAssignmentData arraySettingData(babelwires::IntValue(4));
    arraySettingData.m_pathToFeature = arraySettingIntPath;

    processorElement->clearChanges();
    processorElement->addModifier(testEnvironment.m_log, arraySettingData);
    processorElement->process(testEnvironment.m_project, testEnvironment.m_log);

    EXPECT_EQ(output.getArray().getSize(), 6);
    EXPECT_FALSE(processorElement->isFailed());
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    babelwires::ValueAssignmentData badArraySettingData(babelwires::IntValue(-1));
    badArraySettingData.m_pathToFeature = arraySettingIntPath;

    processorElement->clearChanges();
    processorElement->removeModifier(processorElement->findModifier(arraySettingIntPath));
    processorElement->addModifier(testEnvironment.m_log, badArraySettingData);
    processorElement->process(testEnvironment.m_project, testEnvironment.m_log);

    EXPECT_TRUE(processorElement->isFailed());
    EXPECT_EQ(output.getArray().getSize(), 2);
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementFailed));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    processorElement->clearChanges();
    processorElement->removeModifier(processorElement->findModifier(arraySettingIntPath));
    processorElement->addModifier(testEnvironment.m_log, arraySettingData);
    processorElement->process(testEnvironment.m_project, testEnvironment.m_log);

    EXPECT_FALSE(processorElement->isFailed());
    EXPECT_EQ(output.getArray().getSize(), 6);
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementRecovered));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(processorElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
}
