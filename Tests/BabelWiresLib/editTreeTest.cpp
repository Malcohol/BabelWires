#include <gtest/gtest.h>

#include <BabelWiresLib/Project/FeatureElements/editTree.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>
#include <BabelWiresLib/Project/FeatureElements/SourceFileElement/sourceFileElementData.hpp>
#include <BabelWiresLib/Project/FeatureElements/TargetFileElement/targetFileElement.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <Tests/TestUtils/equalSets.hpp>

namespace {
    std::unique_ptr<babelwires::Modifier> createModifier(babelwires::FeaturePath path, int x,
                                                         babelwires::FeatureElement* owner = nullptr) {
        auto data = std::make_unique<babelwires::ValueAssignmentData>(babelwires::IntValue(x));
        data->m_pathToFeature = std::move(path);
        auto modPtr = std::make_unique<babelwires::LocalModifier>(std::move(data));
        modPtr->setOwner(owner);
        return modPtr;
    }

    std::unique_ptr<babelwires::ConnectionModifier>
    createConnectionModifier(babelwires::FeaturePath path, babelwires::FeatureElement* owner = nullptr) {
        auto data = std::make_unique<babelwires::ConnectionModifierData>();
        data->m_pathToFeature = std::move(path);
        data->m_pathToSourceFeature = babelwires::FeaturePath();
        auto modPtr = std::make_unique<babelwires::ConnectionModifier>(std::move(data));
        modPtr->setOwner(owner);
        return modPtr;
    }
} // namespace

TEST(EditTreeTest, fieldAddFindRemoveEmptyPath) {
    babelwires::EditTree tree;

    babelwires::FeaturePath path;
    auto modPtr = createModifier(path, 1);
    babelwires::Modifier* mod = modPtr.get();

    tree.addModifier(std::move(modPtr));
    EXPECT_EQ(tree.findModifier(path), mod);

    auto modPtr2 = tree.removeModifier(mod);
    EXPECT_EQ(modPtr2.get(), mod);
    EXPECT_EQ(tree.findModifier(path), nullptr);
}


TEST(EditTreeTest, fieldAddFindRemove) {
    babelwires::EditTree tree;

    babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa");
    auto modPtr = createModifier(path, 1);
    babelwires::Modifier* mod = modPtr.get();

    tree.addModifier(std::move(modPtr));
    EXPECT_EQ(tree.findModifier(path), mod);

    auto modPtr2 = tree.removeModifier(mod);
    EXPECT_EQ(modPtr2.get(), mod);
    EXPECT_EQ(tree.findModifier(path), nullptr);
}

TEST(EditTreeTest, indexAddFindRemove) {
    babelwires::EditTree tree;

    babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("4");
    auto modPtr = createModifier(path, 1);
    babelwires::Modifier* mod = modPtr.get();

    tree.addModifier(std::move(modPtr));
    EXPECT_EQ(tree.findModifier(path), mod);

    auto modPtr2 = tree.removeModifier(mod);
    EXPECT_EQ(modPtr2.get(), mod);
    EXPECT_EQ(tree.findModifier(path), nullptr);
}

