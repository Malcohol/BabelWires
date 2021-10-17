#include <gtest/gtest.h>

#include "BabelWiresLib/Identifiers/registeredIdentifier.hpp"
#include "BabelWiresLib/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifier.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

#include "Tests/TestUtils/equalSets.hpp"
#include "Tests/TestUtils/testLog.hpp"

TEST(FeatureElementTest, basicAccessors) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    libTestUtils::TestFeatureElementData featureElementData;

    auto featureElement = featureElementData.createFeatureElement(context.m_projectContext, context.m_log, 54);

    ASSERT_TRUE(featureElement);
    EXPECT_FALSE(featureElement->isFailed());
    EXPECT_TRUE(featureElement->getInputFeature());
    EXPECT_TRUE(featureElement->getOutputFeature());
    EXPECT_TRUE(featureElement->getInputFeature()->as<const libTestUtils::TestRecordFeature>());
    EXPECT_TRUE(featureElement->getOutputFeature()->as<const libTestUtils::TestRecordFeature>());
    EXPECT_EQ(featureElement->getElementId(), 54);
}

TEST(FeatureElementTest, labels) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    libTestUtils::TestFeatureElementData featureElementData;

    auto featureElement = featureElementData.createFeatureElement(context.m_projectContext, context.m_log, 66);

    EXPECT_NE(featureElement->getLabel().find("66"), std::string::npos);
    // The test element uses the data's identifier as a factory name.
    EXPECT_NE(featureElement->getLabel().find(featureElementData.m_factoryIdentifier), std::string::npos);
}

TEST(FeatureElementTest, uiData) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    libTestUtils::TestFeatureElementData featureElementData;
    featureElementData.m_uiData.m_uiPosition = babelwires::UiPosition{23, -29};
    featureElementData.m_uiData.m_uiSize = babelwires::UiSize{120};

    auto featureElement = featureElementData.createFeatureElement(context.m_projectContext, context.m_log, 66);

    EXPECT_EQ(featureElement->getUiPosition(), (babelwires::UiPosition{23, -29}));
    EXPECT_EQ(featureElement->getUiSize(), babelwires::UiSize{120});

    featureElement->setUiPosition(babelwires::UiPosition{18, 100});
    EXPECT_EQ(featureElement->getUiPosition(), (babelwires::UiPosition{18, 100}));

    featureElement->setUiSize(babelwires::UiSize{199});
    EXPECT_EQ(featureElement->getUiSize(), babelwires::UiSize{199});
}

