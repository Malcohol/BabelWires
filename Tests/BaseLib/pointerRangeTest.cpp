#include <BaseLib/Utilities/pointerRange.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <vector>

namespace {
    struct Foo {
        Foo(int i)
            : m_int(i) {}
        int m_int;
    };
} // namespace

TEST(PointerRange, vectorNonConst) {
    std::vector<std::unique_ptr<Foo>> vecfoo;

    {
        babelwires::PointerRange empty(vecfoo);
        EXPECT_EQ(empty.begin(), empty.end());
    }
    vecfoo.emplace_back(std::make_unique<Foo>(2));
    {
        babelwires::PointerRange range(vecfoo);
        auto it = range.begin();
        EXPECT_NE(it, range.end());
        EXPECT_EQ(*it, vecfoo[0].get());
        it->m_int = 20;
        EXPECT_EQ(vecfoo[0].get()->m_int, 20);
        ++it;
        EXPECT_NE(it, range.begin());
        EXPECT_EQ(it, range.end());
    }
    vecfoo.emplace_back(std::make_unique<Foo>(8));
    {
        babelwires::PointerRange range(vecfoo);
        auto it = range.begin();
        EXPECT_NE(it, range.end());
        ++it;
        EXPECT_NE(it, range.begin());
        EXPECT_NE(it, range.end());
        ++it;
        EXPECT_EQ(it, range.end());
    }
}

TEST(PointerRange, vectorConst) {
    std::vector<std::unique_ptr<const Foo>> vecfoo;

    {
        babelwires::PointerRange empty(vecfoo);
        EXPECT_EQ(empty.begin(), empty.end());
    }
    vecfoo.emplace_back(std::make_unique<Foo>(2));
    {
        babelwires::PointerRange range(vecfoo);
        auto it = range.begin();
        EXPECT_NE(it, range.end());
        EXPECT_EQ(*it, vecfoo[0].get());
        ++it;
        EXPECT_NE(it, range.begin());
        EXPECT_EQ(it, range.end());
    }
    vecfoo.emplace_back(std::make_unique<Foo>(8));
    {
        babelwires::PointerRange range(vecfoo);
        auto it = range.begin();
        EXPECT_NE(it, range.end());
        ++it;
        EXPECT_NE(it, range.begin());
        EXPECT_NE(it, range.end());
        ++it;
        EXPECT_EQ(it, range.end());
    }
}
