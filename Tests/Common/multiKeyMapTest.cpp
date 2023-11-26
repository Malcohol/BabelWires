#include <Common/multiKeyMap.hpp>

#include <gtest/gtest.h>

#include <string>
#include <string_view>
#include <array>

#include <Tests/TestUtils/equalSets.hpp>

using namespace babelwires;

namespace {
    constexpr int numEntries = 6;

    template <typename T> struct TestData {};

    template <> struct TestData<std::string> {
        static constexpr std::array<const char*, numEntries> values = {"Hello",   "Goodbye",   "",
                                                                       "Aadvark", "Mongooses", "Zebras"};
        static constexpr const char* notAValue = "Ouch";
    };

    template <> struct TestData<int> {
        static constexpr std::array<int, numEntries> values = {14, -2, 0, 1333, 87, 99};
        static constexpr int notAValue = 43;
    };

    template <> struct TestData<float> {
        // In practise, floating point numbers make poor keys.
        static constexpr std::array<float, numEntries> values = {0.f, 51.9f, -0.4f, -44.0f, 1.2e12f, 18.0f};
        static constexpr float notAValue = 17.3f;
    };

    struct Test0Types {
        using Key0Type = int;
        using Key1Type = int;
        using ValueType = int;
    };

    struct Test1Types {
        using Key0Type = std::string;
        using Key1Type = int;
        using ValueType = float;
    };

    struct Test2Types {
        using Key0Type = float;
        using Key1Type = std::string;
        using ValueType = int;
    };

    struct Test3Types {
        using Key0Type = int;
        using Key1Type = float;
        using ValueType = std::string;
    };
} // namespace

template <typename TUPLE> class MultiKeyMapTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(MultiKeyMapTest);

TYPED_TEST_P(MultiKeyMapTest, basicOperations) {
    using Key0Type = typename TypeParam::Key0Type;
    using Key1Type = typename TypeParam::Key1Type;
    using ValueType = typename TypeParam::ValueType;
    auto& keys0 = TestData<Key0Type>::values;
    auto& keys1 = TestData<Key1Type>::values;
    auto& values = TestData<ValueType>::values;
    auto& notAValue = TestData<ValueType>::notAValue;

    MultiKeyMap<Key0Type, Key1Type, ValueType> map;
    EXPECT_EQ(map.find0(keys0[0]), map.end());
    EXPECT_EQ(map.find1(keys1[0]), map.end());

    for (int i = 0; i < numEntries; ++i) {
        // We'll update the value to values[i] below.
        EXPECT_TRUE(map.insert_or_assign(keys0[i], keys1[i], notAValue));
        for (int j = 0; j < numEntries; ++j) {
            if (j <= i) {
                auto it0 = map.find0(keys0[j]);
                EXPECT_NE(it0, map.end());
                EXPECT_EQ(it0.getKey0(), keys0[j]);
                EXPECT_EQ(it0.getKey1(), keys1[j]);
                if (i == j) {
                    EXPECT_EQ(it0.getValue(), notAValue);
                    // Update the value
                    EXPECT_FALSE(map.insert_or_assign(keys0[i], keys1[i], values[i]));
                }
                EXPECT_EQ(it0.getValue(), values[j]);
                auto it1 = map.find1(keys1[j]);
                EXPECT_NE(it1, map.end());
                EXPECT_EQ(it1.getKey0(), keys0[j]);
                EXPECT_EQ(it1.getKey1(), keys1[j]);
                EXPECT_EQ(it1.getValue(), values[j]);
            } else {
                EXPECT_EQ(map.find0(keys0[j]), map.end());
                EXPECT_EQ(map.find1(keys1[j]), map.end());
            }
        }
    }

    for (int i = 0; i < numEntries; ++i) {
        switch (i % 3) {
            case 0: {
                EXPECT_TRUE(map.erase0(keys0[i]));
                break;
            }
            case 1: {
                EXPECT_TRUE(map.erase1(keys1[i]));
                break;
            }
            case 2: {
                EXPECT_TRUE(map.erase(map.find0(keys0[i])));
                break;
            }
        }
        for (int j = 0; j < numEntries; ++j) {
            if (j > i) {
                auto it0 = map.find0(keys0[j]);
                EXPECT_NE(it0, map.end());
                EXPECT_EQ(it0.getKey0(), keys0[j]);
                EXPECT_EQ(it0.getKey1(), keys1[j]);
                EXPECT_EQ(it0.getValue(), values[j]);
                auto it1 = map.find1(keys1[j]);
                EXPECT_NE(it1, map.end());
                EXPECT_EQ(it1.getKey0(), keys0[j]);
                EXPECT_EQ(it1.getKey1(), keys1[j]);
                EXPECT_EQ(it1.getValue(), values[j]);
            } else {
                EXPECT_EQ(map.find0(keys0[j]), map.end());
                EXPECT_EQ(map.find1(keys1[j]), map.end());
            }
        }
    }
}

TYPED_TEST_P(MultiKeyMapTest, iteration) {
    using Key0Type = typename TypeParam::Key0Type;
    using Key1Type = typename TypeParam::Key1Type;
    using ValueType = typename TypeParam::ValueType;
    auto& keys0 = TestData<Key0Type>::values;
    auto& keys1 = TestData<Key1Type>::values;
    auto& values = TestData<ValueType>::values;
    auto& notAValue = TestData<ValueType>::notAValue;

    MultiKeyMap<Key0Type, Key1Type, ValueType> map;

    EXPECT_EQ(map.begin(), map.end());

    for (const auto& it : map) {
        EXPECT_TRUE(false);    
    }

    using Triple = std::tuple<Key0Type, Key1Type, ValueType>;
    std::vector<Triple> source;
    for (int i = 0; i < keys0.size(); ++i) {
        map.insert_or_assign(keys0[i], keys1[i], values[i]);
        source.emplace_back(Triple{keys0[i], keys1[i], values[i]});
    }

    std::vector<std::tuple<Key0Type, Key1Type, ValueType>> result;

    for (const auto& it : map) {
        result.emplace_back(Triple{it.getKey0(), it.getKey1(), it.getValue()});
    }

    EXPECT_TRUE(testUtils::areEqualSets(source, result));
}

REGISTER_TYPED_TEST_SUITE_P(MultiKeyMapTest, basicOperations, iteration);

typedef ::testing::Types<Test0Types, Test1Types, Test2Types, Test3Types> TestTypes;
INSTANTIATE_TYPED_TEST_SUITE_P(MultiKeyMapTest, MultiKeyMapTest, TestTypes);