TEST(FeatureElementTest, modifiers) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    libTestUtils::TestFeatureElementData featureElementData;

    const babelwires::FeaturePath arrayPath =
        babelwires::FeaturePath::deserializeFromString(libTestUtils::TestRecordFeature::s_arrayIdInitializer);
    const babelwires::FeaturePath arrayElemPath = babelwires::FeaturePath::deserializeFromString("array/3");
    const babelwires::FeaturePath arrayElemPath2 = babelwires::FeaturePath::deserializeFromString("array/4");
    const babelwires::FeaturePath failedPath = babelwires::FeaturePath::deserializeFromString("foo/bar");

    // Deliberately have modifiers in non-canonical order.
    {
        auto arrayElemData = std::make_unique<babelwires::IntValueAssignmentData>();
        arrayElemData->m_pathToFeature = arrayElemPath;
        arrayElemData->m_value = 16;
        featureElementData.m_modifiers.emplace_back(std::move(arrayElemData));
    }
    {
        auto arrayInitData = std::make_unique<babelwires::ArraySizeModifierData>();
        arrayInitData->m_pathToFeature = arrayPath;
        arrayInitData->m_size = 5;
        featureElementData.m_modifiers.emplace_back(std::move(arrayInitData));
    }
    {
        auto failedModifier = std::make_unique<babelwires::IntValueAssignmentData>();
        failedModifier->m_pathToFeature = failedPath;
        failedModifier->m_value = 71;
        featureElementData.m_modifiers.emplace_back(std::move(failedModifier));
    }

    auto featureElement = featureElementData.createFeatureElement(context.m_projectContext, context.m_log, 66);
    ASSERT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());

    {
        const babelwires::Modifier* arrayInitData = featureElement->findModifier(arrayPath);
        ASSERT_TRUE(arrayInitData);
        EXPECT_FALSE(arrayInitData->isFailed());
        ASSERT_TRUE(arrayInitData->getModifierData().as<babelwires::ArraySizeModifierData>());
        EXPECT_EQ(static_cast<const babelwires::ArraySizeModifierData*>(&arrayInitData->getModifierData())->m_size,
                  5);
    }
    {
        const babelwires::Modifier* arrayElemData = featureElement->findModifier(arrayElemPath);
        ASSERT_TRUE(arrayElemData);
        EXPECT_FALSE(arrayElemData->isFailed());
        ASSERT_TRUE(arrayElemData->getModifierData().as<babelwires::IntValueAssignmentData>());
        EXPECT_EQ(static_cast<const babelwires::IntValueAssignmentData*>(&arrayElemData->getModifierData())->m_value,
                  16);
    }
    {
        const babelwires::Modifier* failedModifier = featureElement->findModifier(failedPath);
        ASSERT_TRUE(failedModifier);
        EXPECT_TRUE(failedModifier->isFailed());
        ASSERT_TRUE(failedModifier->getModifierData().as<babelwires::IntValueAssignmentData>());
        EXPECT_EQ(static_cast<const babelwires::IntValueAssignmentData*>(&failedModifier->getModifierData())->m_value,
                  71);
    }
    const babelwires::RecordFeature* const recordFeature = featureElement->getInputFeature();
    ASSERT_TRUE(recordFeature);
    const libTestUtils::TestRecordFeature* const testRecordFeature =
        recordFeature->as<const libTestUtils::TestRecordFeature>();
    ASSERT_TRUE(testRecordFeature);
    EXPECT_EQ(testRecordFeature->m_arrayFeature->getNumFeatures(), 5);
    ASSERT_TRUE(testRecordFeature->m_arrayFeature->tryGetChildFromStep(3)->as<const babelwires::IntFeature>());
    EXPECT_EQ(
        static_cast<const babelwires::IntFeature*>(testRecordFeature->m_arrayFeature->tryGetChildFromStep(3))->get(),
        16);
    {
        featureElement->removeModifier(featureElement->findModifier(failedPath));
        EXPECT_EQ(featureElement->findModifier(failedPath), nullptr);
    }
    {
        babelwires::IntValueAssignmentData arrayElemData2;
        arrayElemData2.m_pathToFeature = arrayElemPath2;
        arrayElemData2.m_value = 12;
        featureElement->addModifier(context.m_log, arrayElemData2);
        EXPECT_EQ(static_cast<const babelwires::IntFeature*>(testRecordFeature->m_arrayFeature->tryGetChildFromStep(4))
                      ->get(),
                  12);
    }
    {
        featureElement->removeModifier(featureElement->findModifier(arrayElemPath));
        EXPECT_EQ(featureElement->findModifier(arrayElemPath), nullptr);
        // The value should return to default.
        EXPECT_EQ(static_cast<const babelwires::IntFeature*>(testRecordFeature->m_arrayFeature->tryGetChildFromStep(3))
                      ->get(),
                  0);
    }
    {
        featureElement->removeModifier(featureElement->findModifier(arrayElemPath2));
        EXPECT_EQ(featureElement->findModifier(arrayElemPath2), nullptr);
        // The value should return to default.
        EXPECT_EQ(static_cast<const babelwires::IntFeature*>(testRecordFeature->m_arrayFeature->tryGetChildFromStep(4))
                      ->get(),
                  0);
    }
    {
        featureElement->removeModifier(featureElement->findModifier(arrayPath));
        EXPECT_EQ(featureElement->findModifier(arrayPath), nullptr);
        // The array should return to default size, which in this case is 2.
        EXPECT_EQ(testRecordFeature->m_arrayFeature->getNumFeatures(), 2);
    }
}

