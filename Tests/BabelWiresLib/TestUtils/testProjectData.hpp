#pragma once

#include <BabelWiresLib/Project/projectData.hpp>

namespace testUtils {
    /// A source file wired to a processor wired to a target file.
    /// The file features are TestFileFeatures.
    /// The processor is a TestProcessor.
    struct TestProjectData : babelwires::ProjectData {
        /// The field identifiers have zero discriminators after construction.
        TestProjectData();

        /// Ensure the discriminators in the fields in the paths match those stored in the current context by
        /// attempting to resolve the paths against a TestRecordFeature and TestFileFeature.
        /// Note: This assumes the elements are still in the order they were on construction.
        void resolvePathsInCurrentContext(const babelwires::ProjectContext& context);

        /// Set the file paths in the features to the give ones. Also updates the members of this class.
        void setFilePaths(std::string_view sourceFilePath, std::string_view targetFilePath);

        /// Test that the structure of the data matches the structure as constructed.
        /// Also check that the discriminators in the path IDs match the provided values.
        /// This is agnostic about the order of the elements.
        // Note: The first argument is not required to be an instance of this class.
        static void testProjectDataAndDisciminators(const babelwires::ProjectData& projectData,
                                                    int recordIntDiscriminator, int recordArrayDiscriminator,
                                                    int recordRecordDiscriminator, int recordInt2Disciminator,
                                                    int fileIntChildDiscriminator);

        /// Test that the structure of the data matches the structure as constructed.
        /// Also check that the discriminators in the path IDs match the currently registered values.
        /// This is agnostic about the order of the elements.
        // Note: The first argument is not required to be an instance of this class.
        static void testProjectData(const babelwires::ProjectContext& context, const babelwires::ProjectData& projectData);

        static constexpr babelwires::ElementId c_sourceElementId = 12;
        static constexpr babelwires::ElementId c_processorId = 6;
        static constexpr babelwires::ElementId c_targetElementId = 45;

        std::string m_sourceFilePath;
        std::string m_targetFilePath;
    };
} // namespace testUtils
