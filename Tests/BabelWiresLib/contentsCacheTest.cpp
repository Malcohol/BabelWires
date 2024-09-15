#include <gtest/gtest.h>

#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/contentsCache.hpp>
#include <BabelWiresLib/Project/FeatureElements/editTree.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Identifiers/registeredIdentifier.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testModifier.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

namespace {
    std::unique_ptr<testUtils::LocalTestModifier> createIntModifier(babelwires::FeaturePath path,
                                                                    babelwires::FeatureElement* owner = nullptr) {
        auto data = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(8));
        data->m_pathToFeature = std::move(path);
        auto modPtr = std::make_unique<testUtils::LocalTestModifier>(std::move(data));
        modPtr->setOwner(owner);
        return modPtr;
    }

    std::unique_ptr<testUtils::LocalTestModifier> createStringModifier(babelwires::FeaturePath path,
                                                                       babelwires::FeatureElement* owner = nullptr) {
        auto data = std::make_unique<babelwires::ValueAssignmentData>(babelwires::StringValue("Hello"));
        data->m_pathToFeature = std::move(path);
        auto modPtr = std::make_unique<testUtils::LocalTestModifier>(std::move(data));
        modPtr->setOwner(owner);
        return modPtr;
    }

    void checkFirstIntEntry(const babelwires::ContentsCacheEntry* entry,
                            testUtils::TestComplexRecordTypeFeatureInfo* inputInfo,
                            testUtils::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), testUtils::TestComplexRecordType::s_intFieldName);
        if (inputInfo) {
            EXPECT_EQ(entry->getInputFeature(), &inputInfo->m_intFeature);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToInt);
        } else {
            EXPECT_FALSE(entry->getInputFeature());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutputFeature(), &outputInfo->m_intFeature);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToInt);
        } else {
            EXPECT_FALSE(entry->getOutputFeature());
        }
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkSubRecordEntry(const babelwires::ContentsCacheEntry* entry,
                             testUtils::TestComplexRecordTypeFeatureInfo* inputInfo,
                             testUtils::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), testUtils::TestComplexRecordType::s_subRecordFieldName);
        if (inputInfo) {
            EXPECT_EQ(entry->getInputFeature(), &inputInfo->m_subRecordFeature);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToSubRecord);
        } else {
            EXPECT_FALSE(entry->getInputFeature());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutputFeature(), &outputInfo->m_subRecordFeature);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToSubRecord);
        } else {
            EXPECT_FALSE(entry->getOutputFeature());
        }
        EXPECT_TRUE(entry->isExpandable());
    }

    void checkSubRecordIntEntry(const babelwires::ContentsCacheEntry* entry,
                                testUtils::TestComplexRecordTypeFeatureInfo* inputInfo,
                                testUtils::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), testUtils::TestSimpleRecordType::s_int0FieldName);
        if (inputInfo) {
            EXPECT_EQ(entry->getInputFeature(), &inputInfo->m_subRecordIntFeature);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToSubRecordInt);
        } else {
            EXPECT_FALSE(entry->getInputFeature());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutputFeature(), &outputInfo->m_subRecordIntFeature);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToSubRecordInt);
        } else {
            EXPECT_FALSE(entry->getOutputFeature());
        }
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkArrayEntry(const babelwires::ContentsCacheEntry* entry,
                         testUtils::TestComplexRecordTypeFeatureInfo* inputInfo,
                         testUtils::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), testUtils::TestComplexRecordType::s_arrayFieldName);
        if (inputInfo) {
            EXPECT_EQ(entry->getInputFeature(), &inputInfo->m_arrayFeature);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToArray);
        } else {
            EXPECT_FALSE(entry->getInputFeature());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutputFeature(), &outputInfo->m_arrayFeature);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToArray);
        } else {
            EXPECT_FALSE(entry->getOutputFeature());
        }
        EXPECT_TRUE(entry->isExpandable());
    }

    void checkFirstArrayEntry(const babelwires::ContentsCacheEntry* entry,
                              testUtils::TestComplexRecordTypeFeatureInfo* inputInfo,
                              testUtils::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), "[0]");
        if (inputInfo) {
            EXPECT_EQ(entry->getInputFeature(), &inputInfo->m_elem0);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToElem0);
        } else {
            EXPECT_FALSE(entry->getInputFeature());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutputFeature(), &outputInfo->m_elem0);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToElem0);
        } else {
            EXPECT_FALSE(entry->getOutputFeature());
        }
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkSecondArrayEntry(const babelwires::ContentsCacheEntry* entry,
                               testUtils::TestComplexRecordTypeFeatureInfo* inputInfo,
                               testUtils::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), "[1]");
        if (inputInfo) {
            EXPECT_EQ(entry->getInputFeature(), &inputInfo->m_elem1);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToElem1);
        } else {
            EXPECT_FALSE(entry->getInputFeature());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutputFeature(), &outputInfo->m_elem1);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToElem1);
        } else {
            EXPECT_FALSE(entry->getOutputFeature());
        }
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkUnmodified(const babelwires::ContentsCacheEntry* entry) {
        EXPECT_FALSE(entry->hasModifier());
        EXPECT_FALSE(entry->hasSubmodifiers());
        EXPECT_FALSE(entry->hasFailedModifier());
        EXPECT_FALSE(entry->hasHiddenModifier());
        EXPECT_FALSE(entry->hasFailedHiddenModifiers());
    }

    void testCommonBehaviour(babelwires::ContentsCache& cache, babelwires::EditTree& editTree,
                             babelwires::ValueFeature* inputFeature, babelwires::ValueFeature* outputFeature) {
        cache.setFeatures(inputFeature, outputFeature);
        ASSERT_EQ(cache.getNumRows(), 5);
        auto inputInfo =
            inputFeature ? std::make_unique<testUtils::TestComplexRecordTypeFeatureInfo>(*inputFeature) : nullptr;
        auto outputInfo =
            outputFeature ? std::make_unique<testUtils::TestComplexRecordTypeFeatureInfo>(*outputFeature) : nullptr;
        auto info = inputInfo ? inputInfo.get() : outputInfo.get();
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkFirstIntEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkSubRecordEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(4);
            checkArrayEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        // Expand the record
        editTree.setExpanded(info->m_pathToSubRecord, true);
        cache.setFeatures(inputFeature, outputFeature);
        ASSERT_EQ(cache.getNumRows(), 7);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkFirstIntEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkSubRecordEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
            checkSubRecordIntEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(6);
            checkArrayEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        // Expand the array
        editTree.setExpanded(info->m_pathToArray, true);
        cache.setFeatures(inputFeature, outputFeature);
        ASSERT_EQ(cache.getNumRows(), 11);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkFirstIntEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkSubRecordEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
            checkSubRecordIntEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(6);
            checkArrayEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(7);
            checkFirstArrayEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(8);
            checkSecondArrayEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
    }

    void testModifierBehaviour(babelwires::ProjectContext& context, babelwires::ContentsCache& cache,
                               babelwires::EditTree& editTree, babelwires::ValueFeature* inputFeature,
                               babelwires::ValueFeature* outputFeature) {
        ASSERT_TRUE(inputFeature);

        auto inputInfo =
            inputFeature ? std::make_unique<testUtils::TestComplexRecordTypeFeatureInfo>(*inputFeature) : nullptr;
        auto outputInfo =
            outputFeature ? std::make_unique<testUtils::TestComplexRecordTypeFeatureInfo>(*outputFeature) : nullptr;
        auto info = inputInfo ? inputInfo.get() : outputInfo.get();

        // Add a modifier to the first int
        editTree.addModifier(createIntModifier(info->m_pathToInt));
        cache.updateModifierCache();

        ASSERT_EQ(cache.getNumRows(), 11);
        for (int i = 1; i < 11; ++i) {
            checkUnmodified(cache.getEntry(i));
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkFirstIntEntry(entry, inputInfo.get(), outputInfo.get());
            EXPECT_TRUE(entry->hasModifier());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
            EXPECT_TRUE(entry->hasSubmodifiers());
        }
        // Add a submodifier to the array
        editTree.addModifier(createIntModifier(info->m_pathToElem1));
        cache.updateModifierCache();
        ASSERT_EQ(cache.getNumRows(), 11);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(6);
            checkArrayEntry(entry, inputInfo.get(), outputInfo.get());
            EXPECT_FALSE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }
        checkUnmodified(cache.getEntry(7));
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(8);
            checkSecondArrayEntry(entry, inputInfo.get(), outputInfo.get());
            EXPECT_TRUE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }
        checkUnmodified(cache.getEntry(9));
        // Add a failing modifier to the array
        {
            testUtils::TestFeatureElement owner(context);
            auto modifier = createStringModifier(info->m_pathToElem0, &owner);
            modifier->simulateFailure();
            editTree.addModifier(std::move(modifier));
        }
        cache.updateModifierCache();
        ASSERT_EQ(cache.getNumRows(), 11);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(6);
            checkArrayEntry(entry, inputInfo.get(), outputInfo.get());
            EXPECT_FALSE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(7);
            checkFirstArrayEntry(entry, inputInfo.get(), outputInfo.get());
            EXPECT_TRUE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_TRUE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }

        // Collapse the array
        editTree.setExpanded(info->m_pathToArray, false);
        cache.setFeatures(inputFeature, outputFeature);
        ASSERT_EQ(cache.getNumRows(), 7);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(6);
            checkArrayEntry(entry, inputInfo.get(), outputInfo.get());
            EXPECT_FALSE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_TRUE(entry->hasHiddenModifier());
            EXPECT_TRUE(entry->hasFailedHiddenModifiers());
        }
        // Remove the failing modifier
        editTree.removeModifier(editTree.findModifier(info->m_pathToElem0));
        cache.updateModifierCache();
        ASSERT_EQ(cache.getNumRows(), 7);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(6);
            checkArrayEntry(entry, inputInfo.get(), outputInfo.get());
            EXPECT_FALSE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_TRUE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }
        editTree.removeModifier(editTree.findModifier(info->m_pathToElem1));
        cache.updateModifierCache();
        ASSERT_EQ(cache.getNumRows(), 7);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(6);
            checkArrayEntry(entry, inputInfo.get(), outputInfo.get());
            EXPECT_FALSE(entry->hasModifier());
            EXPECT_FALSE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }
    }
} // namespace