TEST(FeatureElementTest, expandedPaths) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    libTestUtils::TestFeatureElementData featureElementData;

    const babelwires::FeaturePath arrayPath =
        babelwires::FeaturePath::deserializeFromString(libTestUtils::TestRecordFeature::s_arrayIdInitializer);
    const babelwires::FeaturePath arrayElemPath = babelwires::FeaturePath::deserializeFromString(
        std::string(libTestUtils::TestRecordFeature::s_arrayIdInitializer) + "/3");
    const babelwires::FeaturePath arrayElemPath2 = babelwires::FeaturePath::deserializeFromString(
        std::string(libTestUtils::TestRecordFeature::s_arrayIdInitializer) + "/4");
    const babelwires::FeaturePath fooBar = babelwires::FeaturePath::deserializeFromString("foo/bar");

    featureElementData.m_expandedPaths.emplace_back(arrayElemPath);
    featureElementData.m_expandedPaths.emplace_back(arrayPath);
    featureElementData.m_expandedPaths.emplace_back(fooBar);

    auto featureElement = featureElementData.createFeatureElement(context.m_projectContext, context.m_log, 66);
    ASSERT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());

    EXPECT_TRUE(featureElement->isExpanded(arrayPath));
    EXPECT_TRUE(featureElement->isExpanded(arrayElemPath));
    EXPECT_TRUE(featureElement->isExpanded(fooBar));
    EXPECT_FALSE(featureElement->isExpanded(arrayElemPath2));

    featureElement->setExpanded(arrayPath, false);
    EXPECT_FALSE(featureElement->isExpanded(arrayPath));
    EXPECT_TRUE(featureElement->isExpanded(arrayElemPath));
    EXPECT_TRUE(featureElement->isExpanded(fooBar));
    EXPECT_FALSE(featureElement->isExpanded(arrayElemPath2));

    featureElement->setExpanded(arrayElemPath, false);
    EXPECT_FALSE(featureElement->isExpanded(arrayPath));
    EXPECT_FALSE(featureElement->isExpanded(arrayElemPath));
    EXPECT_TRUE(featureElement->isExpanded(fooBar));
    EXPECT_FALSE(featureElement->isExpanded(arrayElemPath2));

    featureElement->setExpanded(arrayElemPath2, true);
    EXPECT_FALSE(featureElement->isExpanded(arrayPath));
    EXPECT_FALSE(featureElement->isExpanded(arrayElemPath));
    EXPECT_TRUE(featureElement->isExpanded(fooBar));
    EXPECT_TRUE(featureElement->isExpanded(arrayElemPath2));

    featureElement->setExpanded(fooBar, false);
    EXPECT_FALSE(featureElement->isExpanded(arrayPath));
    EXPECT_FALSE(featureElement->isExpanded(arrayElemPath));
    EXPECT_FALSE(featureElement->isExpanded(fooBar));
    EXPECT_TRUE(featureElement->isExpanded(arrayElemPath2));

    featureElement->setExpanded(arrayElemPath2, false);
    EXPECT_FALSE(featureElement->isExpanded(arrayPath));
    EXPECT_FALSE(featureElement->isExpanded(arrayElemPath));
    EXPECT_FALSE(featureElement->isExpanded(fooBar));
    EXPECT_FALSE(featureElement->isExpanded(arrayElemPath2));
}

