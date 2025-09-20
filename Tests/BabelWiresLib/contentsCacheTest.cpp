#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/Nodes/contentsCache.hpp>
#include <BabelWiresLib/Project/Nodes/editTree.hpp>
#include <BabelWiresLib/Types/File/fileType.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Identifiers/registeredIdentifier.hpp>

#include <Domains/TestDomain/testArrayType.hpp>
#include <Domains/TestDomain/testFileFormats.hpp>
#include <Domains/TestDomain/testGenericType.hpp>
#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testModifier.hpp>
#include <Tests/BabelWiresLib/TestUtils/testNode.hpp>

namespace {
    std::unique_ptr<testUtils::LocalTestModifier> createIntModifier(babelwires::Path path,
                                                                    babelwires::Node* owner = nullptr) {
        auto data = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(8));
        data->m_targetPath = std::move(path);
        auto modPtr = std::make_unique<testUtils::LocalTestModifier>(std::move(data));
        modPtr->setOwner(owner);
        return modPtr;
    }

    std::unique_ptr<testUtils::LocalTestModifier> createStringModifier(babelwires::Path path,
                                                                       babelwires::Node* owner = nullptr) {
        auto data = std::make_unique<babelwires::ValueAssignmentData>(babelwires::StringValue("Hello"));
        data->m_targetPath = std::move(path);
        auto modPtr = std::make_unique<testUtils::LocalTestModifier>(std::move(data));
        modPtr->setOwner(owner);
        return modPtr;
    }

    void checkRootEntry(const babelwires::ContentsCacheEntry* entry,
                        testDomain::TestComplexRecordTypeFeatureInfo* inputInfo,
                        testDomain::TestComplexRecordTypeFeatureInfo* outputInfo) {
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
                            testDomain::TestComplexRecordTypeFeatureInfo* inputInfo,
                            testDomain::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), testDomain::TestComplexRecordType::s_intFieldName);
        if (inputInfo) {
            EXPECT_EQ(entry->getInput(), &inputInfo->m_int);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToInt);
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutput(), &outputInfo->m_int);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToInt);
        } else {
            EXPECT_FALSE(entry->getOutput());
        }
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkSubRecordEntry(const babelwires::ContentsCacheEntry* entry,
                             testDomain::TestComplexRecordTypeFeatureInfo* inputInfo,
                             testDomain::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), testDomain::TestComplexRecordType::s_subRecordFieldName);
        if (inputInfo) {
            EXPECT_EQ(entry->getInput(), &inputInfo->m_subRecord);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToSubRecord);
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutput(), &outputInfo->m_subRecord);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToSubRecord);
        } else {
            EXPECT_FALSE(entry->getOutput());
        }
        EXPECT_TRUE(entry->isExpandable());
    }

    void checkSubRecordIntEntry(const babelwires::ContentsCacheEntry* entry,
                                testDomain::TestComplexRecordTypeFeatureInfo* inputInfo,
                                testDomain::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), testDomain::TestSimpleRecordType::s_int0FieldName);
        if (inputInfo) {
            EXPECT_EQ(entry->getInput(), &inputInfo->m_subRecordInt);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToSubRecordInt);
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutput(), &outputInfo->m_subRecordInt);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToSubRecordInt);
        } else {
            EXPECT_FALSE(entry->getOutput());
        }
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void checkArrayEntry(const babelwires::ContentsCacheEntry* entry,
                         testDomain::TestComplexRecordTypeFeatureInfo* inputInfo,
                         testDomain::TestComplexRecordTypeFeatureInfo* outputInfo) {
        EXPECT_EQ(entry->getLabel(), testDomain::TestComplexRecordType::s_arrayFieldName);
        if (inputInfo) {
            EXPECT_EQ(entry->getInput(), &inputInfo->m_array);
            EXPECT_EQ(entry->getPath(), inputInfo->m_pathToArray);
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputInfo) {
            EXPECT_EQ(entry->getOutput(), &outputInfo->m_array);
            EXPECT_EQ(entry->getPath(), outputInfo->m_pathToArray);
        } else {
            EXPECT_FALSE(entry->getOutput());
        }
        EXPECT_TRUE(entry->isExpandable());
    }

    void checkFirstArrayEntry(const babelwires::ContentsCacheEntry* entry,
                              testDomain::TestComplexRecordTypeFeatureInfo* inputInfo,
                              testDomain::TestComplexRecordTypeFeatureInfo* outputInfo) {
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
                               testDomain::TestComplexRecordTypeFeatureInfo* inputInfo,
                               testDomain::TestComplexRecordTypeFeatureInfo* outputInfo) {
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
                             babelwires::ValueTreeNode* input, babelwires::ValueTreeNode* output) {
        cache.setValueTrees("Test", input, output);
        ASSERT_EQ(cache.getNumRows(), 6);
        auto inputInfo = input ? std::make_unique<testDomain::TestComplexRecordTypeFeatureInfo>(*input) : nullptr;
        auto outputInfo = output ? std::make_unique<testDomain::TestComplexRecordTypeFeatureInfo>(*output) : nullptr;
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
        cache.setValueTrees("Test", input, output);
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
        cache.setValueTrees("Test", input, output);
        ASSERT_EQ(cache.getNumRows(), 8 + testDomain::TestSimpleArrayType::s_defaultSize);
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
                               babelwires::EditTree& editTree, babelwires::ValueTreeNode* input,
                               babelwires::ValueTreeNode* output) {
        ASSERT_TRUE(input);

        auto inputInfo = input ? std::make_unique<testDomain::TestComplexRecordTypeFeatureInfo>(*input) : nullptr;
        auto outputInfo = output ? std::make_unique<testDomain::TestComplexRecordTypeFeatureInfo>(*output) : nullptr;
        auto info = inputInfo ? inputInfo.get() : outputInfo.get();

        // Add a modifier to the first int
        editTree.addModifier(createIntModifier(info->m_pathToInt));
        cache.updateModifierCache();

        ASSERT_EQ(cache.getNumRows(), 8 + testDomain::TestSimpleArrayType::s_defaultSize);
        for (int i = 2; i < 8 + testDomain::TestSimpleArrayType::s_defaultSize; ++i) {
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
        ASSERT_EQ(cache.getNumRows(), 8 + testDomain::TestSimpleArrayType::s_defaultSize);
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
            testUtils::TestNode owner(context);
            auto modifier = createStringModifier(info->m_pathToElem0, &owner);
            modifier->simulateFailure();
            editTree.addModifier(std::move(modifier));
        }
        cache.updateModifierCache();
        ASSERT_EQ(cache.getNumRows(), 8 + testDomain::TestSimpleArrayType::s_defaultSize);
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
        cache.setValueTrees("Test", input, output);
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
                                           testDomain::TestComplexRecordType::getThisType());
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
                                            testDomain::TestComplexRecordType::getThisType());
    outputFeature.setToDefault();
    editTree.setExpanded(babelwires::Path(), true);

    testCommonBehaviour(cache, editTree, nullptr, &outputFeature);
}

