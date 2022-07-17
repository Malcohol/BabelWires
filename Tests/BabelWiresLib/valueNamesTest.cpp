#include <gtest/gtest.h>

#include <BabelWiresLib/ValueNames/sparseValueNamesImpl.hpp>
#include <BabelWiresLib/ValueNames/contiguousValueNamesImpl.hpp>

TEST(ValueNames, sparseIteration) {
    babelwires::SparseValueNamesImpl names({ {4, "four"}, {8, "eight"}, {12, "twelve"}});

    auto it = names.begin();
    EXPECT_NE(it, names.end());
    EXPECT_EQ(std::get<0>(*it), 4);
    EXPECT_EQ(std::get<1>(*it), "four");
    ++it;

    EXPECT_NE(it, names.end());
    EXPECT_EQ(std::get<0>(*it), 8);
    EXPECT_EQ(std::get<1>(*it), "eight");
    ++it;

    EXPECT_NE(it, names.end());
    EXPECT_EQ(std::get<0>(*it), 12);
    EXPECT_EQ(std::get<1>(*it), "twelve");
    ++it;

    EXPECT_EQ(it, names.end());
}

TEST(ValueNames, sparseGetNameForValue) {
    babelwires::SparseValueNamesImpl names({ {4, "four"}, {8, "eight"}, {12, "twelve"}});

    std::string name;
    EXPECT_FALSE(names.getNameForValue(0, name));
    EXPECT_FALSE(names.getNameForValue(1, name));
    EXPECT_FALSE(names.getNameForValue(2, name));
    EXPECT_FALSE(names.getNameForValue(3, name));
    EXPECT_TRUE(names.getNameForValue(4, name));
    EXPECT_EQ(name, "four");

    EXPECT_FALSE(names.getNameForValue(5, name));
    EXPECT_FALSE(names.getNameForValue(6, name));
    EXPECT_FALSE(names.getNameForValue(7, name));
    EXPECT_TRUE(names.getNameForValue(8, name));
    EXPECT_EQ(name, "eight");

    EXPECT_FALSE(names.getNameForValue(9, name));
    EXPECT_FALSE(names.getNameForValue(10, name));
    EXPECT_FALSE(names.getNameForValue(11, name));
    EXPECT_TRUE(names.getNameForValue(12, name));
    EXPECT_EQ(name, "twelve");
    EXPECT_FALSE(names.getNameForValue(13, name));
}

TEST(ValueNames, sparseGetValueForName) {
    babelwires::SparseValueNamesImpl names({ {4, "four"}, {8, "eight"}, {12, "twelve"}});

    int value = 0;
    EXPECT_FALSE(names.getValueForName("two", value));
    EXPECT_TRUE(names.getValueForName("four", value));
    EXPECT_EQ(value, 4);
    EXPECT_TRUE(names.getValueForName("eight", value));
    EXPECT_EQ(value, 8);
    EXPECT_TRUE(names.getValueForName("twelve", value));
    EXPECT_EQ(value, 12);
}

TEST(ValueNames, contiguousIteration) {
    babelwires::ContiguousValueNamesImpl names({ "four", "five", "six"}, 4);

    auto it = names.begin();
    EXPECT_NE(it, names.end());
    EXPECT_EQ(std::get<0>(*it), 4);
    EXPECT_EQ(std::get<1>(*it), "four");
    ++it;

    EXPECT_NE(it, names.end());
    EXPECT_EQ(std::get<0>(*it), 5);
    EXPECT_EQ(std::get<1>(*it), "five");
    ++it;

    EXPECT_NE(it, names.end());
    EXPECT_EQ(std::get<0>(*it), 6);
    EXPECT_EQ(std::get<1>(*it), "six");
    ++it;

    EXPECT_EQ(it, names.end());
}

TEST(ValueNames, contiguousGetNameForValue) {
    babelwires::ContiguousValueNamesImpl names({ "four", "five", "six"}, 4);

    std::string name;
    EXPECT_FALSE(names.getNameForValue(0, name));
    EXPECT_FALSE(names.getNameForValue(1, name));
    EXPECT_FALSE(names.getNameForValue(2, name));
    EXPECT_FALSE(names.getNameForValue(3, name));
    EXPECT_TRUE(names.getNameForValue(4, name));
    EXPECT_EQ(name, "four");
    EXPECT_TRUE(names.getNameForValue(5, name));
    EXPECT_EQ(name, "five");
    EXPECT_TRUE(names.getNameForValue(6, name));
    EXPECT_EQ(name, "six");
    EXPECT_FALSE(names.getNameForValue(7, name));
}

TEST(ValueNames, contiguousGetValueForName) {
    babelwires::ContiguousValueNamesImpl names({ "four", "five", "six"}, 4);

    int value = 0;
    EXPECT_FALSE(names.getValueForName("two", value));
    EXPECT_TRUE(names.getValueForName("four", value));
    EXPECT_EQ(value, 4);
    EXPECT_TRUE(names.getValueForName("five", value));
    EXPECT_EQ(value, 5);
    EXPECT_TRUE(names.getValueForName("six", value));
    EXPECT_EQ(value, 6);
}