TEST(EditTreeTest, severalModifier) {
    babelwires::EditTree tree;

    const babelwires::FeaturePath path1 = babelwires::FeaturePath::deserializeFromString("bb/4");
    const babelwires::FeaturePath path2 = babelwires::FeaturePath::deserializeFromString("2/5");
    const babelwires::FeaturePath path3 = babelwires::FeaturePath::deserializeFromString("bb");
    const babelwires::FeaturePath path4 = babelwires::FeaturePath::deserializeFromString("cc/1");
    const babelwires::FeaturePath path5 = babelwires::FeaturePath::deserializeFromString("aa/3");
    const babelwires::FeaturePath path6 = babelwires::FeaturePath::deserializeFromString("cc/6");

    tree.addModifier(createModifier(path1, 1));
    tree.addModifier(createModifier(path2, 2));
    tree.addModifier(createModifier(path3, 3));
    tree.addModifier(createModifier(path4, 4));
    tree.addModifier(createModifier(path5, 5));
    tree.addModifier(createModifier(path6, 6));

    ASSERT_TRUE(tree.findModifier(path1));
    ASSERT_TRUE(tree.findModifier(path2));
    ASSERT_TRUE(tree.findModifier(path3));
    ASSERT_TRUE(tree.findModifier(path4));
    ASSERT_TRUE(tree.findModifier(path5));
    ASSERT_TRUE(tree.findModifier(path6));

    ASSERT_TRUE(tree.findModifier(path1)->getModifierData().as<babelwires::ValueAssignmentData>());
    ASSERT_TRUE(tree.findModifier(path2)->getModifierData().as<babelwires::ValueAssignmentData>());
    ASSERT_TRUE(tree.findModifier(path3)->getModifierData().as<babelwires::ValueAssignmentData>());
    ASSERT_TRUE(tree.findModifier(path4)->getModifierData().as<babelwires::ValueAssignmentData>());
    ASSERT_TRUE(tree.findModifier(path5)->getModifierData().as<babelwires::ValueAssignmentData>());
    ASSERT_TRUE(tree.findModifier(path6)->getModifierData().as<babelwires::ValueAssignmentData>());

    EXPECT_EQ(static_cast<const babelwires::ValueAssignmentData*>(&tree.findModifier(path1)->getModifierData())
                  ->getValue()
                  ->as<babelwires::IntValue>()
                  ->get(),
              1);
    EXPECT_EQ(
        static_cast<const babelwires::ValueAssignmentData*>(&tree.findModifier(path2)->getModifierData())->getValue()
                  ->as<babelwires::IntValue>()
                  ->get(), 2);
    EXPECT_EQ(
        static_cast<const babelwires::ValueAssignmentData*>(&tree.findModifier(path3)->getModifierData())->getValue()
                  ->as<babelwires::IntValue>()
                  ->get(), 3);
    EXPECT_EQ(
        static_cast<const babelwires::ValueAssignmentData*>(&tree.findModifier(path4)->getModifierData())->getValue()
                  ->as<babelwires::IntValue>()
                  ->get(), 4);
    EXPECT_EQ(
        static_cast<const babelwires::ValueAssignmentData*>(&tree.findModifier(path5)->getModifierData())->getValue()
                  ->as<babelwires::IntValue>()
                  ->get(), 5);
    EXPECT_EQ(
        static_cast<const babelwires::ValueAssignmentData*>(&tree.findModifier(path6)->getModifierData())->getValue()
                  ->as<babelwires::IntValue>()
                  ->get(), 6);

    tree.removeModifier(tree.findModifier(path3));

    EXPECT_TRUE(tree.findModifier(path1));
    EXPECT_TRUE(tree.findModifier(path2));
    EXPECT_FALSE(tree.findModifier(path3));
    EXPECT_TRUE(tree.findModifier(path4));
    EXPECT_TRUE(tree.findModifier(path5));
    EXPECT_TRUE(tree.findModifier(path6));

    tree.removeModifier(tree.findModifier(path5));

    EXPECT_TRUE(tree.findModifier(path1));
    EXPECT_TRUE(tree.findModifier(path2));
    EXPECT_FALSE(tree.findModifier(path3));
    EXPECT_TRUE(tree.findModifier(path4));
    EXPECT_FALSE(tree.findModifier(path5));
    EXPECT_TRUE(tree.findModifier(path6));

    tree.addModifier(createModifier(path5, 55));

    EXPECT_TRUE(tree.findModifier(path1));
    EXPECT_TRUE(tree.findModifier(path2));
    EXPECT_FALSE(tree.findModifier(path3));
    EXPECT_TRUE(tree.findModifier(path4));
    EXPECT_TRUE(tree.findModifier(path5));
    EXPECT_TRUE(tree.findModifier(path6));

    tree.removeModifier(tree.findModifier(path5));
    tree.removeModifier(tree.findModifier(path2));
    tree.removeModifier(tree.findModifier(path6));

    const babelwires::FeaturePath path22 = babelwires::FeaturePath::deserializeFromString("2/xx/2");
    tree.addModifier(createModifier(path22, 22));

    EXPECT_TRUE(tree.findModifier(path1));
    EXPECT_FALSE(tree.findModifier(path2));
    EXPECT_FALSE(tree.findModifier(path3));
    EXPECT_TRUE(tree.findModifier(path4));
    EXPECT_FALSE(tree.findModifier(path5));
    EXPECT_FALSE(tree.findModifier(path6));
    EXPECT_TRUE(tree.findModifier(path22));

    tree.removeModifier(tree.findModifier(path1));
    tree.removeModifier(tree.findModifier(path4));
    tree.removeModifier(tree.findModifier(path22));

    EXPECT_FALSE(tree.findModifier(path1));
    EXPECT_FALSE(tree.findModifier(path2));
    EXPECT_FALSE(tree.findModifier(path3));
    EXPECT_FALSE(tree.findModifier(path4));
    EXPECT_FALSE(tree.findModifier(path5));
    EXPECT_FALSE(tree.findModifier(path6));
    EXPECT_FALSE(tree.findModifier(path22));
}

