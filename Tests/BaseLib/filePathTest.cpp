#include <gtest/gtest.h>

#include <BaseLib/DataContext/filePath.hpp>

#include <BaseLib/exceptions.hpp>

#include <Tests/TestUtils/testLog.hpp>
#include <Tests/TestUtils/tempFilePath.hpp>

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
    EXPECT_EQ(babelwires::FilePath::deserializeFromString(""), "");
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
#if false && (defined(_WIN64) || defined(_WIN32))
    // TODO std::filesystem is throwing an exception when there is no d: drive, so I cannot unit test this.
    // This doesn't have to work on non-windows platforms, since the functionality is only needed when exporting.
    {
        babelwires::FilePath path("c:/Foo/Bar.boo");
        path.interpretRelativeTo("d:/Flerm");
        EXPECT_EQ(path, "c:/Foo/Bar.boo");
    }
#endif
}

TEST(FilePathTest, resolveRelativeTo) {
    testUtils::TestLogWithListener log;

    std::filesystem::path root = std::filesystem::canonical(std::filesystem::temp_directory_path());

    testUtils::TempDirectory FooBar("Foo/Bar");
    testUtils::TempDirectory OomBar("Oom/Bar");

    testUtils::TempFilePath fileWhichExists("Foo/Bar/Bar.boo");
    fileWhichExists.ensureExists();

    testUtils::TempFilePath fileWhichExists2("Oom/Bar/Bar.boo");
    fileWhichExists2.ensureExists();

    // Same context.
    {
        babelwires::FilePath path("Bar.boo");
        path.resolveRelativeTo(root / "Foo/Bar", root / "Foo/Bar", log);
        EXPECT_EQ(path, root / "Foo/Bar/Bar.boo");
    }
    // Same context.
    {
        babelwires::FilePath path("Bar/Bar.boo");
        path.resolveRelativeTo(root / "Foo", root / "Foo", log);
        EXPECT_EQ(path, root / "Foo/Bar/Bar.boo");
    }
    // Same context, file doesn't exist.
    {
        babelwires::FilePath path("Bar/Bar.boo");
        path.resolveRelativeTo(root / "Flerm", root / "Flerm", log);
        EXPECT_EQ(path, root / "Flerm/Bar/Bar.boo");
    }
    // New exists, old doesn't.
    {
        babelwires::FilePath path("Bar/Bar.boo");
        path.resolveRelativeTo(root / "Foo", root / "Flerm", log);
        EXPECT_EQ(path, root / "Foo/Bar/Bar.boo");
    }
    // Old exists, new doesn't.
    {
        babelwires::FilePath path("Bar/Bar.boo");
        path.resolveRelativeTo(root / "Flerm", root / "Foo", log);
        EXPECT_EQ(path, root / "Foo/Bar/Bar.boo");
    }
    // Both exist. New prefered. Check log.
    {
        log.clear();
        babelwires::FilePath path("Bar/Bar.boo");
        path.resolveRelativeTo(root / "Foo", root / "Oom", log);
        EXPECT_EQ(path, root / "Foo/Bar/Bar.boo");
        EXPECT_TRUE(log.hasSubstringIgnoreCase("Favouring"));
    }
    // Empty new
    {
        babelwires::FilePath path("Bar/Bar.boo");
        path.resolveRelativeTo(std::filesystem::path(), root / "Foo", log);
        EXPECT_EQ(path, root / "Foo/Bar/Bar.boo");
    }
    // Empty old
    {
        babelwires::FilePath path("Bar/Bar.boo");
        path.resolveRelativeTo(root / "Foo", std::filesystem::path(), log);
        EXPECT_EQ(path, root / "Foo/Bar/Bar.boo");
    }
    // Absolute file
    {
        babelwires::FilePath path(root / "Foo/Bar/Bar.boo");
        path.resolveRelativeTo(root / "Flerm", root / "Glurg", log);
        EXPECT_EQ(path, root / "Foo/Bar/Bar.boo");
    }
    // Windows-style absolute file
    {
        babelwires::FilePath path("c:/Foo/Bar/Bar.boo");
        path.resolveRelativeTo(root / "Flerm", "d:/Glurg", log);
        EXPECT_EQ(path, "c:/Foo/Bar/Bar.boo");
    }
}

