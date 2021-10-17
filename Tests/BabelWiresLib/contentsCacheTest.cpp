#include <gtest/gtest.h>

#include "BabelWiresLib/Identifiers/registeredIdentifier.hpp"
#include "BabelWiresLib/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/FeatureElements/contentsCache.hpp"
#include "BabelWiresLib/Project/FeatureElements/editTree.hpp"
#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWiresLib/TestUtils/testModifier.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

#include "Tests/TestUtils/testLog.hpp"

namespace {
    std::unique_ptr<libTestUtils::LocalTestModifier> createModifier(babelwires::FeaturePath path, int x,
                                                                    babelwires::FeatureElement* owner = nullptr) {
        auto data = std::make_unique<babelwires::IntValueAssignmentData>();
        data->m_pathToFeature = std::move(path);
        data->m_value = x;
        auto modPtr = std::make_unique<libTestUtils::LocalTestModifier>(std::move(data));
        modPtr->setOwner(owner);
        return modPtr;
    }

    void checkFirstIntEntry(const babelwires::ContentsCacheEntry* entry, libTestUtils::TestRecordFeature* inputFeature,
                            libTestUtils::TestRecordFeature* outputFeature) {
        EXPECT_EQ(entry->getLabel(), libTestUtils::TestRecordFeature::s_intFieldName);
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
        EXPECT_EQ(entry->getPath(), libTestUtils::TestRecordFeature::s_pathToInt);
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkArrayEntry(const babelwires::ContentsCacheEntry* entry, libTestUtils::TestRecordFeature* inputFeature,
                         libTestUtils::TestRecordFeature* outputFeature) {
        EXPECT_EQ(entry->getLabel(), libTestUtils::TestRecordFeature::s_arrayFieldName);
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
        EXPECT_EQ(entry->getPath(), libTestUtils::TestRecordFeature::s_pathToArray);
        EXPECT_TRUE(entry->isExpandable());
    }

    void checkSubRecordEntry(const babelwires::ContentsCacheEntry* entry, libTestUtils::TestRecordFeature* inputFeature,
                             libTestUtils::TestRecordFeature* outputFeature) {
        EXPECT_EQ(entry->getLabel(), libTestUtils::TestRecordFeature::s_recordFieldName);
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
        EXPECT_EQ(entry->getPath(), libTestUtils::TestRecordFeature::s_pathToRecord);
        EXPECT_TRUE(entry->isExpandable());
    }

    void checkSecondIntEntry(const babelwires::ContentsCacheEntry* entry, libTestUtils::TestRecordFeature* inputFeature,
                             libTestUtils::TestRecordFeature* outputFeature) {
        EXPECT_EQ(entry->getLabel(), libTestUtils::TestRecordFeature::s_int2FieldName);
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
        EXPECT_EQ(entry->getPath(), libTestUtils::TestRecordFeature::s_pathToInt2);
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkFirstArrayEntry(const babelwires::ContentsCacheEntry* entry,
                              libTestUtils::TestRecordFeature* inputFeature,
                              libTestUtils::TestRecordFeature* outputFeature) {
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
        EXPECT_EQ(entry->getPath(), libTestUtils::TestRecordFeature::s_pathToArray_0);
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkSecondArrayEntry(const babelwires::ContentsCacheEntry* entry,
                               libTestUtils::TestRecordFeature* inputFeature,
                               libTestUtils::TestRecordFeature* outputFeature) {
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
        EXPECT_EQ(entry->getPath(), libTestUtils::TestRecordFeature::s_pathToArray_1);
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
                             libTestUtils::TestRecordFeature* inputFeature,
                             libTestUtils::TestRecordFeature* outputFeature) {
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
        editTree.setExpanded(libTestUtils::TestRecordFeature::s_pathToRecord, true);
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
        editTree.setExpanded(libTestUtils::TestRecordFeature::s_pathToArray, true);
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

    void testModifierBehaviour(babelwires::ContentsCache& cache, babelwires::EditTree& editTree,
                               libTestUtils::TestRecordFeature* inputFeature,
                               libTestUtils::TestRecordFeature* outputFeature) {
        ASSERT_TRUE(inputFeature);

        // Add a modifier to the first int
        editTree.addModifier(createModifier(libTestUtils::TestRecordFeature::s_pathToInt, 12));
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
        editTree.addModifier(createModifier(libTestUtils::TestRecordFeature::s_pathToArray_1, 120));
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
            libTestUtils::TestFeatureElement owner;
            auto modifier = createModifier(libTestUtils::TestRecordFeature::s_pathToArray_0, 2000, &owner);
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
        editTree.setExpanded(libTestUtils::TestRecordFeature::s_pathToArray, false);
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
        editTree.removeModifier(editTree.findModifier(libTestUtils::TestRecordFeature::s_pathToArray_0));
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
        editTree.removeModifier(editTree.findModifier(libTestUtils::TestRecordFeature::s_pathToArray_1));
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
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestLog log;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    libTestUtils::TestRecordFeature inputFeature;
    inputFeature.setToDefault();

    testCommonBehaviour(cache, editTree, &inputFeature, nullptr);
    testModifierBehaviour(cache, editTree, &inputFeature, nullptr);
}

TEST(ContentsCacheTest, outputFeatureOnly) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestLog log;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    libTestUtils::TestRecordFeature outputFeature;
    outputFeature.setToDefault();

    testCommonBehaviour(cache, editTree, nullptr, &outputFeature);
}

TEST(ContentsCacheTest, inputAndOutputFeature) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestLog log;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    libTestUtils::TestRecordFeature inputFeature;
    libTestUtils::TestRecordFeature outputFeature;
    inputFeature.setToDefault();
    outputFeature.setToDefault();

    testCommonBehaviour(cache, editTree, &inputFeature, &outputFeature);
    testModifierBehaviour(cache, editTree, &inputFeature, &outputFeature);
}

TEST(ContentsCacheTest, inputAndOutputDifferentFeatures) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestLog log;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    libTestUtils::TestRecordFeature inputFeature(255, true);
    libTestUtils::TestRecordFeature outputFeature;
    inputFeature.setToDefault();
    outputFeature.setToDefault();

    outputFeature.m_arrayFeature->setSize(3);

    cache.setFeatures(&inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 3);

    editTree.setExpanded(libTestUtils::TestRecordFeature::s_pathToRecord, true);
    cache.setFeatures(&inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 5);

    editTree.setExpanded(libTestUtils::TestRecordFeature::s_pathToArray, true);
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
        EXPECT_EQ(entry->getPath(), libTestUtils::TestRecordFeature::s_pathToArray_2);
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
        EXPECT_EQ(entry->getLabel(), libTestUtils::TestRecordFeature::s_extraIntFieldName);
        EXPECT_FALSE(entry->getOutputFeature());
        EXPECT_EQ(entry->getInputFeature(), inputFeature.m_extraIntFeature);
        EXPECT_EQ(entry->getPath(), libTestUtils::TestRecordFeature::s_pathToExtraInt);
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
        checkUnmodified(entry);
    }
}

TEST(ContentsCacheTest, hiddenTopLevelModifiers) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestLog log;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    libTestUtils::TestRecordFeature inputFeature;
    libTestUtils::TestRecordFeature outputFeature;
    inputFeature.setToDefault();
    outputFeature.setToDefault();