TEST(EditTreeTest, expandCollapse) {
    babelwires::EditTree tree;

    // These tests assume c_expandedByDefault is false.
    static_assert(babelwires::EditTree::c_expandedByDefault == false);

    EXPECT_FALSE(tree.isExpanded(babelwires::FeaturePath()));
    tree.setExpanded(babelwires::FeaturePath(), true);
    EXPECT_TRUE(tree.isExpanded(babelwires::FeaturePath()));
    tree.setExpanded(babelwires::FeaturePath(), false);
    EXPECT_FALSE(tree.isExpanded(babelwires::FeaturePath()));

    const babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("bb/4");
    EXPECT_EQ(tree.isExpanded(path), false);

    tree.setExpanded(path, true);

    EXPECT_TRUE(tree.isExpanded(path));

    tree.setExpanded(path, false);
    EXPECT_FALSE(tree.isExpanded(path));
}

TEST(EditTreeTest, severalExpandsAndCollapses) {
    babelwires::EditTree tree;

    const babelwires::FeaturePath path1 = babelwires::FeaturePath::deserializeFromString("0/xxx/31");
    const babelwires::FeaturePath path2 = babelwires::FeaturePath::deserializeFromString("qtwe/34");
    const babelwires::FeaturePath path3 = babelwires::FeaturePath::deserializeFromString("34/qtwe");
    const babelwires::FeaturePath path4 = babelwires::FeaturePath::deserializeFromString("0/xxx/0");
    const babelwires::FeaturePath path5 = babelwires::FeaturePath::deserializeFromString("35");
    const babelwires::FeaturePath path6 = babelwires::FeaturePath::deserializeFromString("35/36");

    tree.setExpanded(path1, true);
    tree.setExpanded(path2, true);
    tree.setExpanded(path3, true);
    tree.setExpanded(path4, true);
    tree.setExpanded(path5, true);
    tree.setExpanded(path6, true);

    tree.setExpanded(path1, false);

    EXPECT_FALSE(tree.isExpanded(path1));
    EXPECT_TRUE(tree.isExpanded(path2));
    EXPECT_TRUE(tree.isExpanded(path3));
    EXPECT_TRUE(tree.isExpanded(path4));
    EXPECT_TRUE(tree.isExpanded(path5));
    EXPECT_TRUE(tree.isExpanded(path6));

    tree.setExpanded(path4, false);
    tree.setExpanded(path5, false);
    tree.setExpanded(path1, true);

    EXPECT_TRUE(tree.isExpanded(path1));
    EXPECT_TRUE(tree.isExpanded(path2));
    EXPECT_TRUE(tree.isExpanded(path3));
    EXPECT_FALSE(tree.isExpanded(path4));
    EXPECT_FALSE(tree.isExpanded(path5));
    EXPECT_TRUE(tree.isExpanded(path6));

    tree.setExpanded(path2, false);
    tree.setExpanded(path3, false);
    tree.setExpanded(path6, false);
    tree.setExpanded(path1, false);

    EXPECT_FALSE(tree.isExpanded(path1));
    EXPECT_FALSE(tree.isExpanded(path2));
    EXPECT_FALSE(tree.isExpanded(path3));
    EXPECT_FALSE(tree.isExpanded(path4));
    EXPECT_FALSE(tree.isExpanded(path5));
    EXPECT_FALSE(tree.isExpanded(path6));
}

// Clearing changes can affect the structure of the tree.
TEST(EditTreeTest, expandsAndCollapsesChanges) {
    babelwires::EditTree tree;

    const babelwires::FeaturePath path1 = babelwires::FeaturePath::deserializeFromString("tt/tt/tt/4");
    const babelwires::FeaturePath path2 = babelwires::FeaturePath::deserializeFromString("89/www");
    const babelwires::FeaturePath path3 = babelwires::FeaturePath::deserializeFromString("89/www/0");
    const babelwires::FeaturePath path4 = babelwires::FeaturePath::deserializeFromString("et/tt/5");
    const babelwires::FeaturePath path5 = babelwires::FeaturePath::deserializeFromString("tt/tt/tt/2");
    const babelwires::FeaturePath path6 = babelwires::FeaturePath::deserializeFromString("tt/tt/tt/3");

    tree.setExpanded(path1, true);
    tree.setExpanded(path2, true);
    tree.setExpanded(path3, true);
    tree.setExpanded(path4, true);
    tree.setExpanded(path5, true);
    tree.setExpanded(path6, true);

    EXPECT_TRUE(tree.isExpanded(path1));
    EXPECT_TRUE(tree.isExpanded(path2));
    EXPECT_TRUE(tree.isExpanded(path3));
    EXPECT_TRUE(tree.isExpanded(path4));
    EXPECT_TRUE(tree.isExpanded(path5));
    EXPECT_TRUE(tree.isExpanded(path6));

    tree.setExpanded(path1, false);
    tree.clearChanges();

    EXPECT_FALSE(tree.isExpanded(path1));
    EXPECT_TRUE(tree.isExpanded(path2));
    EXPECT_TRUE(tree.isExpanded(path3));
    EXPECT_TRUE(tree.isExpanded(path4));
    EXPECT_TRUE(tree.isExpanded(path5));
    EXPECT_TRUE(tree.isExpanded(path6));

    tree.setExpanded(path4, false);
    tree.setExpanded(path5, false);
    tree.setExpanded(path1, true);
    tree.clearChanges();

    EXPECT_TRUE(tree.isExpanded(path1));
    EXPECT_TRUE(tree.isExpanded(path2));
    EXPECT_TRUE(tree.isExpanded(path3));
    EXPECT_FALSE(tree.isExpanded(path4));
    EXPECT_FALSE(tree.isExpanded(path5));
    EXPECT_TRUE(tree.isExpanded(path6));

    tree.setExpanded(path2, false);
    tree.setExpanded(path3, false);
    tree.setExpanded(path6, false);
    tree.setExpanded(path1, false);
    tree.clearChanges();

    EXPECT_FALSE(tree.isExpanded(path1));
    EXPECT_FALSE(tree.isExpanded(path2));
    EXPECT_FALSE(tree.isExpanded(path3));
    EXPECT_FALSE(tree.isExpanded(path4));
    EXPECT_FALSE(tree.isExpanded(path5));
    EXPECT_FALSE(tree.isExpanded(path6));
}