TEST(ContentsCacheTest, inputFeatureOnly) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    babelwires::SimpleValueFeature inputFeature(testEnvironment.m_typeSystem,
                                                testUtils::TestComplexRecordType::getThisIdentifier());
    inputFeature.setToDefault();

    testCommonBehaviour(cache, editTree, &inputFeature, nullptr);
    testModifierBehaviour(testEnvironment.m_projectContext, cache, editTree, &inputFeature, nullptr);
}

TEST(ContentsCacheTest, outputFeatureOnly) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    babelwires::SimpleValueFeature outputFeature(testEnvironment.m_typeSystem,
                                                 testUtils::TestComplexRecordType::getThisIdentifier());
    outputFeature.setToDefault();

    testCommonBehaviour(cache, editTree, nullptr, &outputFeature);
}

TEST(ContentsCacheTest, inputAndOutputFeature) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    babelwires::SimpleValueFeature inputFeature(testEnvironment.m_typeSystem,
                                                testUtils::TestComplexRecordType::getThisIdentifier());
    babelwires::SimpleValueFeature outputFeature(testEnvironment.m_typeSystem,
                                                 testUtils::TestComplexRecordType::getThisIdentifier());
    inputFeature.setToDefault();
    outputFeature.setToDefault();

    testCommonBehaviour(cache, editTree, &inputFeature, &outputFeature);
    testModifierBehaviour(testEnvironment.m_projectContext, cache, editTree, &inputFeature, &outputFeature);
}