TEST(ContentsCacheTest, inputAndOutputFeature) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    babelwires::ValueTreeRoot inputFeature(testEnvironment.m_typeSystem,
                                           testDomain::TestComplexRecordType::getThisType());
    babelwires::ValueTreeRoot outputFeature(testEnvironment.m_typeSystem,
                                            testDomain::TestComplexRecordType::getThisType());
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
                                           testDomain::TestComplexRecordType::getThisType());
    babelwires::ValueTreeRoot outputFeature(testEnvironment.m_typeSystem,
                                            testDomain::TestComplexRecordType::getThisType());

    inputFeature.setToDefault();
    outputFeature.setToDefault();
    editTree.setExpanded(babelwires::Path(), true);

    testDomain::TestComplexRecordTypeFeatureInfo inputInfo(inputFeature);
    testDomain::TestComplexRecordTypeFeatureInfo outputInfo(outputFeature);

    testDomain::TestComplexRecordType::Instance output(outputFeature);
    output.getarray().setSize(testDomain::TestSimpleArrayType::s_nonDefaultSize);

    cache.setValueTrees("Test", &inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 6);

    editTree.setExpanded(inputInfo.m_pathToSubRecord, true);
    cache.setValueTrees("Test", &inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 8);

    editTree.setExpanded(inputInfo.m_pathToArray, true);
    cache.setValueTrees("Test", &inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 8 + testDomain::TestSimpleArrayType::s_nonDefaultSize);

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
            cache.getEntry(8 + testDomain::TestSimpleArrayType::s_nonDefaultSize - 1);
        EXPECT_FALSE(entry->getInput());
        EXPECT_EQ(entry->getOutput(),
                  outputInfo.m_array.getChild(testDomain::TestSimpleArrayType::s_nonDefaultSize - 1));
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
                                           testDomain::TestComplexRecordType::getThisType());
    babelwires::ValueTreeRoot outputFeature(testEnvironment.m_typeSystem,
                                            testDomain::TestComplexRecordType::getThisType());

    inputFeature.setToDefault();
    outputFeature.setToDefault();
    editTree.setExpanded(babelwires::Path(), true);

    cache.setValueTrees("Test", &inputFeature, &outputFeature);
    ASSERT_EQ(cache.getNumRows(), 6);

    // Adding a hidden failed modifier whose parent is logically the root requires us to present
    // a root entry to the user, so the UI functionality for failed modifiers has somewhere to live.
    testUtils::TestNode owner(testEnvironment.m_projectContext);
    auto modifierPtr = createIntModifier(babelwires::Path::deserializeFromString("flarg"), &owner);
    modifierPtr->simulateFailure();
    auto modifier = modifierPtr.get();
    editTree.addModifier(std::move(modifierPtr));

    testDomain::TestComplexRecordTypeFeatureInfo inputInfo(inputFeature);
    testDomain::TestComplexRecordTypeFeatureInfo outputInfo(outputFeature);

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
            EXPECT_EQ(entry->getInput(), inputFeature->getChild(0));
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputFeature) {
            EXPECT_EQ(entry->getOutput(), outputFeature->getChild(0));
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
        EXPECT_EQ(entry->getLabel(), testDomain::TestSimpleRecordType::s_int0FieldName);
        if (inputFeature) {
            EXPECT_EQ(entry->getInput(), inputFeature->getChild(0)->is<babelwires::ValueTreeNode>().getChild(0));
        } else {
            EXPECT_FALSE(entry->getInput());
        }
        if (outputFeature) {
            EXPECT_EQ(entry->getOutput(), outputFeature->getChild(0)->is<babelwires::ValueTreeNode>().getChild(0));
        } else {
            EXPECT_FALSE(entry->getOutput());
        }
        EXPECT_EQ(entry->getPath(), testDomain::getTestFileElementPathToInt0());
        EXPECT_FALSE(entry->isExpandable());
        EXPECT_FALSE(entry->isExpanded());
    }

    void testFileCommonBehaviour(babelwires::ContentsCache& cache, babelwires::EditTree& editTree,
                                 babelwires::ValueTreeRoot* inputFeature, babelwires::ValueTreeRoot* outputFeature) {

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
        editTree.setExpanded(babelwires::Path({babelwires::FileType::getStepToContents()}), true);
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
        testUtils::TestNode owner(context);
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

    babelwires::ValueTreeRoot inputFeature(testEnvironment.m_projectContext.m_typeSystem,
                                           testDomain::getTestFileType());
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

    babelwires::ValueTreeRoot outputFeature(testEnvironment.m_projectContext.m_typeSystem,
                                            testDomain::getTestFileType());
    outputFeature.setToDefault();
    editTree.setExpanded(babelwires::Path(), true);

    cache.setValueTrees("Test", nullptr, &outputFeature);
    testFileCommonBehaviour(cache, editTree, nullptr, &outputFeature);
}

