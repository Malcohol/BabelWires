#include <BabelWiresLib/FileFormat/fileFeature.hpp>
#include <BabelWiresLib/FileFormat/sourceFileFormat.hpp>
#include <BabelWiresLib/FileFormat/targetFileFormat.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>
#include <BabelWiresLib/Types/Int/intFeature.hpp>

#include <filesystem>

namespace testUtils {

    /// A record with an int feature at path "aaa".
    struct TestFileFeature : babelwires::FileFeature {
        TestFileFeature(const babelwires::ProjectContext& context);

        /// The id of the child feature.
        /// Deliberately match the field in TestRecordFeature.
        static constexpr char s_intChildInitializer[] = "intR0";

        static constexpr char s_intChildFieldName[] = "my int";

        static constexpr char s_intChildUuid[] = "00000000-1111-2222-3333-800000000BBB";

        babelwires::ShortId m_intChildId;

        /// A shortcut for accessing at the child feature.
        babelwires::IntFeature* m_intChildFeature;

        static const babelwires::FeaturePath s_pathToIntChild;
    };

    /// A file type which wraps a TestSimpleRecordType.
    babelwires::TypeRef getTestFileType();

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
