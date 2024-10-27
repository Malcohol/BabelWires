#include <gtest/gtest.h>

#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/Project/FeatureElements/contentsCache.hpp>
#include <BabelWiresLib/Project/FeatureElements/editTree.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Types/File/fileType.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Identifiers/registeredIdentifier.hpp>

#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testModifier.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

namespace {
    std::unique_ptr<testUtils::LocalTestModifier> createIntModifier(babelwires::Path path,
                                                                    babelwires::FeatureElement* owner = nullptr) {
        auto data = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(8));
        data->m_targetPath = std::move(path);
        auto modPtr = std::make_unique<testUtils::LocalTestModifier>(std::move(data));
        modPtr->setOwner(owner);
        return modPtr;
    }

    std::unique_ptr<testUtils::LocalTestModifier> createStringModifier(babelwires::Path path,
                                                                       babelwires::FeatureElement* owner = nullptr) {
        auto data = std::make_unique<babelwires::ValueAssignmentData>(babelwires::StringValue("Hello"));
        data->m_targetPath = std::move(path);
        auto modPtr = std::make_unique<testUtils::LocalTestModifier>(std::move(data));
        modPtr->setOwner(owner);
        return modPtr;
    }

    void checkRootEntry(const babelwires::ContentsCacheEntry* entry,
                        testUtils::TestComplexRecordTypeFeatureInfo* inputInfo,
                        testUtils::TestComplexRecordTypeFeatureInfo* outputInfo) {
        // TODO EXPECT_EQ(entry->getLabel(), ...);
        if (inputInfo) {
            EXPECT_EQ(entry->getInput(), &inputInfo->m_record);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToRecord);
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutput(), &outputInfo->m_record);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToRecord);
        } else {
            EXPECT_FALSE(entry->getOutput());
        }
        EXPECT_TRUE(entry->isExpandable());
        EXPECT_TRUE(entry->isExpanded());
    }

    void checkFirstIntEntry(const babelwires::ContentsCacheEntry* entry,
                            testUtils::TestComplexRecordTypeFeatureInfo* inputInfo,
                            testUtils::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), testUtils::TestComplexRecordType::s_intFieldName);
        if (inputInfo) {
            EXPECT_EQ(entry->getInput(), &inputInfo->m_intFeature);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToInt);
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutput(), &outputInfo->m_intFeature);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToInt);
        } else {
            EXPECT_FALSE(entry->getOutput());
        }
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkSubRecordEntry(const babelwires::ContentsCacheEntry* entry,
                             testUtils::TestComplexRecordTypeFeatureInfo* inputInfo,
                             testUtils::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), testUtils::TestComplexRecordType::s_subRecordFieldName);
        if (inputInfo) {
            EXPECT_EQ(entry->getInput(), &inputInfo->m_subRecordFeature);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToSubRecord);
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutput(), &outputInfo->m_subRecordFeature);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToSubRecord);
        } else {
            EXPECT_FALSE(entry->getOutput());
        }
        EXPECT_TRUE(entry->isExpandable());
    }

    void checkSubRecordIntEntry(const babelwires::ContentsCacheEntry* entry,
                                testUtils::TestComplexRecordTypeFeatureInfo* inputInfo,
                                testUtils::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), testUtils::TestSimpleRecordType::s_int0FieldName);
        if (inputInfo) {
            EXPECT_EQ(entry->getInput(), &inputInfo->m_subRecordIntFeature);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToSubRecordInt);
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutput(), &outputInfo->m_subRecordIntFeature);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToSubRecordInt);
        } else {
            EXPECT_FALSE(entry->getOutput());
        }
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkArrayEntry(const babelwires::ContentsCacheEntry* entry,
                         testUtils::TestComplexRecordTypeFeatureInfo* inputInfo,
                         testUtils::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), testUtils::TestComplexRecordType::s_arrayFieldName);
        if (inputInfo) {
            EXPECT_EQ(entry->getInput(), &inputInfo->m_arrayFeature);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToArray);
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutput(), &outputInfo->m_arrayFeature);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToArray);
        } else {
            EXPECT_FALSE(entry->getOutput());
        }
        EXPECT_TRUE(entry->isExpandable());
    }

    void checkFirstArrayEntry(const babelwires::ContentsCacheEntry* entry,
                              testUtils::TestComplexRecordTypeFeatureInfo* inputInfo,
                              testUtils::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), "[0]");
        if (inputInfo) {
            EXPECT_EQ(entry->getInput(), &inputInfo->m_elem0);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToElem0);
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutput(), &outputInfo->m_elem0);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToElem0);
        } else {
            EXPECT_FALSE(entry->getOutput());
        }
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkSecondArrayEntry(const babelwires::ContentsCacheEntry* entry,
                               testUtils::TestComplexRecordTypeFeatureInfo* inputInfo,
                               testUtils::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), "[1]");
        if (inputInfo) {
            EXPECT_EQ(entry->getInput(), &inputInfo->m_elem1);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToElem1);
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutput(), &outputInfo->m_elem1);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToElem1);
        } else {
            EXPECT_FALSE(entry->getOutput());
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
                             babelwires::ValueTreeNode* inputFeature, babelwires::ValueTreeNode* outputFeature) {
        cache.setValueTrees("Test", inputFeature, outputFeature);
        ASSERT_EQ(cache.getNumRows(), 6);
        auto inputInfo =
            inputFeature ? std::make_unique<testUtils::TestComplexRecordTypeFeatureInfo>(*inputFeature) : nullptr;
        auto outputInfo =
            outputFeature ? std::make_unique<testUtils::TestComplexRecordTypeFeatureInfo>(*outputFeature) : nullptr;
        auto info = inputInfo ? inputInfo.get() : outputInfo.get();
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkRootEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkFirstIntEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
            checkSubRecordEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(5);
            checkArrayEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        // Expand the record
        editTree.setExpanded(info->m_pathToSubRecord, true);
        cache.setValueTrees("Test", inputFeature, outputFeature);
        ASSERT_EQ(cache.getNumRows(), 8);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkRootEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkFirstIntEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
            checkSubRecordEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(3);
            checkSubRecordIntEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(7);
            checkArrayEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        // Expand the array
        editTree.setExpanded(info->m_pathToArray, true);
        cache.setValueTrees("Test", inputFeature, outputFeature);
        ASSERT_EQ(cache.getNumRows(), 8 + testUtils::TestSimpleArrayType::s_defaultSize);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkRootEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkFirstIntEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
            checkSubRecordEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(3);
            checkSubRecordIntEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(7);
            checkArrayEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(8);
            checkFirstArrayEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(9);
            checkSecondArrayEntry(entry, inputInfo.get(), outputInfo.get());
            checkUnmodified(entry);
        }
    }

    void testModifierBehaviour(babelwires::ProjectContext& context, babelwires::ContentsCache& cache,
                               babelwires::EditTree& editTree, babelwires::ValueTreeNode* inputFeature,
                               babelwires::ValueTreeNode* outputFeature) {
        ASSERT_TRUE(inputFeature);

        auto inputInfo =
            inputFeature ? std::make_unique<testUtils::TestComplexRecordTypeFeatureInfo>(*inputFeature) : nullptr;
        auto outputInfo =
            outputFeature ? std::make_unique<testUtils::TestComplexRecordTypeFeatureInfo>(*outputFeature) : nullptr;
        auto info = inputInfo ? inputInfo.get() : outputInfo.get();

        // Add a modifier to the first int
        editTree.addModifier(createIntModifier(info->m_pathToInt));
        cache.updateModifierCache();

        ASSERT_EQ(cache.getNumRows(), 8 + testUtils::TestSimpleArrayType::s_defaultSize);
        for (int i = 2; i < 8 + testUtils::TestSimpleArrayType::s_defaultSize; ++i) {
            checkUnmodified(cache.getEntry(i));
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
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
        ASSERT_EQ(cache.getNumRows(), 8 + testUtils::TestSimpleArrayType::s_defaultSize);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(7);
            checkArrayEntry(entry, inputInfo.get(), outputInfo.get());
            EXPECT_FALSE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }
        checkUnmodified(cache.getEntry(8));
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(9);
            checkSecondArrayEntry(entry, inputInfo.get(), outputInfo.get());
            EXPECT_TRUE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }
        checkUnmodified(cache.getEntry(10));
        // Add a failing modifier to the array
        {
            testUtils::TestFeatureElement owner(context);
            auto modifier = createStringModifier(info->m_pathToElem0, &owner);
            modifier->simulateFailure();
            editTree.addModifier(std::move(modifier));
        }
        cache.updateModifierCache();
        ASSERT_EQ(cache.getNumRows(), 8 + testUtils::TestSimpleArrayType::s_defaultSize);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(7);
            checkArrayEntry(entry, inputInfo.get(), outputInfo.get());
            EXPECT_FALSE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(8);
            checkFirstArrayEntry(entry, inputInfo.get(), outputInfo.get());
            EXPECT_TRUE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_TRUE(entry->hasFailedModifier());
            EXPECT_FALSE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }

        // Collapse the array
        editTree.setExpanded(info->m_pathToArray, false);
        cache.setValueTrees("Test", inputFeature, outputFeature);
        ASSERT_EQ(cache.getNumRows(), 8);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(7);
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
        ASSERT_EQ(cache.getNumRows(), 8);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(7);
            checkArrayEntry(entry, inputInfo.get(), outputInfo.get());
            EXPECT_FALSE(entry->hasModifier());
            EXPECT_TRUE(entry->hasSubmodifiers());
            EXPECT_FALSE(entry->hasFailedModifier());
            EXPECT_TRUE(entry->hasHiddenModifier());
            EXPECT_FALSE(entry->hasFailedHiddenModifiers());
        }
        editTree.removeModifier(editTree.findModifier(info->m_pathToElem1));
        cache.updateModifierCache();
        ASSERT_EQ(cache.getNumRows(), 8);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(7);
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

    babelwires::ValueTreeRoot inputFeature(testEnvironment.m_typeSystem,
                                                testUtils::TestComplexRecordType::getThisIdentifier());
    inputFeature.setToDefault();
    editTree.setExpanded(babelwires::Path(), true);

    testCommonBehaviour(cache, editTree, &inputFeature, nullptr);
    testModifierBehaviour(testEnvironment.m_projectContext, cache, editTree, &inputFeature, nullptr);
}