    cache.setFeatures(&inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 3);

    // Adding a hidden failed modifier whose parent is logically the root requires us to present
    // a root entry to the user, so the UI functionality for failed modifiers has somewhere to live.
    libTestUtils::TestFeatureElement owner;
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
    void checkFileRootEntry(const babelwires::ContentsCacheEntry* entry, libTestUtils::TestFileFeature* inputFeature,
                            libTestUtils::TestFileFeature* outputFeature) {
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

    void checkFileIntEntry(const babelwires::ContentsCacheEntry* entry, libTestUtils::TestFileFeature* inputFeature,
                           libTestUtils::TestFileFeature* outputFeature) {
        EXPECT_EQ(entry->getLabel(), libTestUtils::TestFileFeature::s_intChildFieldName);
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
        EXPECT_EQ(entry->getPath(), libTestUtils::TestFileFeature::s_pathToIntChild);
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void testFileCommonBehaviour(babelwires::ContentsCache& cache, babelwires::EditTree& editTree,
                                 libTestUtils::TestFileFeature* inputFeature,
                                 libTestUtils::TestFileFeature* outputFeature) {
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

    void testModifierBehaviour(babelwires::ContentsCache& cache, babelwires::EditTree& editTree,
                               libTestUtils::TestFileFeature* inputFeature,
                               libTestUtils::TestFileFeature* outputFeature) {
        ASSERT_TRUE(inputFeature);

        // Adding a hidden failed modifier whose parent is logically the root.
        // In this case, there's already an entry for the root, so no new
        // artificial entry need be added.
        libTestUtils::TestFeatureElement owner;
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
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestLog log;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    libTestUtils::TestFileFeature inputFeature;
    inputFeature.setToDefault();

    cache.setFeatures(&inputFeature, nullptr);
    testFileCommonBehaviour(cache, editTree, &inputFeature, nullptr);
    testModifierBehaviour(cache, editTree, &inputFeature, nullptr);
}

TEST(ContentsCacheTest, outputFileFeatureOnly) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestLog log;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    libTestUtils::TestFileFeature outputFeature;
    outputFeature.setToDefault();

    cache.setFeatures(nullptr, &outputFeature);
    testFileCommonBehaviour(cache, editTree, nullptr, &outputFeature);
}

TEST(ContentsCacheTest, inputAndOutputFileFeature) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestLog log;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    libTestUtils::TestFileFeature inputFeature;
    libTestUtils::TestFileFeature outputFeature;
    inputFeature.setToDefault();
    outputFeature.setToDefault();

    cache.setFeatures(&inputFeature, &outputFeature);
    testFileCommonBehaviour(cache, editTree, &inputFeature, &outputFeature);
    testModifierBehaviour(cache, editTree, &inputFeature, &outputFeature);
}
