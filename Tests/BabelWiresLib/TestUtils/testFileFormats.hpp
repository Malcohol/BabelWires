#include <BabelWiresLib/FileFormat/sourceFileFormat.hpp>
#include <BabelWiresLib/FileFormat/targetFileFormat.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <filesystem>

namespace testUtils {
    /// A file type which wraps a TestSimpleRecordType.
    babelwires::TypeRef getTestFileType();

    /// Get the path in a FileElement to Int0 in TestSimpleRecordType.
    babelwires::FeaturePath getTestFileElementPathToInt0();

    /// A file format that can save and load some test data.
    /// The serialized format is just the identifier followed by a single byte which carries the value of
    /// intChildFeature. This has version 1.
    struct TestSourceFileFormat : babelwires::SourceFileFormat {
        static babelwires::LongId getThisIdentifier();
        static std::string getFileExtension();

        TestSourceFileFormat();
        std::string getManufacturerName() const override;
        std::string getProductName() const override;
        std::unique_ptr<babelwires::SimpleValueFeature> loadFromFile(babelwires::DataSource& dataSource,
                                                              const babelwires::ProjectContext& projectContext,
                                                              babelwires::UserLogger& userLogger) const override;

        static char getFileData(const std::filesystem::path& path);
        static void writeToTestFile(const std::filesystem::path& path, char testData = 3);
    };

    /// A factor for construction new file features.
    /// This is given version 3, to allow version testing.
    struct TestTargetFileFormat : babelwires::TargetFileFormat {
        static babelwires::LongId getThisIdentifier();

        TestTargetFileFormat();
        std::string getManufacturerName() const override;
        std::string getProductName() const override;
        std::unique_ptr<babelwires::SimpleValueFeature>
        createNewFeature(const babelwires::ProjectContext& projectContext) const override;
        void writeToFile(const babelwires::ProjectContext& projectContext, babelwires::UserLogger& userLogger,
                         const babelwires::SimpleValueFeature& contents, std::ostream& os) const override;
    };

} // namespace testUtils
