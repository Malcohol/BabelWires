#include <gtest/gtest.h>
#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/subtypeUtils.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

namespace {
    const std::array<babelwires::SubtypeOrder, 5> subtypeOrderElements = {
        babelwires::SubtypeOrder::IsEquivalent, babelwires::SubtypeOrder::IsSubtype,
        babelwires::SubtypeOrder::IsSupertype, babelwires::SubtypeOrder::IsIntersecting,
        babelwires::SubtypeOrder::IsDisjoint};
}

TEST(TypeSystemTest, subtypeProductSymmetric) {
    for (auto a : subtypeOrderElements) {
        for (auto b : subtypeOrderElements) {
            const babelwires::SubtypeOrder ab = babelwires::subtypeProduct(a, b);
            const babelwires::SubtypeOrder ba = babelwires::subtypeProduct(b, a);
            EXPECT_EQ(ab, ba);
        }
    }
}

TEST(TypeSystemTest, subtypeProductAssociative) {
    for (auto a : subtypeOrderElements) {
        for (auto b : subtypeOrderElements) {
            for (auto c : subtypeOrderElements) {
                const babelwires::SubtypeOrder ab_c =
                    babelwires::subtypeProduct(babelwires::subtypeProduct(a, b), c);
                const babelwires::SubtypeOrder a_bc =
                    babelwires::subtypeProduct(a, babelwires::subtypeProduct(c, b));
                EXPECT_EQ(ab_c, a_bc);
            }
        }
    }
}

TEST(TypeSystemTest, subtypeFromRanges) {
    const babelwires::Range<int> range04(0, 4);
    const babelwires::Range<int> range03(0, 3);
    const babelwires::Range<int> range13(1, 3);
    const babelwires::Range<int> range14(1, 4);
    const babelwires::Range<int> range48(4, 8);
    const babelwires::Range<int> range59(5, 9);
    
    EXPECT_EQ(babelwires::subtypeFromRanges(range13, range13), babelwires::SubtypeOrder::IsEquivalent);

    EXPECT_EQ(babelwires::subtypeFromRanges(range13, range04), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(babelwires::subtypeFromRanges(range04, range13), babelwires::SubtypeOrder::IsSupertype);

    EXPECT_EQ(babelwires::subtypeFromRanges(range03, range04), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(babelwires::subtypeFromRanges(range04, range03), babelwires::SubtypeOrder::IsSupertype);

    EXPECT_EQ(babelwires::subtypeFromRanges(range13, range14), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(babelwires::subtypeFromRanges(range14, range13), babelwires::SubtypeOrder::IsSupertype);

    EXPECT_EQ(babelwires::subtypeFromRanges(range04, range48), babelwires::SubtypeOrder::IsIntersecting);
    EXPECT_EQ(babelwires::subtypeFromRanges(range48, range04), babelwires::SubtypeOrder::IsIntersecting);

    EXPECT_EQ(babelwires::subtypeFromRanges(range04, range59), babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(babelwires::subtypeFromRanges(range59, range04), babelwires::SubtypeOrder::IsDisjoint);
}

