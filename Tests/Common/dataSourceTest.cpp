#include <Common/IO/fileDataSource.hpp>

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
        try {
            FileDataSource foo(fooPath);
            EXPECT_EQ(foo.peekNextByte(), 'a');
            for (int i = 0; i < 26; ++i) {
                EXPECT_EQ(foo.getAbsolutePosition(), i - 1);
                EXPECT_EQ(foo.getNextByte(), testContents[i]);
            }
            EXPECT_TRUE(foo.isEof());
        } catch (const std::exception&) {
            EXPECT_TRUE(false);
        }
    }
}

TEST(DataSource, peekNextByte) {
    testUtils::TempFilePath fooPath("foo.txt");
    {
        std::ofstream foo = fooPath.openForWriting(std::ios_base::binary);
        foo << testContents;
    }

    {
        try {
            FileDataSource foo(fooPath);
            for (int i = 0; i < 26; ++i) {
                EXPECT_EQ(foo.getAbsolutePosition(), i - 1);
                EXPECT_EQ(foo.peekNextByte(), testContents[i]);
                EXPECT_EQ(foo.getAbsolutePosition(), i - 1);
                EXPECT_EQ(foo.peekNextByte(), testContents[i]);
                EXPECT_EQ(foo.getAbsolutePosition(), i - 1);
                EXPECT_EQ(foo.getNextByte(), testContents[i]);
                EXPECT_EQ(foo.getAbsolutePosition(), i);
            }
            EXPECT_TRUE(foo.isEof());
        } catch (const std::exception&) {
            EXPECT_TRUE(false);
        }
    }
}

TEST(DataSource, rewind) {
    testUtils::TempFilePath fooPath("foo.txt");
    {
        std::ofstream foo = fooPath.openForWriting(std::ios_base::binary);
        foo << testContents;
    }

    {
        try {
            FileDataSource foo(fooPath);
            EXPECT_EQ(foo.peekNextByte(), 'a');
            foo.setRewindPoint(10);
            for (int i = 0; i < 5; ++i) {
                EXPECT_EQ(foo.getNextByte(), testContents[i]);
                EXPECT_EQ(foo.getAbsolutePosition(), i);
            }
            foo.rewind();
            for (int i = 0; i < 10; ++i) {
                EXPECT_EQ(foo.getNextByte(), testContents[i]);
                EXPECT_EQ(foo.getAbsolutePosition(), i);
            }
            foo.rewind();
            for (int i = 0; i < 5; ++i) {
                EXPECT_EQ(foo.getNextByte(), testContents[i]);
                EXPECT_EQ(foo.getAbsolutePosition(), i);
            }
            foo.setRewindPoint(10);
            for (int i = 0; i < 10; ++i) {
                EXPECT_EQ(foo.getNextByte(), testContents[5 + i]);
                EXPECT_EQ(foo.getAbsolutePosition(), 5 + i);
            }
            foo.rewind();
            foo.setRewindPoint(5);
            for (int i = 0; i < 5; ++i) {
                EXPECT_EQ(foo.getNextByte(), testContents[5 + i]);
                EXPECT_EQ(foo.getAbsolutePosition(), 5 + i);
            }
            foo.rewind();
            EXPECT_EQ(foo.getNextByte(), testContents[5]);
            foo.setRewindPoint(3);
            for (int i = 0; i < 3; ++i) {
                EXPECT_EQ(foo.getNextByte(), testContents[6 + i]);
                EXPECT_EQ(foo.getAbsolutePosition(), 6 + i);
            }
            foo.rewind();
            foo.setRewindPoint(20);
            for (int i = 0; i < 20; ++i) {
                EXPECT_EQ(foo.getNextByte(), testContents[6 + i]);
                EXPECT_EQ(foo.getAbsolutePosition(), 6 + i);
            }
            EXPECT_TRUE(foo.isEof());
            foo.rewind();
            for (int i = 0; i < 20; ++i) {
                EXPECT_EQ(foo.getNextByte(), testContents[6 + i]);
                EXPECT_EQ(foo.getAbsolutePosition(), 6 + i);
            }
            EXPECT_TRUE(foo.isEof());
        } catch (const std::exception&) {
            EXPECT_TRUE(false);
        }
    }
}

TEST(DataSource, peekAndRewind) {
    testUtils::TempFilePath fooPath("foo.txt");
    {
        std::ofstream foo = fooPath.openForWriting(std::ios_base::binary);
        foo << testContents;
    }

    {
        try {
            FileDataSource foo(fooPath);
            EXPECT_EQ(foo.peekNextByte(), 'a');
            for (int i = 0; i < 10; ++i) {
                EXPECT_EQ(foo.getNextByte(), testContents[i]);
                EXPECT_EQ(foo.getAbsolutePosition(), i);
            }
            foo.setRewindPoint(10);
            EXPECT_EQ(foo.peekNextByte(), 'k');
            for (int i = 0; i < 10; ++i) {
                EXPECT_EQ(foo.getNextByte(), testContents[10 + i]);
                EXPECT_EQ(foo.getAbsolutePosition(), 10 + i);
            }
            foo.rewind();
            EXPECT_EQ(foo.peekNextByte(), 'k');
            for (int i = 0; i < 6; ++i) {
                EXPECT_EQ(foo.getNextByte(), testContents[10 + i]);
                EXPECT_EQ(foo.getAbsolutePosition(), 10 + i);
            }
            EXPECT_EQ(foo.peekNextByte(), 'q');
            for (int i = 0; i < 10; ++i) {
                EXPECT_EQ(foo.getNextByte(), testContents[16 + i]);
                EXPECT_EQ(foo.getAbsolutePosition(), 16 + i);
            }
            EXPECT_TRUE(foo.isEof());
        } catch (const std::exception&) {
            EXPECT_TRUE(false);
        }
    }
}