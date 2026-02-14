#include <BaseLib/IO/fileDataSource.hpp>

#include <Tests/TestUtils/tempFilePath.hpp>

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

using namespace babelwires;

namespace {
    constexpr char testContents[] = "abcdefghijklmnopqrstuvwxyz";
}

TEST(DataSource, getNextByte) {
    testUtils::TempFilePath fooPath("foo.txt");
    {
        std::ofstream foo(fooPath.m_asString, std::ios_base::binary);
        foo << testContents;
    }

    {
        auto fooResult = FileDataSource::open(fooPath);
        ASSERT_TRUE(fooResult.has_value());
        auto foo = std::move(*fooResult);
        auto peekResult = foo.peekNextByte();
        ASSERT_TRUE(peekResult.has_value());
        EXPECT_EQ(*peekResult, 'a');
        for (int i = 0; i < 26; ++i) {
            EXPECT_EQ(foo.getAbsolutePosition(), i - 1);
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[i]);
        }
        EXPECT_TRUE(foo.isEof());
    }
}

TEST(DataSource, peekNextByte) {
    testUtils::TempFilePath fooPath("foo.txt");
    {
        std::ofstream foo = fooPath.openForWriting(std::ios_base::binary);
        foo << testContents;
    }

    {
        auto fooResult = FileDataSource::open(fooPath);
        ASSERT_TRUE(fooResult.has_value());
        auto foo = std::move(*fooResult);
        for (int i = 0; i < 26; ++i) {
            EXPECT_EQ(foo.getAbsolutePosition(), i - 1);
            auto peekResult1 = foo.peekNextByte();
            ASSERT_TRUE(peekResult1.has_value());
            EXPECT_EQ(*peekResult1, testContents[i]);
            EXPECT_EQ(foo.getAbsolutePosition(), i - 1);
            auto peekResult2 = foo.peekNextByte();
            ASSERT_TRUE(peekResult2.has_value());
            EXPECT_EQ(*peekResult2, testContents[i]);
            EXPECT_EQ(foo.getAbsolutePosition(), i - 1);
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[i]);
            EXPECT_EQ(foo.getAbsolutePosition(), i);
        }
        EXPECT_TRUE(foo.isEof());
    }
}

TEST(DataSource, rewind) {
    testUtils::TempFilePath fooPath("foo.txt");
    {
        std::ofstream foo = fooPath.openForWriting(std::ios_base::binary);
        foo << testContents;
    }

    {
        auto fooResult = FileDataSource::open(fooPath);
        ASSERT_TRUE(fooResult.has_value());
        auto foo = std::move(*fooResult);
        auto peekResult = foo.peekNextByte();
        ASSERT_TRUE(peekResult.has_value());
        EXPECT_EQ(*peekResult, 'a');
        auto rwResult = foo.setRewindPoint(10);
        ASSERT_TRUE(rwResult.has_value());
        for (int i = 0; i < 5; ++i) {
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[i]);
            EXPECT_EQ(foo.getAbsolutePosition(), i);
        }
        foo.rewind();
        for (int i = 0; i < 10; ++i) {
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[i]);
            EXPECT_EQ(foo.getAbsolutePosition(), i);
        }
        foo.rewind();
        for (int i = 0; i < 5; ++i) {
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[i]);
            EXPECT_EQ(foo.getAbsolutePosition(), i);
        }
        rwResult = foo.setRewindPoint(10);
        ASSERT_TRUE(rwResult.has_value());
        for (int i = 0; i < 10; ++i) {
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[5 + i]);
            EXPECT_EQ(foo.getAbsolutePosition(), 5 + i);
        }
        foo.rewind();
        rwResult = foo.setRewindPoint(5);
        ASSERT_TRUE(rwResult.has_value());
        for (int i = 0; i < 5; ++i) {
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[5 + i]);
            EXPECT_EQ(foo.getAbsolutePosition(), 5 + i);
        }
        foo.rewind();
        {
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[5]);
        }
        rwResult = foo.setRewindPoint(3);
        ASSERT_TRUE(rwResult.has_value());
        for (int i = 0; i < 3; ++i) {
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[6 + i]);
            EXPECT_EQ(foo.getAbsolutePosition(), 6 + i);
        }
        foo.rewind();
        rwResult = foo.setRewindPoint(20);
        ASSERT_TRUE(rwResult.has_value());
        for (int i = 0; i < 20; ++i) {
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[6 + i]);
            EXPECT_EQ(foo.getAbsolutePosition(), 6 + i);
        }
        EXPECT_TRUE(foo.isEof());
        foo.rewind();
        for (int i = 0; i < 20; ++i) {
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[6 + i]);
            EXPECT_EQ(foo.getAbsolutePosition(), 6 + i);
        }
        EXPECT_TRUE(foo.isEof());
    }
}

TEST(DataSource, peekAndRewind) {
    testUtils::TempFilePath fooPath("foo.txt");
    {
        std::ofstream foo = fooPath.openForWriting(std::ios_base::binary);
        foo << testContents;
    }

    {
        auto fooResult = FileDataSource::open(fooPath);
        ASSERT_TRUE(fooResult.has_value());
        auto foo = std::move(*fooResult);
        auto peekResult = foo.peekNextByte();
        ASSERT_TRUE(peekResult.has_value());
        EXPECT_EQ(*peekResult, 'a');
        for (int i = 0; i < 10; ++i) {
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[i]);
            EXPECT_EQ(foo.getAbsolutePosition(), i);
        }
        auto rwResult = foo.setRewindPoint(10);
        ASSERT_TRUE(rwResult.has_value());
        peekResult = foo.peekNextByte();
        ASSERT_TRUE(peekResult.has_value());
        EXPECT_EQ(*peekResult, 'k');
        for (int i = 0; i < 10; ++i) {
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[10 + i]);
            EXPECT_EQ(foo.getAbsolutePosition(), 10 + i);
        }
        foo.rewind();
        peekResult = foo.peekNextByte();
        ASSERT_TRUE(peekResult.has_value());
        EXPECT_EQ(*peekResult, 'k');
        for (int i = 0; i < 6; ++i) {
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[10 + i]);
            EXPECT_EQ(foo.getAbsolutePosition(), 10 + i);
        }
        peekResult = foo.peekNextByte();
        ASSERT_TRUE(peekResult.has_value());
        EXPECT_EQ(*peekResult, 'q');
        for (int i = 0; i < 10; ++i) {
            auto byteResult = foo.getNextByte();
            ASSERT_TRUE(byteResult.has_value());
            EXPECT_EQ(*byteResult, testContents[16 + i]);
            EXPECT_EQ(foo.getAbsolutePosition(), 16 + i);
        }
        EXPECT_TRUE(foo.isEof());
    }
}