TEST(EditTreeTest, modifiersAndExpansion) {
    babelwires::EditTree tree;

    const babelwires::FeaturePath path1 = babelwires::FeaturePath::deserializeFromString("bb/4");
    const babelwires::FeaturePath path2 = babelwires::FeaturePath::deserializeFromString("bb/4");
    const babelwires::FeaturePath path3 = babelwires::FeaturePath::deserializeFromString("bb");
    const babelwires::FeaturePath path4 = babelwires::FeaturePath::deserializeFromString("cc/1");
    const babelwires::FeaturePath path5 = babelwires::FeaturePath::deserializeFromString("aa/3");
    const babelwires::FeaturePath path6 = babelwires::FeaturePath::deserializeFromString("cc/6");

    tree.setExpanded(path1, true);
    tree.addModifier(createModifier(path2, 2));
    tree.addModifier(createModifier(path3, 3));
    tree.setExpanded(path4, true);
    tree.addModifier(createModifier(path5, 5));
    tree.setExpanded(path6, true);

    EXPECT_TRUE(tree.isExpanded(path1));
    EXPECT_TRUE(tree.findModifier(path2));
    EXPECT_TRUE(tree.findModifier(path3));
    EXPECT_TRUE(tree.isExpanded(path4));
    EXPECT_TRUE(tree.findModifier(path5));
    EXPECT_TRUE(tree.isExpanded(path6));

    tree.removeModifier(tree.findModifier(path3));

    EXPECT_TRUE(tree.isExpanded(path1));
    EXPECT_TRUE(tree.findModifier(path2));
    EXPECT_FALSE(tree.findModifier(path3));
    EXPECT_TRUE(tree.isExpanded(path4));
    EXPECT_TRUE(tree.findModifier(path5));
    EXPECT_TRUE(tree.isExpanded(path6));

    tree.setExpanded(path4, false);
    tree.setExpanded(path6, false);
    tree.clearChanges();

    EXPECT_TRUE(tree.isExpanded(path1));
    EXPECT_TRUE(tree.findModifier(path2));
    EXPECT_FALSE(tree.findModifier(path3));
    EXPECT_FALSE(tree.isExpanded(path4));
    EXPECT_TRUE(tree.findModifier(path5));
    EXPECT_FALSE(tree.isExpanded(path6));

    tree.removeModifier(tree.findModifier(path2));

    EXPECT_TRUE(tree.isExpanded(path1));
    EXPECT_FALSE(tree.findModifier(path2));
    EXPECT_FALSE(tree.findModifier(path3));
    EXPECT_FALSE(tree.isExpanded(path4));
    EXPECT_TRUE(tree.findModifier(path5));
    EXPECT_FALSE(tree.isExpanded(path6));

    tree.setExpanded(path1, false);
    tree.clearChanges();

    EXPECT_FALSE(tree.isExpanded(path1));
    EXPECT_FALSE(tree.findModifier(path2));
    EXPECT_FALSE(tree.findModifier(path3));
    EXPECT_FALSE(tree.isExpanded(path4));
    EXPECT_TRUE(tree.findModifier(path5));
    EXPECT_FALSE(tree.isExpanded(path6));

    tree.removeModifier(tree.findModifier(path5));

    EXPECT_FALSE(tree.isExpanded(path1));
    EXPECT_FALSE(tree.findModifier(path2));
    EXPECT_FALSE(tree.findModifier(path3));
    EXPECT_FALSE(tree.isExpanded(path4));
    EXPECT_FALSE(tree.findModifier(path5));
    EXPECT_FALSE(tree.isExpanded(path6));
}

