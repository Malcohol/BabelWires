#include <gtest/gtest.h>

#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/contentsCache.hpp>
#include <BabelWiresLib/Project/FeatureElements/editTree.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Identifiers/registeredIdentifier.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testModifier.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRootFeature.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

namespace {
    std::unique_ptr<testUtils::LocalTestModifier> createModifier(babelwires::FeaturePath path, int x,
                                                                 babelwires::FeatureElement* owner = nullptr) {
        auto data = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(x));
        data->m_pathToFeature = std::move(path);
        auto modPtr = std::make_unique<testUtils::LocalTestModifier>(std::move(data));
        modPtr->setOwner(owner);
        return modPtr;
    }

    void checkFirstIntEntry(const babelwires::ContentsCacheEntry* entry, testUtils::TestRootFeature* inputFeature,
                            testUtils::TestRootFeature* outputFeature) {
        EXPECT_EQ(entry->getLabel(), testUtils::TestRootFeature::s_intFieldName);
        if (inputFeature) {
            EXPECT_EQ(entry->getInputFeature(), inputFeature->m_intFeature);
        } else {
            EXPECT_FALSE(entry->getInputFeature());
        }
        if (outputFeature) {
            EXPECT_EQ(entry->getOutputFeature(), outputFeature->m_intFeature);
        } else {
            EXPECT_FALSE(entry->getOutputFeature());
        }
        EXPECT_EQ(entry->getPath(), testUtils::TestRootFeature::s_pathToInt);
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkArrayEntry(const babelwires::ContentsCacheEntry* entry, testUtils::TestRootFeature* inputFeature,
                         testUtils::TestRootFeature* outputFeature) {
        EXPECT_EQ(entry->getLabel(), testUtils::TestRootFeature::s_arrayFieldName);
        if (inputFeature) {
            EXPECT_EQ(entry->getInputFeature(), inputFeature->m_arrayFeature);
        } else {
            EXPECT_FALSE(entry->getInputFeature());
        }
        if (outputFeature) {
            EXPECT_EQ(entry->getOutputFeature(), outputFeature->m_arrayFeature);
        } else {
            EXPECT_FALSE(entry->getOutputFeature());
        }
        EXPECT_EQ(entry->getPath(), testUtils::TestRootFeature::s_pathToArray);
        EXPECT_TRUE(entry->isExpandable());
    }

    void checkSubRecordEntry(const babelwires::ContentsCacheEntry* entry, testUtils::TestRootFeature* inputFeature,
                             testUtils::TestRootFeature* outputFeature) {
        EXPECT_EQ(entry->getLabel(), testUtils::TestRootFeature::s_recordFieldName);
        if (inputFeature) {
            EXPECT_EQ(entry->getInputFeature(), inputFeature->m_subRecordFeature);
        } else {
            EXPECT_FALSE(entry->getInputFeature());
        }
        if (outputFeature) {
            EXPECT_EQ(entry->getOutputFeature(), outputFeature->m_subRecordFeature);
        } else {
            EXPECT_FALSE(entry->getOutputFeature());
        }
        EXPECT_EQ(entry->getPath(), testUtils::TestRootFeature::s_pathToRecord);
        EXPECT_TRUE(entry->isExpandable());
    }

    void checkSecondIntEntry(const babelwires::ContentsCacheEntry* entry, testUtils::TestRootFeature* inputFeature,
                             testUtils::TestRootFeature* outputFeature) {
        EXPECT_EQ(entry->getLabel(), testUtils::TestRootFeature::s_int2FieldName);
        if (inputFeature) {
            EXPECT_EQ(entry->getInputFeature(), inputFeature->m_intFeature2);
        } else {
            EXPECT_FALSE(entry->getInputFeature());
        }
        if (outputFeature) {
            EXPECT_EQ(entry->getOutputFeature(), outputFeature->m_intFeature2);
        } else {
            EXPECT_FALSE(entry->getOutputFeature());
        }
        EXPECT_EQ(entry->getPath(), testUtils::TestRootFeature::s_pathToInt2);
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkFirstArrayEntry(const babelwires::ContentsCacheEntry* entry, testUtils::TestRootFeature* inputFeature,
                              testUtils::TestRootFeature* outputFeature) {
        EXPECT_EQ(entry->getLabel(), "[0]");
        if (inputFeature) {
            EXPECT_EQ(entry->getInputFeature(), inputFeature->m_elem0);
        } else {
            EXPECT_FALSE(entry->getInputFeature());
        }
        if (outputFeature) {
            EXPECT_EQ(entry->getOutputFeature(), outputFeature->m_elem0);
        } else {
            EXPECT_FALSE(entry->getOutputFeature());
        }
        EXPECT_EQ(entry->getPath(), testUtils::TestRootFeature::s_pathToArray_0);
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkSecondArrayEntry(const babelwires::ContentsCacheEntry* entry, testUtils::TestRootFeature* inputFeature,
                               testUtils::TestRootFeature* outputFeature) {
        EXPECT_EQ(entry->getLabel(), "[1]");
        if (inputFeature) {
            EXPECT_EQ(entry->getInputFeature(), inputFeature->m_elem1);
        } else {
            EXPECT_FALSE(entry->getInputFeature());
        }
        if (outputFeature) {
            EXPECT_EQ(entry->getOutputFeature(), outputFeature->m_elem1);
        } else {
            EXPECT_FALSE(entry->getOutputFeature());
        }
        EXPECT_EQ(entry->getPath(), testUtils::TestRootFeature::s_pathToArray_1);
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
                             testUtils::TestRootFeature* inputFeature, testUtils::TestRootFeature* outputFeature) {
        cache.setFeatures(inputFeature, outputFeature);
        ASSERT_EQ(cache.getNumRows(), 3);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkFirstIntEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkArrayEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
            checkSubRecordEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        // Expand the record
        editTree.setExpanded(testUtils::TestRootFeature::s_pathToRecord, true);
        cache.setFeatures(inputFeature, outputFeature);
        ASSERT_EQ(cache.getNumRows(), 4);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkFirstIntEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkArrayEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
            checkSubRecordEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(3);
            checkSecondIntEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        // Expand the array
        editTree.setExpanded(testUtils::TestRootFeature::s_pathToArray, true);
        cache.setFeatures(inputFeature, outputFeature);
        ASSERT_EQ(cache.getNumRows(), 6);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkFirstIntEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkArrayEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
            checkFirstArrayEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(3);
            checkSecondArrayEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(4);
            checkSubRecordEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(5);
            checkSecondIntEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
    }

    void testModifierBehaviour(babelwires::ProjectContext& context, babelwires::ContentsCache& cache,
                               babelwires::EditTree& editTree, testUtils::TestRootFeature* inputFeature,
                               testUtils::TestRootFeature* outputFeature) {
        ASSERT_TRUE(inputFeature);

        // Add a modifier to the first int
        editTree.addModifier(createModifier(testUtils::TestRootFeature::s_pathToInt, 12));
        cache.updateModifierCache();
        ASSERT_EQ(cache.getNumRows(), 6);
        checkUnmodified(cache.getEntry(1));
        checkUnmodified(cache.getEntry(2));
        checkUnmodified(cache.getEntry(3));
        checkUnmodified(cache.getEntry(4));
        checkUnmodified(cache.getEntry(5));
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkFirstIntEntry(entry, inputFeature, outputFeature);
            EXPECT_TRUE(entry->hasModifier());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
            EXPECT_TRUE(entry->hasSubmodifiers());
        }
        // Add a submodifier to the array
        editTree.addModifier(createModifier(testUtils::TestRootFeature::s_pathToArray_1, 120));
        cache.updateModifierCache();
        ASSERT_EQ(cache.getNumRows(), 6);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkArrayEntry(entry, inputFeature, outputFeature);
            EXPECT_FALSE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }
        checkUnmodified(cache.getEntry(2));
        checkUnmodified(cache.getEntry(4));
        checkUnmodified(cache.getEntry(5));
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(3);
            checkSecondArrayEntry(entry, inputFeature, outputFeature);
            EXPECT_TRUE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }
        // Add a failing modifier to the array
        {
            testUtils::TestFeatureElement owner(context);
            auto modifier = createModifier(testUtils::TestRootFeature::s_pathToArray_0, 2000, &owner);
            modifier->simulateFailure();
            editTree.addModifier(std::move(modifier));
        }
        cache.updateModifierCache();
        ASSERT_EQ(cache.getNumRows(), 6);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkArrayEntry(entry, inputFeature, outputFeature);
            EXPECT_FALSE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
            checkFirstArrayEntry(entry, inputFeature, outputFeature);
            EXPECT_TRUE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_TRUE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }
        checkUnmodified(cache.getEntry(4));
        checkUnmodified(cache.getEntry(5));

        // Collapse the array
        editTree.setExpanded(testUtils::TestRootFeature::s_pathToArray, false);
        cache.setFeatures(inputFeature, outputFeature);
        ASSERT_EQ(cache.getNumRows(), 4);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkArrayEntry(entry, inputFeature, outputFeature);
            EXPECT_FALSE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_TRUE(entry->hasHiddenModifier());
            EXPECT_TRUE(entry->hasFailedHiddenModifiers());
        }
        // Remove the failing modifier
        editTree.removeModifier(editTree.findModifier(testUtils::TestRootFeature::s_pathToArray_0));
        cache.updateModifierCache();
        ASSERT_EQ(cache.getNumRows(), 4);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkArrayEntry(entry, inputFeature, outputFeature);
            EXPECT_FALSE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_TRUE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }
        editTree.removeModifier(editTree.findModifier(testUtils::TestRootFeature::s_pathToArray_1));
        cache.updateModifierCache();
        ASSERT_EQ(cache.getNumRows(), 4);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkArrayEntry(entry, inputFeature, outputFeature);
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

    testUtils::TestRootFeature inputFeature(testEnvironment.m_projectContext);
    inputFeature.setToDefault();

    testCommonBehaviour(cache, editTree, &inputFeature, nullptr);
    testModifierBehaviour(testEnvironment.m_projectContext, cache, editTree, &inputFeature, nullptr);
}