TEST(ContentsCacheTest, outputFeatureOnly) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    babelwires::ValueTreeRoot outputFeature(testEnvironment.m_typeSystem,
                                                 testUtils::TestComplexRecordType::getThisIdentifier());
    outputFeature.setToDefault();
    editTree.setExpanded(babelwires::Path(), true);

    testCommonBehaviour(cache, editTree, nullptr, &outputFeature);
}

TEST(ContentsCacheTest, inputAndOutputFeature) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    babelwires::ValueTreeRoot inputFeature(testEnvironment.m_typeSystem,
                                                testUtils::TestComplexRecordType::getThisIdentifier());
    babelwires::ValueTreeRoot outputFeature(testEnvironment.m_typeSystem,
                                                 testUtils::TestComplexRecordType::getThisIdentifier());
    inputFeature.setToDefault();
    outputFeature.setToDefault();
    editTree.setExpanded(babelwires::Path(), true);

    testCommonBehaviour(cache, editTree, &inputFeature, &outputFeature);
    testModifierBehaviour(testEnvironment.m_projectContext, cache, editTree, &inputFeature, &outputFeature);
}

TEST(ContentsCacheTest, inputAndOutputDifferentFeatures) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    babelwires::ValueTreeRoot inputFeature(testEnvironment.m_typeSystem,
                                                testUtils::TestComplexRecordType::getThisIdentifier());
    babelwires::ValueTreeRoot outputFeature(testEnvironment.m_typeSystem,
                                                 testUtils::TestComplexRecordType::getThisIdentifier());

    inputFeature.setToDefault();
    outputFeature.setToDefault();
    editTree.setExpanded(babelwires::Path(), true);

    testUtils::TestComplexRecordTypeFeatureInfo inputInfo(inputFeature);
    testUtils::TestComplexRecordTypeFeatureInfo outputInfo(outputFeature);

    testUtils::TestComplexRecordType::Instance output(outputFeature);
    output.getarray().setSize(testUtils::TestSimpleArrayType::s_nonDefaultSize);

    cache.setValueTrees("Test", &inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 6);

    editTree.setExpanded(inputInfo.m_pathToSubRecord, true);
    cache.setValueTrees("Test", &inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 8);

    editTree.setExpanded(inputInfo.m_pathToArray, true);
    cache.setValueTrees("Test", &inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 8 + testUtils::TestSimpleArrayType::s_nonDefaultSize);

    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
        checkRootEntry(entry, &inputInfo, &outputInfo);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
        checkFirstIntEntry(entry, &inputInfo, &outputInfo);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(7);
        checkArrayEntry(entry, &inputInfo, &outputInfo);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(8);
        checkFirstArrayEntry(entry, &inputInfo, &outputInfo);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry =
            cache.getEntry(8 + testUtils::TestSimpleArrayType::s_nonDefaultSize - 1);
        EXPECT_FALSE(entry->getInput());
        EXPECT_EQ(entry->getOutput(),
                  outputInfo.m_arrayFeature.getFeature(testUtils::TestSimpleArrayType::s_nonDefaultSize - 1));
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
        checkUnmodified(entry);
    }
}