TEST(EditTreeTest, getAllExplicitlyExpandedPaths) {
    babelwires::EditTree tree;

    const babelwires::FeaturePath path1 = babelwires::FeaturePath::deserializeFromString("tt/tt/tt/4");
    const babelwires::FeaturePath path2 = babelwires::FeaturePath::deserializeFromString("89/www");
    const babelwires::FeaturePath path3 = babelwires::FeaturePath::deserializeFromString("89/www/0");
    const babelwires::FeaturePath path4 = babelwires::FeaturePath::deserializeFromString("et/tt/5");
    const babelwires::FeaturePath path5 = babelwires::FeaturePath::deserializeFromString("tt/tt/tt/2");
    const babelwires::FeaturePath path6 = babelwires::FeaturePath::deserializeFromString("tt/tt/tt/3");

    tree.setExpanded(path1, true);
    tree.setExpanded(path2, true);
    tree.addModifier(createModifier(path3, 3));
    tree.setExpanded(path4, true);
    tree.setExpanded(path5, true);
    tree.addModifier(createModifier(path5, 5));
    tree.setExpanded(path6, true);

    using setOfPaths = std::vector<babelwires::FeaturePath>;

    EXPECT_TRUE(
        testUtils::areEqualSets(tree.getAllExplicitlyExpandedPaths(), setOfPaths{path1, path2, path4, path5, path6}));

    tree.setExpanded(path3, true);

    EXPECT_TRUE(testUtils::areEqualSets(tree.getAllExplicitlyExpandedPaths(),
                                        setOfPaths{path1, path2, path3, path4, path5, path6}));

    tree.setExpanded(path3, false);

    EXPECT_TRUE(
        testUtils::areEqualSets(tree.getAllExplicitlyExpandedPaths(), setOfPaths{path1, path2, path4, path5, path6}));

    tree.setExpanded(path6, false);
    tree.setExpanded(path1, false);
    tree.removeModifier(tree.findModifier(path5));
    tree.clearChanges();

    EXPECT_TRUE(testUtils::areEqualSets(tree.getAllExplicitlyExpandedPaths(), setOfPaths{path2, path4, path5}));

    tree.setExpanded(path2, false);
    tree.setExpanded(path4, false);
    tree.setExpanded(path5, false);
    tree.clearChanges();

    EXPECT_TRUE(testUtils::areEqualSets(tree.getAllExplicitlyExpandedPaths(), setOfPaths{}));
}

namespace {
    struct TestOwner : babelwires::FeatureElement {
        TestOwner()
            : FeatureElement(babelwires::SourceFileElementData(), 0) {}

        void doProcess(babelwires::UserLogger& userLogger) override {}
    };
} // namespace

