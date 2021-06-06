#include <gtest/gtest.h>

#include "BabelWires/FileFormat/filePath.hpp"
#include "Common/exceptions.hpp"

#include "Tests/TestUtils/testLog.hpp"

#include <fstream>

TEST(FilePathTest, serializeToString) {
    EXPECT_EQ(babelwires::FilePath().serializeToString(), "");
    EXPECT_EQ(babelwires::FilePath("Foo").serializeToString(), "Foo");
    EXPECT_EQ(babelwires::FilePath("Foo/Bar.boo").serializeToString(), "Foo/Bar.boo");
    EXPECT_EQ(babelwires::FilePath("Foo/Bar/Bar.boo").serializeToString(), "Foo/Bar/Bar.boo");
    EXPECT_EQ(babelwires::FilePath("/Foo/Bar.boo").serializeToString(), "/Foo/Bar.boo");
    EXPECT_EQ(babelwires::FilePath("c:/Foo/Bar.boo").serializeToString(), "c:/Foo/Bar.boo");
}

TEST(FilePathTest, deserializeFromString) {
    EXPECT_EQ(babelwires::FilePath::deserializeFromString("Foo"), "Foo");
    EXPECT_EQ(babelwires::FilePath::deserializeFromString("Foo/Bar.boo"), "Foo/Bar.boo");
    EXPECT_EQ(babelwires::FilePath::deserializeFromString("Foo/Bar/Bar.boo"), "Foo/Bar/Bar.boo");
    EXPECT_EQ(babelwires::FilePath::deserializeFromString("/Foo/Bar.boo"), "/Foo/Bar.boo");
    EXPECT_EQ(babelwires::FilePath::deserializeFromString("c:/Foo/Bar.boo"), "c:/Foo/Bar.boo");

    EXPECT_THROW(babelwires::FilePath::deserializeFromString(""), babelwires::ParseException);
    EXPECT_THROW(babelwires::FilePath::deserializeFromString("\t"), babelwires::ParseException);
    EXPECT_THROW(babelwires::FilePath::deserializeFromString("\n"), babelwires::ParseException);
}

TEST(FilePathTest, interpretRelativeTo) {
    {
        babelwires::FilePath path("/Foo/Bar.boo");
        path.interpretRelativeTo("/Foo");
        EXPECT_EQ(path, "Bar.boo");
    }
    {
        babelwires::FilePath path("/Foo/Bar/Bar.boo");
        path.interpretRelativeTo("/Foo/Boo");
        EXPECT_EQ(path, "../Bar/Bar.boo");
    }
    {
        babelwires::FilePath path("/Foo/Bar.boo");
        path.interpretRelativeTo("/Flerm");
        EXPECT_EQ(path, "../Foo/Bar.boo");
    }
#if defined(_WIN64) || defined(_WIN32)
    // This doesn't have to work on non-windows platforms, since the functionality
    // is only needed when exporting.
    // I assume this is the behaviour on Windows, but actually it's untested.
    {
        babelwires::FilePath path("c:/Foo/Bar.boo");
        path.interpretRelativeTo("d:/Flerm");
        EXPECT_EQ(path, "c:/Foo/Bar.boo");
    }
#endif
}

TEST(FilePathTest, resolveRelativeTo) {
    testUtils::TestLogWithListener log;

    std::filesystem::path tmp = std::filesystem::temp_directory_path();

    std::filesystem::create_directories(tmp / "Foo/Bar");    
    std::filesystem::path fileWhichExists(tmp / "Foo/Bar/Bar.boo");

    std::filesystem::create_directories(tmp / "Oom/Bar");    
    std::filesystem::path fileWhichExists2(tmp / "Oom/Bar/Bar.boo");

    // Ensure the file2 exists.
    {
        std::ofstream fs(fileWhichExists, std::ofstream::out);
        fs << "Test";
        fs.close();
        ASSERT_TRUE(fs.good());
    }
    {
        std::ofstream fs(fileWhichExists2, std::ofstream::out);
        fs << "Test";
        fs.close();
        ASSERT_TRUE(fs.good());
    }

    // Same context.
    {
        babelwires::FilePath path("Bar.boo");
        path.resolveRelativeTo(tmp / "Foo/Bar", tmp / "Foo/Bar", log);
        EXPECT_EQ(path, tmp / "Foo/Bar/Bar.boo");
    }
    // Same context.
    {
        babelwires::FilePath path("Bar/Bar.boo");
        path.resolveRelativeTo(tmp / "Foo", tmp / "Foo", log);
        EXPECT_EQ(path, tmp / "Foo/Bar/Bar.boo");
    }
    // Same context, file doesn't exist.
    {
        babelwires::FilePath path("Bar/Bar.boo");
        path.resolveRelativeTo(tmp / "Flerm", tmp / "Flerm", log);
        EXPECT_EQ(path, tmp / "Flerm/Bar/Bar.boo");
    }
    // New exists, old doesn't.
    {
        babelwires::FilePath path("Bar/Bar.boo");
        path.resolveRelativeTo(tmp / "Foo", tmp / "Flerm", log);
        EXPECT_EQ(path, tmp / "Foo/Bar/Bar.boo");
    }
    // Old exists, new doesn't.
    {
        babelwires::FilePath path("Bar/Bar.boo");
        path.resolveRelativeTo(tmp / "Flerm", tmp / "Foo", log);
        EXPECT_EQ(path, tmp / "Foo/Bar/Bar.boo");
    }
    // Both exist. New prefered. Check log.
    {
        log.clear();
        babelwires::FilePath path("Bar/Bar.boo");
        path.resolveRelativeTo(tmp / "Foo", tmp / "Oom", log);
        EXPECT_EQ(path, tmp / "Foo/Bar/Bar.boo");
        EXPECT_TRUE(log.hasSubstringIgnoreCase("Favouring"));
    }
    // Empty new
    {
        babelwires::FilePath path("Bar/Bar.boo");
        path.resolveRelativeTo(std::filesystem::path(), tmp / "Foo", log);
        EXPECT_EQ(path, tmp / "Foo/Bar/Bar.boo");
    }
    // Empty old
    {
        babelwires::FilePath path("Bar/Bar.boo");
        path.resolveRelativeTo(tmp / "Foo", std::filesystem::path(), log);
        EXPECT_EQ(path, tmp / "Foo/Bar/Bar.boo");
    }
    // Absolute file
    {
        babelwires::FilePath path(tmp / "Foo/Bar/Bar.boo");
        path.resolveRelativeTo(tmp / "Flerm", tmp / "Glurg", log);
        EXPECT_EQ(path, tmp / "Foo/Bar/Bar.boo");
    }
    // Windows-style absolute file
    {
        babelwires::FilePath path("c:/Foo/Bar/Bar.boo");
        path.resolveRelativeTo(tmp / "Flerm", "d:/Glurg", log);
        EXPECT_EQ(path, "c:/Foo/Bar/Bar.boo");
    }
}