TEST(ContentsCacheTest, hiddenTopLevelModifiers) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    babelwires::ValueTreeRoot inputFeature(testEnvironment.m_typeSystem,
                                                testUtils::TestComplexRecordType::getThisIdentifier());
    babelwires::ValueTreeRoot outputFeature(testEnvironment.m_typeSystem,
                                                 testUtils::TestComplexRecordType::getThisIdentifier());

    inputFeature.setToDefault();
    outputFeature.setToDefault();
    editTree.setExpanded(babelwires::Path(), true);

    cache.setValueTrees("Test", &inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 6);

    // Adding a hidden failed modifier whose parent is logically the root requires us to present
    // a root entry to the user, so the UI functionality for failed modifiers has somewhere to live.
    testUtils::TestFeatureElement owner(testEnvironment.m_projectContext);
    auto modifierPtr = createIntModifier(babelwires::Path::deserializeFromString("flarg"), &owner);
    modifierPtr->simulateFailure();
    auto modifier = modifierPtr.get();
    editTree.addModifier(std::move(modifierPtr));

    testUtils::TestComplexRecordTypeFeatureInfo inputInfo(inputFeature);
    testUtils::TestComplexRecordTypeFeatureInfo outputInfo(outputFeature);

    cache.clearChanges();
    cache.updateModifierCache();

    EXPECT_FALSE(cache.isChanged(babelwires::ContentsCache::Changes::StructureChanged));
    EXPECT_EQ(cache.getNumRows(), 6);

    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
        EXPECT_EQ(entry->getLabel(), "Test");
        EXPECT_EQ(entry->getInput(), &inputFeature);
        EXPECT_EQ(entry->getOutput(), &outputFeature);
        EXPECT_EQ(entry->getPath(), babelwires::Path());
        EXPECT_TRUE(entry->isExpandable());
        EXPECT_TRUE(entry->isExpanded());
        EXPECT_FALSE(entry->hasModifier());
        EXPECT_TRUE(entry->hasSubmodifiers());
        EXPECT_FALSE(entry->hasFailedModifier());
        EXPECT_TRUE(entry->hasHiddenModifier());
        EXPECT_TRUE(entry->hasFailedHiddenModifiers());
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
        checkFirstIntEntry(entry, &inputInfo, &outputInfo);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
        checkSubRecordEntry(entry, &inputInfo, &outputInfo);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(5);
        checkArrayEntry(entry, &inputInfo, &outputInfo);
        checkUnmodified(entry);
    }

    editTree.removeModifier(modifier);
    cache.clearChanges();
    cache.updateModifierCache();

    EXPECT_FALSE(cache.isChanged(babelwires::ContentsCache::Changes::StructureChanged));
    EXPECT_EQ(cache.getNumRows(), 6);
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
        checkFirstIntEntry(entry, &inputInfo, &outputInfo);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
        checkSubRecordEntry(entry, &inputInfo, &outputInfo);
        checkUnmodified(entry);
    }
    {
        const babelwires::ContentsCacheEntry* const entry = cache.getEntry(5);
        checkArrayEntry(entry, &inputInfo, &outputInfo);
        checkUnmodified(entry);
    }
}