TEST(ContentsCacheTest, outputFeatureOnly) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    testUtils::TestRootFeature outputFeature(testEnvironment.m_projectContext);
    outputFeature.setToDefault();

    testCommonBehaviour(cache, editTree, nullptr, &outputFeature);
}

TEST(ContentsCacheTest, inputAndOutputFeature) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    testUtils::TestRootFeature inputFeature(testEnvironment.m_projectContext);
    testUtils::TestRootFeature outputFeature(testEnvironment.m_projectContext);
    inputFeature.setToDefault();
    outputFeature.setToDefault();

    testCommonBehaviour(cache, editTree, &inputFeature, &outputFeature);
    testModifierBehaviour(testEnvironment.m_projectContext, cache, editTree, &inputFeature, &outputFeature);
}

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
        checkSecondIntEntry(entry, &inputFeature, &outputFeature);
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
    auto modifierPtr = createModifier(babelwires::FeaturePath::deserializeFromString("flarg"), 2000, &owner);
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
        auto modifierPtr = createModifier(babelwires::FeaturePath::deserializeFromString("flarg"), 2000, &owner);
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
            addField(std::make_unique<TestRecordWithStyle>(Style::isCollapsable), testUtils::getTestRegisteredIdentifier("coll"));
            addField(std::make_unique<TestRecordWithStyle>(Style::isInlined), testUtils::getTestRegisteredIdentifier("inline"));
            addField(std::make_unique<TestRecordWithStyle>(Style::isCollapsable | Style::isInlined), testUtils::getTestRegisteredIdentifier("both"));
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