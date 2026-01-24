#include <gtest/gtest.h>

#include <BaseLib/IO/outFileStream.hpp>

#include <Tests/TestUtils/tempFilePath.hpp>
#include <string>

TEST(OutFileStreamTest, basics) {
    testUtils::TempFilePath path("foo.txt");

    std::string testData = "Test data.\n Here is another line of test data.";

    {
        babelwires::OutFileStream os(path);
        os << testData;
        os.close();
    }
    {
        std::ifstream is = path.openForReading();
        std::string contents{std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>()};
        EXPECT_EQ(contents, testData);
    }
}

TEST(OutFileStreamTest, openFailure) {
    testUtils::TempFilePath path("foo.txt");

    std::filesystem::create_directory(path);
    // The exception means the assert in the destructor doesn't fire.
    EXPECT_THROW((babelwires::OutFileStream(path)), std::ios_base::failure);
}