namespace {
    void checkFileRootEntry(const babelwires::ContentsCacheEntry* entry, babelwires::ValueTreeRoot* inputFeature,
                            babelwires::ValueTreeRoot* outputFeature) {
        EXPECT_EQ(entry->getLabel(), "Test");
        if (inputFeature) {
            EXPECT_EQ(entry->getInput(), inputFeature);
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputFeature) {
            EXPECT_EQ(entry->getOutput(), outputFeature);
        } else {
            EXPECT_FALSE(entry->getOutput());
        }
        EXPECT_EQ(entry->getPath(), babelwires::Path());
        EXPECT_TRUE(entry->isExpandable());
        EXPECT_TRUE(entry->isExpanded());
    }

    void checkFileContentsEntry(const babelwires::ContentsCacheEntry* entry, babelwires::ValueTreeRoot* inputFeature,
                           babelwires::ValueTreeRoot* outputFeature, bool isExpanded) {
        EXPECT_EQ(entry->getLabel(), "Contents");
        if (inputFeature) {
            EXPECT_EQ(entry->getInput(), inputFeature->getFeature(0));
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputFeature) {
            EXPECT_EQ(entry->getOutput(), outputFeature->getFeature(0));
        } else {
            EXPECT_FALSE(entry->getOutput());
        }
        EXPECT_EQ(entry->getPath().getNumSteps(), 1);
        EXPECT_EQ(*entry->getPath().getStep(0).asField(), babelwires::FileType::getStepToContents());
        EXPECT_TRUE(entry->isExpandable());
        EXPECT_EQ(entry->isExpanded(), isExpanded);
    }

