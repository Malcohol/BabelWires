#include <BaseLib/IO/fileDataSink.hpp>

#include <Tests/TestUtils/tempFilePath.hpp>

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

using namespace babelwires;

TEST(FileDataSinkTest, basics) {
    testUtils::TempFilePath path("foo.txt");

    const std::string testData = "Test data.\n Here is another line of test data.";

    {
        auto sinkResult = FileDataSink::open(path);
        ASSERT_TRUE(sinkResult.has_value());
        auto sink = std::move(*sinkResult);
        sink.stream() << testData;
        EXPECT_TRUE(sink.close());
    }

    {
        std::ifstream is = path.openForReading();
        std::string contents{std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>()};
        EXPECT_EQ(contents, testData);
    }
}

TEST(FileDataSinkTest, writeBytes) {
    testUtils::TempFilePath path("foo.txt");

    const std::string testData = "123456789";

    {
        auto sinkResult = FileDataSink::open(path);
        ASSERT_TRUE(sinkResult.has_value());
        auto sink = std::move(*sinkResult);
        sink.stream().write(testData.data(), testData.size());
        EXPECT_TRUE(sink.close());
    }

    {
        std::ifstream is = path.openForReading();
        std::string contents{std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>()};
        EXPECT_EQ(contents, testData);
    }
}

TEST(FileDataSinkTest, openFailure) {
    testUtils::TempFilePath path("foo.txt");

    std::filesystem::create_directory(path);
    auto sinkResult = FileDataSink::open(path);
    EXPECT_FALSE(sinkResult.has_value());
}

TEST(FileDataSinkTest, closeReportsWriteFailure) {
    testUtils::TempFilePath path("foo.txt");

    const std::string testData = "Test data.\n Here is another line of test data.";

    auto sinkResult = FileDataSink::open(path);
    ASSERT_TRUE(sinkResult.has_value());
    auto sink = std::move(*sinkResult);

    sink.stream().write(testData.data(), testData.size());
    sink.stream().setstate(std::ios_base::badbit);
    sink.stream().write(testData.data(), testData.size());

    auto closeResult = sink.close();
    EXPECT_FALSE(closeResult.has_value());
}
