#include <BabelWiresLib/FileFormat/sourceFileFormat.hpp>
#include <BabelWiresLib/FileFormat/targetFileFormat.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <filesystem>

namespace testDomain {
    /// A file type which wraps a TestSimpleRecordType.
    babelwires::TypeRef getTestFileType();

    /// Get the path in a FileNode to Int0 in TestSimpleRecordType.
    babelwires::Path getTestFileElementPathToInt0();

    /// A file format that can save and load some test data.
    /// The serialized format is just the identifier followed two ints. This has version 1.
    struct TestSourceFileFormat : babelwires::SourceFileFormat {
        static babelwires::LongId getThisIdentifier();
        static std::string getFileExtension();

        TestSourceFileFormat();
        std::string getManufacturerName() const override;
        std::string getProductName() const override;
        std::unique_ptr<babelwires::ValueTreeRoot> loadFromFile(const std::filesystem::path& path,
                                                              const babelwires::ProjectContext& projectContext,
                                                              babelwires::UserLogger& userLogger) const override;

        static std::tuple<int, int> getFileData(const std::filesystem::path& path);
        static void writeToTestFile(const std::filesystem::path& path, int r0 = 0, int r1 = 0);
    };

    /// A factor for construction new file features.
    /// This is given version 3, to allow version testing.
    struct TestTargetFileFormat : babelwires::TargetFileFormat {
        static babelwires::LongId getThisIdentifier();

        TestTargetFileFormat();
        std::string getManufacturerName() const override;
        std::string getProductName() const override;
        std::unique_ptr<babelwires::ValueTreeRoot>
        createNewValue(const babelwires::ProjectContext& projectContext) const override;
        void writeToFile(const babelwires::ProjectContext& projectContext, babelwires::UserLogger& userLogger,
                         const babelwires::ValueTreeRoot& contents, const std::filesystem::path& path) const override;
    };

} // namespace testDomain
