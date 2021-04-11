#include <gtest/gtest.h>

#include "BabelWires/ValueNames/valueNames.hpp"

namespace {
    struct TestValueNames : babelwires::ValueNames {
        virtual int getFirstValue() const override { return 4; }
        virtual bool getNextValueWithName(int& value) const override {
            switch (value) {
                case 4:
                case 8:
                    value += 4;
                    return true;
                default:
                    return false;
            }
        }
        virtual bool doGetValueForName(const std::string& name, int& valueOut) const override {
            if (name == "four") {
                valueOut = 4;
                return true;
            } else if (name == "eight") {
                valueOut = 8;
                return true;
            } else if (name == "twelve") {
                valueOut = 12;
                return true;
            }
            return false;
        }

        virtual bool doGetNameForValue(int value, std::string& nameOut) const override {
            switch (value) {
                case 4:
                    nameOut = "four";
                    return true;
                case 8:
                    nameOut = "eight";
                    return true;
                case 12:
                    nameOut = "twelve";
                    return true;
                default:
                    return false;
            }
        }
    };
} // namespace

// This doesn't really test the library, so much as exercise the API.
TEST(ValueNames, valueNames) {
    TestValueNames names;
    int value = names.getFirstValue();
    std::string name;
    EXPECT_EQ(value, 4);
    EXPECT_TRUE(names.getNameForValue(value, name));
    EXPECT_EQ(name, "four");
    EXPECT_TRUE(names.getNextValueWithName(value));
    EXPECT_EQ(value, 8);
    EXPECT_TRUE(names.getNameForValue(value, name));
    EXPECT_EQ(name, "eight");
    EXPECT_TRUE(names.getNextValueWithName(value));
    EXPECT_EQ(value, 12);
    EXPECT_TRUE(names.getNameForValue(value, name));
    EXPECT_EQ(name, "twelve");
    EXPECT_FALSE(names.getNextValueWithName(value));

    EXPECT_FALSE(names.getNameForValue(3, name));
    EXPECT_FALSE(names.getNameForValue(5, name));
    EXPECT_FALSE(names.getNameForValue(20, name));

    EXPECT_TRUE(names.getValueForName("four", value));
    EXPECT_EQ(value, 4);
    EXPECT_TRUE(names.getValueForName("eight", value));
    EXPECT_EQ(value, 8);
    EXPECT_TRUE(names.getValueForName("twelve", value));
    EXPECT_EQ(value, 12);
}