TEST(FeatureElementTest, extractElementData) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    libTestUtils::TestFeatureElementData featureElementData;

    const babelwires::FeaturePath arrayPath =
        babelwires::FeaturePath::deserializeFromString(libTestUtils::TestRecordFeature::s_arrayIdInitializer);
    const babelwires::FeaturePath arrayElemPath = babelwires::FeaturePath::deserializeFromString(
        std::string(libTestUtils::TestRecordFeature::s_arrayIdInitializer) + "/3");
    const babelwires::FeaturePath arrayElemPath2 = babelwires::FeaturePath::deserializeFromString(
        std::string(libTestUtils::TestRecordFeature::s_arrayIdInitializer) + "/4");
    const babelwires::FeaturePath failedPath = babelwires::FeaturePath::deserializeFromString("foo/bar");

    // Deliberately have modifiers in non-canonical order.
    {
        auto arrayElemData = std::make_unique<babelwires::IntValueAssignmentData>();
        arrayElemData->m_pathToFeature = arrayElemPath;
        arrayElemData->m_value = 16;
        featureElementData.m_modifiers.emplace_back(std::move(arrayElemData));
    }
    {
        auto arrayInitData = std::make_unique<babelwires::ArraySizeModifierData>();
        arrayInitData->m_pathToFeature = arrayPath;
        arrayInitData->m_size = 5;
        featureElementData.m_modifiers.emplace_back(std::move(arrayInitData));
    }
    {
        auto failedModifier = std::make_unique<babelwires::IntValueAssignmentData>();
        failedModifier->m_pathToFeature = failedPath;
        failedModifier->m_value = 71;
        featureElementData.m_modifiers.emplace_back(std::move(failedModifier));
    }
    featureElementData.m_expandedPaths.emplace_back(arrayElemPath2);
    featureElementData.m_expandedPaths.emplace_back(arrayPath);
    featureElementData.m_expandedPaths.emplace_back(failedPath);
    featureElementData.m_uiData.m_uiPosition = babelwires::UiPosition{23, -29};
    featureElementData.m_uiData.m_uiSize = babelwires::UiSize{120};

    auto featureElement = featureElementData.createFeatureElement(context.m_projectContext, context.m_log, 66);
    ASSERT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());

    {
        babelwires::IntValueAssignmentData arrayElemData2;
        arrayElemData2.m_pathToFeature = arrayElemPath2;
        arrayElemData2.m_value = 12;
        featureElement->addModifier(context.m_log, arrayElemData2);
    }
    featureElement->removeModifier(featureElement->findModifier(arrayElemPath));
    featureElement->setExpanded(arrayElemPath, true);
    featureElement->setExpanded(arrayElemPath2, false);

    const auto extractedData = featureElement->extractElementData();

    ASSERT_TRUE(extractedData);
    EXPECT_EQ(extractedData->m_id, 66);

    ASSERT_EQ(extractedData->m_modifiers.size(), 3);
    // We assume extracted data is sorted, though that assumption is not part of the guarantee.
    EXPECT_EQ(extractedData->m_modifiers[0]->m_pathToFeature, arrayPath);
    EXPECT_TRUE(extractedData->m_modifiers[0].get()->as<babelwires::ArraySizeModifierData>());
    EXPECT_EQ(static_cast<const babelwires::ArraySizeModifierData*>(extractedData->m_modifiers[0].get())->m_size, 5);
    EXPECT_EQ(extractedData->m_modifiers[1]->m_pathToFeature, arrayElemPath2);
    EXPECT_TRUE(extractedData->m_modifiers[1]->as<babelwires::IntValueAssignmentData>());
    EXPECT_EQ(static_cast<const babelwires::IntValueAssignmentData*>(extractedData->m_modifiers[1].get())->m_value, 12);
    // Even though this modifier is currently failed, its data is still important.
    EXPECT_EQ(extractedData->m_modifiers[2]->m_pathToFeature, failedPath);
    EXPECT_TRUE(extractedData->m_modifiers[2]->as<babelwires::IntValueAssignmentData>());
    EXPECT_EQ(static_cast<const babelwires::IntValueAssignmentData*>(extractedData->m_modifiers[2].get())->m_value, 71);

    // The failed path is not included.
    ASSERT_EQ(extractedData->m_expandedPaths.size(), 2);
    EXPECT_EQ(extractedData->m_expandedPaths[0], arrayPath);
    EXPECT_EQ(extractedData->m_expandedPaths[1], arrayElemPath);

    EXPECT_EQ(extractedData->m_uiData.m_uiPosition, (babelwires::UiPosition{23, -29}));
    EXPECT_EQ(extractedData->m_uiData.m_uiSize, babelwires::UiSize{120});
}