TEST(EditTreeTest, adjustArrayIndices) {
    TestOwner owner;

    babelwires::EditTree tree;

    tree.setExpanded(babelwires::FeaturePath::deserializeFromString("bb/4"), true);
    tree.setExpanded(babelwires::FeaturePath::deserializeFromString("bb/5"), true);
    tree.addModifier(createModifier(babelwires::FeaturePath::deserializeFromString("bb/1"), 3, &owner));
    tree.setExpanded(babelwires::FeaturePath::deserializeFromString("bb/3/erm/2"), true);
    tree.setExpanded(babelwires::FeaturePath::deserializeFromString("bb/3/erm/3"), true);
    tree.addModifier(createModifier(babelwires::FeaturePath::deserializeFromString("bb/3/erm/3"), 5, &owner));
    tree.setExpanded(babelwires::FeaturePath::deserializeFromString("cc/6"), true);

    tree.adjustArrayIndices(babelwires::FeaturePath::deserializeFromString("bb"), 0, 1);

    EXPECT_FALSE(tree.isExpanded(babelwires::FeaturePath::deserializeFromString("bb/4")));
    EXPECT_TRUE(tree.isExpanded(babelwires::FeaturePath::deserializeFromString("bb/5")));
    EXPECT_TRUE(tree.isExpanded(babelwires::FeaturePath::deserializeFromString("bb/6")));
    EXPECT_FALSE(tree.findModifier(babelwires::FeaturePath::deserializeFromString("bb/1")));
    EXPECT_TRUE(tree.findModifier(babelwires::FeaturePath::deserializeFromString("bb/2")));
    EXPECT_FALSE(tree.isExpanded(babelwires::FeaturePath::deserializeFromString("bb/3/erm/2")));
    EXPECT_TRUE(tree.isExpanded(babelwires::FeaturePath::deserializeFromString("bb/4/erm/2")));
    EXPECT_TRUE(tree.isExpanded(babelwires::FeaturePath::deserializeFromString("bb/4/erm/3")));
    EXPECT_FALSE(tree.findModifier(babelwires::FeaturePath::deserializeFromString("bb/3/erm/3")));
    EXPECT_TRUE(tree.findModifier(babelwires::FeaturePath::deserializeFromString("bb/4/erm/3")));
    EXPECT_TRUE(tree.isExpanded(babelwires::FeaturePath::deserializeFromString("cc/6")));

    tree.adjustArrayIndices(babelwires::FeaturePath::deserializeFromString("bb/4/erm"), 2, -2);

    EXPECT_TRUE(tree.isExpanded(babelwires::FeaturePath::deserializeFromString("bb/5")));
    EXPECT_TRUE(tree.isExpanded(babelwires::FeaturePath::deserializeFromString("bb/6")));
    EXPECT_TRUE(tree.findModifier(babelwires::FeaturePath::deserializeFromString("bb/2")));
    EXPECT_TRUE(tree.isExpanded(babelwires::FeaturePath::deserializeFromString("bb/4/erm/0")));
    EXPECT_TRUE(tree.isExpanded(babelwires::FeaturePath::deserializeFromString("bb/4/erm/1")));
    EXPECT_TRUE(tree.findModifier(babelwires::FeaturePath::deserializeFromString("bb/4/erm/1")));
    EXPECT_FALSE(tree.isExpanded(babelwires::FeaturePath::deserializeFromString("bb/4/erm/2")));
    EXPECT_FALSE(tree.isExpanded(babelwires::FeaturePath::deserializeFromString("bb/4/erm/2")));
    EXPECT_FALSE(tree.findModifier(babelwires::FeaturePath::deserializeFromString("bb/4/erm/3")));
    EXPECT_TRUE(tree.isExpanded(babelwires::FeaturePath::deserializeFromString("cc/6")));
}

TEST(EditTreeTest, truncatePaths) {
    babelwires::EditTree tree;
    tree.setImplicitlyExpanded(babelwires::FeaturePath(), true);

    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::CurrentState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa"));
    }
    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::PreviousState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa"));
    }

    tree.setExpanded(babelwires::FeaturePath::deserializeFromString("aa"), true);

    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::CurrentState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5"));
    }
    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::PreviousState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa"));
    }

    tree.clearChanges();

    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::CurrentState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5"));
    }
    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::PreviousState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5"));
    }

    tree.setExpanded(babelwires::FeaturePath::deserializeFromString("aa/5"), true);

    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::CurrentState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5/bb"));
    }
    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::PreviousState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5"));
    }

    tree.clearChanges();

    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::CurrentState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5/bb"));
    }
    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::PreviousState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5/bb"));
    }

    tree.setExpanded(babelwires::FeaturePath::deserializeFromString("aa"), false);

    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::CurrentState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa"));
    }
    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::PreviousState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5/bb"));
    }

    tree.clearChanges();

    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::CurrentState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa"));
    }
    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::PreviousState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa"));
    }
}

TEST(EditTreeTest, truncatePathsWithImplicitlyExpandedPaths) {
    babelwires::EditTree tree;
    tree.setImplicitlyExpanded(babelwires::FeaturePath(), true);

    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::CurrentState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa"));
    }
    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::PreviousState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa"));
    }

    tree.setExpanded(babelwires::FeaturePath::deserializeFromString("aa"), true);
    // An implicitly expanded path might only be encountered after a path was expanded.
    tree.setImplicitlyExpanded(babelwires::FeaturePath::deserializeFromString("aa/5"), true);

    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::CurrentState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5/bb"));
    }
    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::PreviousState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa"));
    }

    tree.clearChanges();

    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::CurrentState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5/bb"));
    }
    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::PreviousState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5/bb"));
    }

    tree.setExpanded(babelwires::FeaturePath::deserializeFromString("aa/5/bb"), true);

    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::CurrentState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc"));
    }
    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::PreviousState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5/bb"));
    }

    tree.clearChanges();

    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::CurrentState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc"));
    }
    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::PreviousState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc"));
    }

    EXPECT_TRUE(testUtils::areEqualSets(
        std::vector<babelwires::FeaturePath>{babelwires::FeaturePath::deserializeFromString("aa"),
                                             babelwires::FeaturePath::deserializeFromString("aa/5/bb")},
        tree.getAllExplicitlyExpandedPaths()));

    tree.setExpanded(babelwires::FeaturePath::deserializeFromString("aa"), false);

    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::CurrentState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa"));
    }
    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::PreviousState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc"));
    }

    tree.clearChanges();

    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::CurrentState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa"));
    }
    {
        babelwires::FeaturePath path = babelwires::FeaturePath::deserializeFromString("aa/5/bb/cc");
        tree.truncatePathAtFirstCollapsedNode(path, babelwires::EditTree::State::PreviousState);
        EXPECT_EQ(path, babelwires::FeaturePath::deserializeFromString("aa"));
    }

    EXPECT_TRUE(testUtils::areEqualSets(
        std::vector<babelwires::FeaturePath>{babelwires::FeaturePath::deserializeFromString("aa/5/bb")},
        tree.getAllExplicitlyExpandedPaths()));
}