TEST(ContentsCacheTest, inputAndOutputFileFeature) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    babelwires::ValueTreeRoot inputFeature(testEnvironment.m_projectContext.m_typeSystem,
                                           testDomain::getTestFileType());
    babelwires::ValueTreeRoot outputFeature(testEnvironment.m_projectContext.m_typeSystem,
                                            testDomain::getTestFileType());
    inputFeature.setToDefault();
    outputFeature.setToDefault();
    editTree.setExpanded(babelwires::Path(), true);

    cache.setValueTrees("Test", &inputFeature, &outputFeature);
    testFileCommonBehaviour(cache, editTree, &inputFeature, &outputFeature);
    testModifierBehaviour(testEnvironment.m_projectContext, cache, editTree, &inputFeature, &outputFeature);
}

TEST(ContentsCacheTest, unassignedTypeVariables_noAssignments) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EditTree editTree;
    babelwires::ContentsCache cache(editTree);

    babelwires::ValueTreeRoot valueTree(testEnvironment.m_projectContext.m_typeSystem,
                                        testDomain::TestGenericType::getThisType());
    valueTree.setToDefault();

    for (unsigned int i = 0; i < 5; ++i) {
        switch (i) {
            case 0:
                editTree.setExpanded(babelwires::Path(), true);
                break;
            case 1:
                editTree.setExpanded(testDomain::TestGenericType::getPathToWrappedType(), true);
                break;
            case 2:
                editTree.setExpanded(testDomain::TestGenericType::getPathToNestedGenericType(), true);
                break;
            case 3:
                editTree.setExpanded(testDomain::TestGenericType::getPathToNestedWrappedType(), true);
                break;
            case 4:
                editTree.setExpanded(testDomain::TestGenericType::getPathToArray(), true);
        }

        for (unsigned int k = 1; k < 4; ++k) {
            const bool hasInput = (k & 1) != 0;
            const bool hasOutput = (k & 2) != 0;

            cache.setValueTrees("Test", (hasInput ? &valueTree : nullptr), (hasOutput ? &valueTree : nullptr));

            ASSERT_LE(cache.getNumRows(), 11);
            {
                const babelwires::ContentsCacheEntry* const entry = cache.getEntry(0);
                EXPECT_EQ(entry->getPath(), babelwires::Path());
                EXPECT_EQ(entry->hasUnassignedInputTypeVariable(), false);
                EXPECT_EQ(entry->hasUnassignedOutputTypeVariable(), false);
                EXPECT_EQ(entry->isOrHasUnassignedInputTypeVariable(), false);
                EXPECT_EQ(entry->isOrHasUnassignedOutputTypeVariable(), false);
            }
            if (i == 0) continue;
            {
                const babelwires::ContentsCacheEntry* const entry = cache.getEntry(1);
                EXPECT_EQ(entry->getPath(), testDomain::TestGenericType::getPathToWrappedType());
                EXPECT_EQ(entry->hasUnassignedInputTypeVariable(), hasInput);
                EXPECT_EQ(entry->hasUnassignedOutputTypeVariable(), hasOutput);
                EXPECT_EQ(entry->isOrHasUnassignedInputTypeVariable(), hasInput);
                EXPECT_EQ(entry->isOrHasUnassignedOutputTypeVariable(), hasOutput);
            }
            if (i == 1) continue;
            {
                const babelwires::ContentsCacheEntry* const entry = cache.getEntry(2);
                EXPECT_EQ(entry->getPath(), testDomain::TestGenericType::getPathToX());
                EXPECT_EQ(entry->hasUnassignedInputTypeVariable(), false);
                EXPECT_EQ(entry->hasUnassignedOutputTypeVariable(), false);
                EXPECT_EQ(entry->isOrHasUnassignedInputTypeVariable(), hasInput);
                EXPECT_EQ(entry->isOrHasUnassignedOutputTypeVariable(), hasOutput);
            }
            {
                const babelwires::ContentsCacheEntry* const entry = cache.getEntry(3);
                EXPECT_EQ(entry->getPath(), testDomain::TestGenericType::getPathToY());
                EXPECT_EQ(entry->hasUnassignedInputTypeVariable(), false);
                EXPECT_EQ(entry->hasUnassignedOutputTypeVariable(), false);
                EXPECT_EQ(entry->isOrHasUnassignedInputTypeVariable(), hasInput);
                EXPECT_EQ(entry->isOrHasUnassignedOutputTypeVariable(), hasOutput);
            }
            {
                const babelwires::ContentsCacheEntry* const entry = cache.getEntry(4);
                EXPECT_EQ(entry->getPath(), testDomain::TestGenericType::getPathToInt());
                EXPECT_EQ(entry->hasUnassignedInputTypeVariable(), false);
                EXPECT_EQ(entry->hasUnassignedOutputTypeVariable(), false);
                EXPECT_EQ(entry->isOrHasUnassignedInputTypeVariable(), false);
                EXPECT_EQ(entry->isOrHasUnassignedOutputTypeVariable(), false);
            }
            {
                const babelwires::ContentsCacheEntry* const entry = cache.getEntry(5);
                EXPECT_EQ(entry->getPath(), testDomain::TestGenericType::getPathToNestedGenericType());
                EXPECT_EQ(entry->hasUnassignedInputTypeVariable(), hasInput);
                EXPECT_EQ(entry->hasUnassignedOutputTypeVariable(), hasOutput);
                EXPECT_EQ(entry->isOrHasUnassignedInputTypeVariable(), hasInput);
                EXPECT_EQ(entry->isOrHasUnassignedOutputTypeVariable(), hasOutput);
            }
            if (i == 2) continue;
            {
                const babelwires::ContentsCacheEntry* const entry = cache.getEntry(6);
                EXPECT_EQ(entry->getPath(), testDomain::TestGenericType::getPathToNestedWrappedType());
                EXPECT_EQ(entry->hasUnassignedInputTypeVariable(), hasInput);
                EXPECT_EQ(entry->hasUnassignedOutputTypeVariable(), hasOutput);
                EXPECT_EQ(entry->isOrHasUnassignedInputTypeVariable(), hasInput);
                EXPECT_EQ(entry->isOrHasUnassignedOutputTypeVariable(), hasOutput);
            }
            if (i == 3) continue;
            {
                const babelwires::ContentsCacheEntry* const entry = cache.getEntry(7);
                EXPECT_EQ(entry->getPath(), testDomain::TestGenericType::getPathToNestedX());
                EXPECT_EQ(entry->hasUnassignedInputTypeVariable(), false);
                EXPECT_EQ(entry->hasUnassignedOutputTypeVariable(), false);
                EXPECT_EQ(entry->isOrHasUnassignedInputTypeVariable(), hasInput);
                EXPECT_EQ(entry->isOrHasUnassignedOutputTypeVariable(), hasOutput);
            }
            {
                const babelwires::ContentsCacheEntry* const entry = cache.getEntry(8);
                EXPECT_EQ(entry->getPath(), testDomain::TestGenericType::getPathToNestedZ());
                EXPECT_EQ(entry->hasUnassignedInputTypeVariable(), false);
                EXPECT_EQ(entry->hasUnassignedOutputTypeVariable(), false);
                EXPECT_EQ(entry->isOrHasUnassignedInputTypeVariable(), hasInput);
                EXPECT_EQ(entry->isOrHasUnassignedOutputTypeVariable(), hasOutput);
            }
            {
                const babelwires::ContentsCacheEntry* const entry = cache.getEntry(9);
                EXPECT_EQ(entry->getPath(), testDomain::TestGenericType::getPathToArray());
                EXPECT_EQ(entry->hasUnassignedInputTypeVariable(), hasInput);
                EXPECT_EQ(entry->hasUnassignedOutputTypeVariable(), hasOutput);
                EXPECT_EQ(entry->isOrHasUnassignedInputTypeVariable(), hasInput);
                EXPECT_EQ(entry->isOrHasUnassignedOutputTypeVariable(), hasOutput);
            }
            if (i == 4) continue;
            {
                const babelwires::ContentsCacheEntry* const entry = cache.getEntry(10);
                EXPECT_EQ(entry->getPath(), testDomain::TestGenericType::getPathToArray0());
                EXPECT_EQ(entry->hasUnassignedInputTypeVariable(), false);
                EXPECT_EQ(entry->hasUnassignedOutputTypeVariable(), false);
                EXPECT_EQ(entry->isOrHasUnassignedInputTypeVariable(), hasInput);
                EXPECT_EQ(entry->isOrHasUnassignedOutputTypeVariable(), hasOutput);
            }
        }
    }
}