TEST(FeatureElementTest, removedModifiers) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    libTestUtils::TestFeatureElementData featureElementData;

    const babelwires::FeaturePath arrayPath =
        babelwires::FeaturePath::deserializeFromString(libTestUtils::TestRecordFeature::s_arrayIdInitializer);
    const babelwires::FeaturePath arrayElemPath = babelwires::FeaturePath::deserializeFromString(
        std::string(libTestUtils::TestRecordFeature::s_arrayIdInitializer) + "/3");
    const babelwires::FeaturePath failedPath = babelwires::FeaturePath::deserializeFromString("foo/bar");

    // Deliberately have modifiers in non-canonical order.
    {
        auto arrayElemData = std::make_unique<babelwires::IntValueAssignmentData>();
        arrayElemData->m_pathToFeature = arrayElemPath;
        arrayElemData->m_value = 16;
        featureElementData.m_modifiers.emplace_back(std::move(arrayElemData));
    }
    {
        auto arrayInitData = std::make_unique<babelwires::ArraySizeModifierData>();
        arrayInitData->m_pathToFeature = arrayPath;
        arrayInitData->m_size = 5;
        featureElementData.m_modifiers.emplace_back(std::move(arrayInitData));
    }
    {
        auto failedModifier = std::make_unique<babelwires::IntValueAssignmentData>();
        failedModifier->m_pathToFeature = failedPath;
        failedModifier->m_value = 71;
        featureElementData.m_modifiers.emplace_back(std::move(failedModifier));
    }

    auto featureElement = featureElementData.createFeatureElement(context.m_projectContext, context.m_log, 66);
    ASSERT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());

    featureElement->clearChanges();
    featureElement->removeModifier(featureElement->findModifier(failedPath));
    featureElement->removeModifier(featureElement->findModifier(arrayElemPath));

    int numMods = 0;
    int numCorrectMods = 0;
    for (const auto modifier : featureElement->getRemovedModifiers()) {
        ++numMods;
        if (modifier->getModifierData().m_pathToFeature == arrayElemPath) {
            ASSERT_TRUE(modifier->getModifierData().as<babelwires::IntValueAssignmentData>());
            EXPECT_EQ(static_cast<const babelwires::IntValueAssignmentData*>(&modifier->getModifierData())->m_value,
                      16);
            ++numCorrectMods;
        }
        if (modifier->getModifierData().m_pathToFeature == failedPath) {
            ASSERT_TRUE(modifier->getModifierData().as<babelwires::IntValueAssignmentData>());
            EXPECT_EQ(static_cast<const babelwires::IntValueAssignmentData*>(&modifier->getModifierData())->m_value,
                      71);
            ++numCorrectMods;
        }
    }
    EXPECT_EQ(numMods, 2);
    EXPECT_EQ(numCorrectMods, 2);
}

TEST(FeatureElementTest, failure) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    libTestUtils::TestFeatureElementData featureElementData;

    auto featureElement = featureElementData.createFeatureElement(context.m_projectContext, context.m_log, 66);
    ASSERT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    ASSERT_TRUE(featureElement->as<libTestUtils::TestFeatureElement>());
    libTestUtils::TestFeatureElement* testFeatureElement =
        static_cast<libTestUtils::TestFeatureElement*>(featureElement.get());

    testFeatureElement->simulateFailure();
    EXPECT_TRUE(featureElement->isFailed());
    EXPECT_FALSE(featureElement->getReasonForFailure().empty());

    testFeatureElement->simulateRecovery();
    EXPECT_FALSE(featureElement->isFailed());
    EXPECT_TRUE(featureElement->getReasonForFailure().empty());
}