TEST(EditTreeTest, treeIteration) {
    babelwires::EditTree tree;

    const babelwires::FeaturePath path1 = babelwires::FeaturePath::deserializeFromString("bb/6");
    const babelwires::FeaturePath path2 = babelwires::FeaturePath::deserializeFromString("bb/4");
    const babelwires::FeaturePath path3 = babelwires::FeaturePath::deserializeFromString("bb");
    const babelwires::FeaturePath path4 = babelwires::FeaturePath::deserializeFromString("cc/1");
    const babelwires::FeaturePath path5 = babelwires::FeaturePath::deserializeFromString("aa/3");
    const babelwires::FeaturePath path6 = babelwires::FeaturePath::deserializeFromString("cc/6");

    tree.setExpanded(path1, true);
    tree.addModifier(createModifier(path2, 2));
    tree.addModifier(createModifier(path3, 3));
    tree.setExpanded(path4, true);
    tree.addModifier(createModifier(path5, 5));
    tree.setExpanded(path6, true);

    auto rit = tree.begin();
    EXPECT_NE(rit, tree.end());
    
    auto it = rit.childrenBegin();
    EXPECT_NE(it, rit.childrenEnd());
    EXPECT_EQ(it.getStep(), babelwires::PathStep(babelwires::ShortId("aa")));
    EXPECT_EQ(it.getModifier(), nullptr);
    {
        auto cit = it.childrenBegin();
        EXPECT_NE(cit, it.childrenEnd());
        EXPECT_EQ(cit.getStep(), babelwires::PathStep(3));
        ASSERT_NE(cit.getModifier(), nullptr);
        EXPECT_EQ(cit.getModifier()->getModifierData().m_pathToFeature, path5);
        cit.nextSibling();
        EXPECT_EQ(cit, it.childrenEnd());
    }
    it.nextSibling();
    EXPECT_NE(it, rit.childrenEnd());
    EXPECT_EQ(it.getStep(), babelwires::PathStep(babelwires::ShortId("bb")));
    ASSERT_NE(it.getModifier(), nullptr);
    EXPECT_EQ(it.getModifier()->getModifierData().m_pathToFeature, path3);
    {
        auto cit = it.childrenBegin();
        EXPECT_NE(cit, it.childrenEnd());
        EXPECT_EQ(cit.getStep(), babelwires::PathStep(4));
        ASSERT_NE(cit.getModifier(), nullptr);
        EXPECT_EQ(cit.getModifier()->getModifierData().m_pathToFeature, path2);
        cit.nextSibling();
        EXPECT_NE(cit, it.childrenEnd());
        EXPECT_EQ(cit.getStep(), babelwires::PathStep(6));
        EXPECT_EQ(cit.getModifier(), nullptr);
        cit.nextSibling();
        EXPECT_EQ(cit, it.childrenEnd());
    }
    it.nextSibling();
    EXPECT_NE(it, rit.childrenEnd());
    EXPECT_EQ(it.getStep(), babelwires::PathStep(babelwires::ShortId("cc")));
    ASSERT_EQ(it.getModifier(), nullptr);
    {
        auto cit = it.childrenBegin();
        EXPECT_NE(cit, it.childrenEnd());
        EXPECT_EQ(cit.getStep(), babelwires::PathStep(1));
        ASSERT_EQ(cit.getModifier(), nullptr);
        cit.nextSibling();
        EXPECT_NE(cit, it.childrenEnd());
        EXPECT_EQ(cit.getStep(), babelwires::PathStep(6));
        EXPECT_EQ(cit.getModifier(), nullptr);
        cit.nextSibling();
        EXPECT_EQ(cit, it.childrenEnd());
    }
    it.nextSibling();
    EXPECT_EQ(it, rit.childrenEnd());

    rit.nextSibling();
    EXPECT_EQ(rit, tree.end());
}

