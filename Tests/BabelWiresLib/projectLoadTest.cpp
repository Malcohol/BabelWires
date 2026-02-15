#include <gtest/gtest.h>

#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/projectData.hpp>
#include <BabelWiresLib/Serialization/projectSerialization.hpp>

#include <Domains/TestDomain/testFileFormats.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

namespace {
    const char testSourceFile[] = "testSource.testfileformat";
    const char testTargetFile[] = "testTarget.testfileformat";
    const char testProjectFile[] = "testProject.babelwires";
} // namespace

TEST(ProjectLoadTest, loadAndProcessTestProject) {
    testUtils::TestEnvironment testEnvironment;

    ASSERT_TRUE(std::filesystem::exists(testProjectFile));

    const babelwires::ResultT<babelwires::ProjectData> projectData = babelwires::ProjectSerialization::loadFromFile(
        testProjectFile, testEnvironment.m_projectContext, testEnvironment.m_log);
    ASSERT_TRUE(projectData) << projectData.error().toString();
    babelwires::Project project(testEnvironment.m_projectContext, testEnvironment.m_log);
    project.setProjectData(*projectData);
    project.process();

    // TODO If we remove the file before loading the project, the target file will resolve to the original location of
    // the file. That makes sense for source files, but maybe it doesn't for target files...
    std::filesystem::remove(testTargetFile);
    EXPECT_FALSE(std::filesystem::exists(testTargetFile));

    project.tryToSaveAllTargets();

    ASSERT_TRUE(std::filesystem::exists(testTargetFile));
    const auto fileDataResult = testDomain::TestSourceFileFormat::getFileData(testTargetFile);
    ASSERT_TRUE(fileDataResult.has_value());
    const auto [r0, r1] = *fileDataResult;
    EXPECT_EQ(r0, 7);
    EXPECT_EQ(r1, 0);
}