    void checkFileIntEntry(const babelwires::ContentsCacheEntry* entry, babelwires::ValueTreeRoot* inputFeature,
                           babelwires::ValueTreeRoot* outputFeature) {
        EXPECT_EQ(entry->getLabel(), testUtils::TestSimpleRecordType::s_int0FieldName);
        if (inputFeature) {
            EXPECT_EQ(entry->getInput(), inputFeature->getFeature(0)->is<babelwires::ValueTreeNode>().getFeature(0));
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputFeature) {
            EXPECT_EQ(entry->getOutput(), outputFeature->getFeature(0)->is<babelwires::ValueTreeNode>().getFeature(0));
        } else {
            EXPECT_FALSE(entry->getOutput());
        }
        EXPECT_EQ(entry->getPath(), testUtils::getTestFileElementPathToInt0());
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void testFileCommonBehaviour(babelwires::ContentsCache& cache, babelwires::EditTree& editTree,
                                 babelwires::ValueTreeRoot* inputFeature,
                                 babelwires::ValueTreeRoot* outputFeature) {
                                    
        ASSERT_EQ(cache.getNumRows(), 2);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkFileRootEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkFileContentsEntry(entry, inputFeature, outputFeature, false);
            checkUnmodified(entry);
        }
        editTree.setExpanded(babelwires::Path({babelwires::PathStep(babelwires::FileType::getStepToContents())}), true);
        cache.setValueTrees("Test", inputFeature, outputFeature);
        ASSERT_EQ(cache.getNumRows(), 4);
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
            checkFileRootEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
            checkFileContentsEntry(entry, inputFeature, outputFeature, true);
            checkUnmodified(entry);
        }
        {
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
            checkFileIntEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
    }

    void testModifierBehaviour(babelwires::ProjectContext& context, babelwires::ContentsCache& cache,
                               babelwires::EditTree& editTree, babelwires::ValueTreeRoot* inputFeature,
                               babelwires::ValueTreeRoot* outputFeature) {
        ASSERT_TRUE(inputFeature);

        // Adding a hidden failed modifier whose parent is logically the root.
        // In this case, there's already an entry for the root, so no new
        // artificial entry need be added.
        testUtils::TestFeatureElement owner(context);
        auto modifierPtr = createIntModifier(babelwires::Path::deserializeFromString("flarg"), &owner);
        modifierPtr->simulateFailure();
        auto modifier = modifierPtr.get();
        editTree.addModifier(std::move(modifierPtr));

        cache.clearChanges();
        cache.updateModifierCache();

        EXPECT_FALSE(cache.isChanged(babelwires::ContentsCache::Changes::StructureChanged));
        ASSERT_EQ(cache.getNumRows(), 4);

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
            const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
            checkFileIntEntry(entry, inputFeature, outputFeature);
            checkUnmodified(entry);
        }
    }
} // namespace

TEST(ContentsCacheTest, inputFileFeatureOnly) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    babelwires::ValueTreeRoot inputFeature(testEnvironment.m_projectContext.m_typeSystem, testUtils::getTestFileType());
    inputFeature.setToDefault();
    editTree.setExpanded(babelwires::Path(), true);
    
    cache.setValueTrees("Test", &inputFeature, nullptr);
    testFileCommonBehaviour(cache, editTree, &inputFeature, nullptr);
    testModifierBehaviour(testEnvironment.m_projectContext, cache, editTree, &inputFeature, nullptr);
}

TEST(ContentsCacheTest, outputFileFeatureOnly) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    babelwires::ValueTreeRoot outputFeature(testEnvironment.m_projectContext.m_typeSystem, testUtils::getTestFileType());
    outputFeature.setToDefault();
    editTree.setExpanded(babelwires::Path(), true);

    cache.setValueTrees("Test", nullptr, &outputFeature);
    testFileCommonBehaviour(cache, editTree, nullptr, &outputFeature);
}

TEST(ContentsCacheTest, inputAndOutputFileFeature) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    babelwires::ValueTreeRoot inputFeature(testEnvironment.m_projectContext.m_typeSystem, testUtils::getTestFileType());
    babelwires::ValueTreeRoot outputFeature(testEnvironment.m_projectContext.m_typeSystem, testUtils::getTestFileType());
    inputFeature.setToDefault();
    outputFeature.setToDefault();
    editTree.setExpanded(babelwires::Path(), true);

    cache.setValueTrees("Test", &inputFeature, &outputFeature);
    testFileCommonBehaviour(cache, editTree, &inputFeature, &outputFeature);
    testModifierBehaviour(testEnvironment.m_projectContext, cache, editTree, &inputFeature, &outputFeature);
}