TEST(EditTreeTest, modifierIteration) {
    babelwires::EditTree tree;

    const babelwires::FeaturePath path1 = babelwires::FeaturePath::deserializeFromString("bb/4");
    const babelwires::FeaturePath path2 = babelwires::FeaturePath::deserializeFromString("bb/4");
    const babelwires::FeaturePath path3 = babelwires::FeaturePath::deserializeFromString("bb");
    const babelwires::FeaturePath path4 = babelwires::FeaturePath::deserializeFromString("cc/1");
    const babelwires::FeaturePath path5 = babelwires::FeaturePath::deserializeFromString("aa/3");
    const babelwires::FeaturePath path6 = babelwires::FeaturePath::deserializeFromString("cc/6");

    tree.setExpanded(path1, true);
    tree.addModifier(createModifier(path2, 2));
    tree.addModifier(createModifier(path3, 3));
    tree.setExpanded(path4, true);
    tree.addModifier(createModifier(path5, 5));
    tree.setExpanded(path6, true);

    auto range = tree.modifierRange();
    auto it = range.begin();
    EXPECT_NE(it, range.end());
    {
        babelwires::Modifier* mod = *it;
        ASSERT_TRUE(mod);
        EXPECT_EQ(mod->getModifierData().m_pathToFeature, path5);
    }
    ++it;
    EXPECT_NE(it, range.end());
    {
        babelwires::Modifier* mod = *it;
        ASSERT_TRUE(mod);
        EXPECT_EQ(mod->getModifierData().m_pathToFeature, path3);
    }
    ++it;
    EXPECT_NE(it, range.end());
    {
        babelwires::Modifier* mod = *it;
        ASSERT_TRUE(mod);
        EXPECT_EQ(mod->getModifierData().m_pathToFeature, path2);
    }
    ++it;
    EXPECT_EQ(it, range.end());
}

TEST(EditTreeTest, modifierIterationAtSubPath) {
    babelwires::EditTree tree;

    const babelwires::FeaturePath path1 = babelwires::FeaturePath::deserializeFromString("bb/4");
    const babelwires::FeaturePath path2 = babelwires::FeaturePath::deserializeFromString("bb/4");
    const babelwires::FeaturePath path3 = babelwires::FeaturePath::deserializeFromString("bb");
    const babelwires::FeaturePath path4 = babelwires::FeaturePath::deserializeFromString("cc/1");
    const babelwires::FeaturePath path5 = babelwires::FeaturePath::deserializeFromString("aa/3");
    const babelwires::FeaturePath path6 = babelwires::FeaturePath::deserializeFromString("cc/6");

    tree.setExpanded(path1, true);
    tree.addModifier(createModifier(path2, 2));
    tree.addModifier(createModifier(path3, 3));
    tree.setExpanded(path4, true);
    tree.addModifier(createModifier(path5, 5));
    tree.setExpanded(path6, true);

    auto range = tree.modifierRange(path3);
    auto it = range.begin();
    EXPECT_NE(it, range.end());
    {
        babelwires::Modifier* mod = *it;
        ASSERT_TRUE(mod);
        EXPECT_EQ(mod->getModifierData().m_pathToFeature, path3);
    }
    ++it;
    EXPECT_NE(it, range.end());
    {
        babelwires::Modifier* mod = *it;
        ASSERT_TRUE(mod);
        EXPECT_EQ(mod->getModifierData().m_pathToFeature, path2);
    }
    ++it;
    EXPECT_EQ(it, range.end());
}

TEST(EditTreeTest, filteredModifierIteration) {
    babelwires::EditTree tree;

    const babelwires::FeaturePath path1 = babelwires::FeaturePath::deserializeFromString("bb/4");
    const babelwires::FeaturePath path2 = babelwires::FeaturePath::deserializeFromString("bb/4");
    const babelwires::FeaturePath path3 = babelwires::FeaturePath::deserializeFromString("bb");
    const babelwires::FeaturePath path4 = babelwires::FeaturePath::deserializeFromString("cc/1");
    const babelwires::FeaturePath path5 = babelwires::FeaturePath::deserializeFromString("aa/3");
    const babelwires::FeaturePath path6 = babelwires::FeaturePath::deserializeFromString("cc/6");

    tree.setExpanded(path1, true);
    tree.addModifier(createConnectionModifier(path2));
    tree.addModifier(createModifier(path3, 3));
    tree.setExpanded(path4, true);
    tree.addModifier(createConnectionModifier(path5));
    tree.setExpanded(path6, true);

    auto range = tree.modifierRange<babelwires::ConnectionModifier>();
    auto it = range.begin();
    EXPECT_NE(it, range.end());
    {
        babelwires::Modifier* mod = *it;
        ASSERT_TRUE(mod);
        EXPECT_EQ(mod->getModifierData().m_pathToFeature, path5);
    }
    ++it;
    EXPECT_NE(it, range.end());
    {
        babelwires::Modifier* mod = *it;
        ASSERT_TRUE(mod);
        EXPECT_EQ(mod->getModifierData().m_pathToFeature, path2);
    }
    ++it;
    EXPECT_EQ(it, range.end());
}