TEST(FeatureElementTest, simpleChanges) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    libTestUtils::TestFeatureElementData featureElementData;

    auto featureElement = featureElementData.createFeatureElement(context.m_projectContext, context.m_log, 66);
    EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementIsNew));
    EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementChangesMask));
    featureElement->clearChanges();
    EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    const babelwires::FeaturePath arrayPath =
        babelwires::FeaturePath::deserializeFromString(libTestUtils::TestRecordFeature::s_arrayIdInitializer);
    const babelwires::FeaturePath arrayElemPath = babelwires::FeaturePath::deserializeFromString(
        std::string(libTestUtils::TestRecordFeature::s_arrayIdInitializer) + "/3");

    {
        featureElement->clearChanges();
        featureElement->setUiPosition(babelwires::UiPosition{18, 100});
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::UiPositionChanged));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
        EXPECT_FALSE(featureElement->isChanged(~babelwires::FeatureElement::Changes::UiPositionChanged));
    }
    {
        featureElement->clearChanges();
        featureElement->setUiSize(babelwires::UiSize{199});
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::UiSizeChanged));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
        EXPECT_FALSE(featureElement->isChanged(~babelwires::FeatureElement::Changes::UiSizeChanged));
    }
    {
        featureElement->clearChanges();
        babelwires::ArraySizeModifierData arrayInitData;
        arrayInitData.m_pathToFeature = arrayPath;
        arrayInitData.m_size = 5;
        featureElement->addModifier(context.m_log, arrayInitData);
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierAdded));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
        EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    }
    {
        featureElement->clearChanges();
        babelwires::IntValueAssignmentData arrayElemData;
        arrayElemData.m_pathToFeature = arrayElemPath;
        arrayElemData.m_value = 12;
        featureElement->addModifier(context.m_log, arrayElemData);
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierAdded));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
        EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    }
    {
        featureElement->clearChanges();
        featureElement->removeModifier(featureElement->findModifier(arrayElemPath));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierRemoved));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
        EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
    }
    {
        featureElement->clearChanges();
        featureElement->removeModifier(featureElement->findModifier(arrayPath));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierRemoved));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::ModifierChangesMask));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
        EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureValueChanged));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
    }
    {
        featureElement->clearChanges();
        featureElement->setExpanded(arrayPath, true);
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::CompoundExpandedOrCollapsed));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
    }
    {
        featureElement->clearChanges();
        featureElement->setExpanded(arrayPath, false);
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::CompoundExpandedOrCollapsed));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
    }
    {
        featureElement->clearChanges();
        ASSERT_TRUE(featureElement->as<libTestUtils::TestFeatureElement>());
        static_cast<libTestUtils::TestFeatureElement*>(featureElement.get())->simulateFailure();
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementFailed));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
    }
    {
        featureElement->clearChanges();
        ASSERT_TRUE(featureElement->as<libTestUtils::TestFeatureElement>());
        static_cast<libTestUtils::TestFeatureElement*>(featureElement.get())->simulateRecovery();
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementRecovered));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureStructureChanged));
        EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
    }
}

TEST(FeatureElementTest, isInDependencyLoop) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    libTestUtils::TestFeatureElementData featureElementData;

    auto featureElement = featureElementData.createFeatureElement(context.m_projectContext, context.m_log, 66);
    ASSERT_TRUE(featureElement);
    ASSERT_FALSE(featureElement->isFailed());
    ASSERT_TRUE(featureElement->as<libTestUtils::TestFeatureElement>());

    featureElement->clearChanges();
    featureElement->setInDependencyLoop(true);

    EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementFailed));
    EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    featureElement->clearChanges();
    featureElement->setInDependencyLoop(false);

    EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementRecovered));
    EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    static_cast<libTestUtils::TestFeatureElement*>(featureElement.get())->simulateFailure();
    featureElement->clearChanges();
    featureElement->setInDependencyLoop(true);

    EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementFailed));
    EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementRecovered));
    EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    // Don't think we need to note any change.
    // EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    featureElement->clearChanges();
    featureElement->setInDependencyLoop(false);

    EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementFailed));
    EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementRecovered));
    EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    // Don't think we need to note any change.
    // EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    static_cast<libTestUtils::TestFeatureElement*>(featureElement.get())->simulateRecovery();
    featureElement->setInDependencyLoop(true);
    featureElement->clearChanges();
    static_cast<libTestUtils::TestFeatureElement*>(featureElement.get())->simulateFailure();

    EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementFailed));
    EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementRecovered));
    EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));

    featureElement->clearChanges();
    static_cast<libTestUtils::TestFeatureElement*>(featureElement.get())->simulateRecovery();

    EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementFailed));
    EXPECT_FALSE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureElementRecovered));
    EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::FeatureChangesMask));
    EXPECT_TRUE(featureElement->isChanged(babelwires::FeatureElement::Changes::SomethingChanged));
}
