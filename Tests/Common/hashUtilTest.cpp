#include <Common/Utilities/hash.hpp>

#include <gtest/gtest.h>

TEST(Hash, mixInto) {
    // In theory, a tests might fail due to a hash collisions, but it's very unlikely.

    std::string_view foo = "Foo";
    std::string_view bar = "Bar";
    std::string_view foobar = "FooBar";

    std::size_t h0 = 0xABCD;
    babelwires::hash::mixInto(h0, foo);
    EXPECT_NE(h0, 0xABCD);
    babelwires::hash::mixInto(h0, bar);
    babelwires::hash::mixInto(h0, 18);

    std::size_t h1 = 0xABCD;
    babelwires::hash::mixInto(h1, foo, bar, 18);
    EXPECT_EQ(h0, h1);
}

TEST(Hash, mixtureOf) {
    // In theory, a tests might fail due to a hash collisions, but it's very unlikely.

    std::string_view foo = "Foo";
    std::string_view bar = "Bar";
    std::string_view foobar = "FooBar";

    EXPECT_NE(babelwires::hash::mixtureOf(foo, bar), babelwires::hash::mixtureOf(foobar));
    EXPECT_NE(babelwires::hash::mixtureOf(foo, bar), babelwires::hash::mixtureOf(bar, foo));

    EXPECT_NE(babelwires::hash::mixtureOf(0, 0), babelwires::hash::mixtureOf(0));
    EXPECT_NE(babelwires::hash::mixtureOf(0, 0, 0), babelwires::hash::mixtureOf(0, 0));
    EXPECT_NE(babelwires::hash::mixtureOf(0, 0, 0, 0), babelwires::hash::mixtureOf(0, 0, 0));

    EXPECT_NE(babelwires::hash::mixtureOf(foo, bar), babelwires::hash::mixtureOf(bar, foo));
    EXPECT_NE(babelwires::hash::mixtureOf(foo, 0), babelwires::hash::mixtureOf(0, foo));
}