/*

TEST(ContentsCacheTest, inputAndOutputDifferentFeatures) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    testUtils::TestRootFeature inputFeature(testEnvironment.m_projectContext, 255, true);
    testUtils::TestRootFeature outputFeature(testEnvironment.m_projectContext);
    inputFeature.setToDefault();
    outputFeature.setToDefault();

    outputFeature.m_arrayFeature->setSize(3);

    cache.setFeatures(&inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 3);

    editTree.setExpanded(testUtils::TestRootFeature::s_pathToRecord, true);
    cache.setFeatures(&inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 5);

    editTree.setExpanded(testUtils::TestRootFeature::s_pathToArray, true);
    cache.setFeatures(&inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 8);

    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
        checkFirstIntEntry(entry, &inputFeature, &outputFeature);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
        checkArrayEntry(entry, &inputFeature, &outputFeature);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
        checkFirstArrayEntry(entry, &inputFeature, &outputFeature);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(3);
        checkSecondArrayEntry(entry, &inputFeature, &outputFeature);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(4);
        EXPECT_EQ(entry->getLabel(), "[2]");
        EXPECT_FALSE(entry->getInputFeature());
        EXPECT_EQ(entry->getOutputFeature(), outputFeature.m_arrayFeature->getFeature(2));
        EXPECT_EQ(entry->getPath(), testUtils::TestRootFeature::s_pathToArray_2);
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(5);
        checkSubRecordEntry(entry, &inputFeature, &outputFeature);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(6);
        checkSubRecordIntEntry(entry, &inputFeature, &outputFeature);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(7);
        EXPECT_EQ(entry->getLabel(), testUtils::TestRootFeature::s_extraIntFieldName);
        EXPECT_FALSE(entry->getOutputFeature());
        EXPECT_EQ(entry->getInputFeature(), inputFeature.m_extraIntFeature);
        EXPECT_EQ(entry->getPath(), testUtils::TestRootFeature::s_pathToExtraInt);
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
        checkUnmodified(entry);
    }
}

TEST(ContentsCacheTest, hiddenTopLevelModifiers) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    testUtils::TestRootFeature inputFeature(testEnvironment.m_projectContext);
    testUtils::TestRootFeature outputFeature(testEnvironment.m_projectContext);
    inputFeature.setToDefault();
    outputFeature.setToDefault();

    cache.setFeatures(&inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 3);

    // Adding a hidden failed modifier whose parent is logically the root requires us to present
    // a root entry to the user, so the UI functionality for failed modifiers has somewhere to live.
    testUtils::TestFeatureElement owner(testEnvironment.m_projectContext);
    auto modifierPtr = createIntModifier(babelwires::FeaturePath::deserializeFromString("flarg"), &owner);
    modifierPtr->simulateFailure();
    auto modifier = modifierPtr.get();
    editTree.addModifier(std::move(modifierPtr));

    cache.clearChanges();
    cache.updateModifierCache();

    EXPECT_TRUE(cache.isChanged(babelwires::ContentsCache::Changes::StructureChanged));
    EXPECT_EQ(cache.getNumRows(), 4);

    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
        EXPECT_EQ(entry->getLabel(), "Root");
        EXPECT_EQ(entry->getInputFeature(), &inputFeature);
        EXPECT_EQ(entry->getOutputFeature(), &outputFeature);
        EXPECT_EQ(entry->getPath(), babelwires::FeaturePath());
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
        EXPECT_FALSE(entry->hasModifier());
        EXPECT_TRUE(entry->hasSubmodifiers());
        EXPECT_FALSE(entry->hasFailedModifier());
        EXPECT_TRUE(entry->hasHiddenModifier());
        EXPECT_TRUE(entry->hasFailedHiddenModifiers());
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
        checkFirstIntEntry(entry, &inputFeature, &outputFeature);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
        checkArrayEntry(entry, &inputFeature, &outputFeature);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(3);
        checkSubRecordEntry(entry, &inputFeature, &outputFeature);
        checkUnmodified(entry);
    }

    editTree.removeModifier(modifier);
    cache.clearChanges();
    cache.updateModifierCache();

    EXPECT_TRUE(cache.isChanged(babelwires::ContentsCache::Changes::StructureChanged));
    EXPECT_EQ(cache.getNumRows(), 3);
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
        checkFirstIntEntry(entry, &inputFeature, &outputFeature);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
        checkArrayEntry(entry, &inputFeature, &outputFeature);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
        checkSubRecordEntry(entry, &inputFeature, &outputFeature);
        checkUnmodified(entry);
    }
}

namespace {
    void checkFileRootEntry(const babelwires::ContentsCacheEntry* entry, testUtils::TestFileFeature* inputFeature,
                            testUtils::TestFileFeature* outputFeature) {
        EXPECT_EQ(entry->getLabel(), "File");
        if (inputFeature) {
            EXPECT_EQ(entry->getInputFeature(), inputFeature);
        } else {
            EXPECT_FALSE(entry->getInputFeature());
        }
        if (outputFeature) {
            EXPECT_EQ(entry->getOutputFeature(), outputFeature);
        } else {
            EXPECT_FALSE(entry->getOutputFeature());
        }
        EXPECT_EQ(entry->getPath(), babelwires::FeaturePath());
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkFileIntEntry(const babelwires::ContentsCacheEntry* entry, testUtils::TestFileFeature* inputFeature,
                           testUtils::TestFileFeature* outputFeature) {
        EXPECT_EQ(entry->getLabel(), testUtils::TestFileFeature::s_intChildFieldName);
        if (inputFeature) {
            EXPECT_EQ(entry->getInputFeature(), inputFeature->m_intChildFeature);
        } else {
            EXPECT_FALSE(entry->getInputFeature());
        }
        if (outputFeature) {
            EXPECT_EQ(entry->getOutputFeature(), outputFeature->m_intChildFeature);
        } else {
            EXPECT_FALSE(entry->getOutputFeature());
        }
        EXPECT_EQ(entry->getPath(), testUtils::TestFileFeature::s_pathToIntChild);
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void testFileCommonBehaviour(babelwires::ContentsCache& cache, babelwires::EditTree& editTree,
                                 testUtils::TestFileFeature* inputFeature, testUtils::TestFileFeature* outputFeature) {
        ASSERT_EQ(cache.getNumRows(), 2);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkFileRootEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkFileIntEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
    }

    void testModifierBehaviour(babelwires::ProjectContext& context, babelwires::ContentsCache& cache,
                               babelwires::EditTree& editTree, testUtils::TestFileFeature* inputFeature,
                               testUtils::TestFileFeature* outputFeature) {
        ASSERT_TRUE(inputFeature);

        // Adding a hidden failed modifier whose parent is logically the root.
        // In this case, there's already an entry for the root, so no new
        // artificial entry need be added.
        testUtils::TestFeatureElement owner(context);
        auto modifierPtr = createIntModifier(babelwires::FeaturePath::deserializeFromString("flarg"), &owner);
        modifierPtr->simulateFailure();
        auto modifier = modifierPtr.get();
        editTree.addModifier(std::move(modifierPtr));

        cache.clearChanges();
        cache.updateModifierCache();

        EXPECT_FALSE(cache.isChanged(babelwires::ContentsCache::Changes::StructureChanged));
        ASSERT_EQ(cache.getNumRows(), 2);

        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkFileRootEntry(entry, inputFeature, outputFeature);
            EXPECT_FALSE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_TRUE(entry->hasHiddenModifier());
            EXPECT_TRUE(entry->hasFailedHiddenModifiers());
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkFileIntEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
    }
} // namespace

TEST(ContentsCacheTest, inputFileFeatureOnly) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    testUtils::TestFileFeature inputFeature(testEnvironment.m_projectContext);
    inputFeature.setToDefault();

    cache.setFeatures(&inputFeature, nullptr);
    testFileCommonBehaviour(cache, editTree, &inputFeature, nullptr);
    testModifierBehaviour(testEnvironment.m_projectContext, cache, editTree, &inputFeature, nullptr);
}

TEST(ContentsCacheTest, outputFileFeatureOnly) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    testUtils::TestFileFeature outputFeature(testEnvironment.m_projectContext);
    outputFeature.setToDefault();

    cache.setFeatures(nullptr, &outputFeature);
    testFileCommonBehaviour(cache, editTree, nullptr, &outputFeature);
}

TEST(ContentsCacheTest, inputAndOutputFileFeature) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    testUtils::TestFileFeature inputFeature(testEnvironment.m_projectContext);
    testUtils::TestFileFeature outputFeature(testEnvironment.m_projectContext);
    inputFeature.setToDefault();
    outputFeature.setToDefault();

    cache.setFeatures(&inputFeature, &outputFeature);
    testFileCommonBehaviour(cache, editTree, &inputFeature, &outputFeature);
    testModifierBehaviour(testEnvironment.m_projectContext, cache, editTree, &inputFeature, &outputFeature);
}

namespace {
    /// Style is normally set on a class with a virtual method. For testing convenience, we will use a class which
    /// returns a style from a member.
    struct TestRecordWithStyle : babelwires::RecordFeature {
        TestRecordWithStyle(Style style)
            : m_style(style) {
                addField(std::make_unique<babelwires::IntFeature>(), testUtils::getTestRegisteredIdentifier("Flerm"));
            }

        Style getStyle() const { return m_style; }

        Style m_style;
    };

    struct TestRecordWithChildStyles : babelwires::RootFeature {
        TestRecordWithChildStyles(const babelwires::ProjectContext& context) : babelwires::RootFeature(context) {
            addField(std::make_unique<TestRecordWithStyle>(Style(0)), testUtils::getTestRegisteredIdentifier("zero"));
            addField(std::make_unique<TestRecordWithStyle>(Style::isCollapsable),
testUtils::getTestRegisteredIdentifier("coll")); addField(std::make_unique<TestRecordWithStyle>(Style::isInlined),
testUtils::getTestRegisteredIdentifier("inline")); addField(std::make_unique<TestRecordWithStyle>(Style::isCollapsable |
Style::isInlined), testUtils::getTestRegisteredIdentifier("both"));
        }
    };
} // namespace

TEST(ContentsCacheTest, style) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    TestRecordWithChildStyles record(testEnvironment.m_projectContext);
    // Handling of style should be unaffected by input / output features.
    cache.setFeatures(&record, nullptr);

    EXPECT_EQ(cache.getNumRows(), 6);
    EXPECT_EQ(cache.getEntry(0)->getIndent(), 0);
    EXPECT_EQ(cache.getEntry(1)->getIndent(), 1);
    EXPECT_EQ(cache.getEntry(2)->getIndent(), 0);
    EXPECT_EQ(cache.getEntry(3)->getIndent(), 0);
    EXPECT_EQ(cache.getEntry(4)->getIndent(), 0);
    EXPECT_EQ(cache.getEntry(5)->getIndent(), 0);

    EXPECT_EQ(cache.getEntry(0)->isExpandable(), false);
    EXPECT_EQ(cache.getEntry(1)->isExpandable(), false);
    EXPECT_EQ(cache.getEntry(2)->isExpandable(), true);
    EXPECT_EQ(cache.getEntry(3)->isExpandable(), false);
    EXPECT_EQ(cache.getEntry(4)->isExpandable(), false);
    EXPECT_EQ(cache.getEntry(5)->isExpandable(), true);

    editTree.setExpanded(cache.getEntry(2)->getPath(), true);
    editTree.setExpanded(cache.getEntry(5)->getPath(), true);

    cache.setFeatures(&record, nullptr);

    EXPECT_EQ(cache.getNumRows(), 8);
    EXPECT_EQ(cache.getEntry(0)->getIndent(), 0);
    EXPECT_EQ(cache.getEntry(1)->getIndent(), 1);
    EXPECT_EQ(cache.getEntry(2)->getIndent(), 0);
    EXPECT_EQ(cache.getEntry(3)->getIndent(), 1);
    EXPECT_EQ(cache.getEntry(4)->getIndent(), 0);
    EXPECT_EQ(cache.getEntry(5)->getIndent(), 0);
    EXPECT_EQ(cache.getEntry(6)->getIndent(), 0);
    EXPECT_EQ(cache.getEntry(7)->getIndent(), 0);
}
*/