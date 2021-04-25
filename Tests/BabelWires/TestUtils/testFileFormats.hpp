#include "BabelWires/Features/numericFeature.hpp"
#include "BabelWires/FileFormat/fileFeature.hpp"
#include "BabelWires/FileFormat/fileFormat.hpp"
#include "BabelWires/Project/FeatureElements/featureElementData.hpp"

#include <filesystem>

namespace libTestUtils {

    /// A record with an int feature at path "aaa".
    struct TestFileFeature : babelwires::FileFeature {
        TestFileFeature();

        /// The id of the child feature.
        /// Deliberately match the field in TestRecordFeature.
        static constexpr char s_intChildInitializer[] = "aaa";

        static constexpr char s_intChildFieldName[] = "my int";

        static constexpr char s_intChildUuid[] = "00000000-1111-2222-3333-800000000BBB";

        babelwires::FieldIdentifier m_intChildId;

        /// A shortcut for accessing at the child feature.
        babelwires::IntFeature* m_intChildFeature;

        static const babelwires::FeaturePath s_pathToIntChild;
    };

    /// A file format that can save and load some test data.
    /// The serialized format is just the identifier followed by a single byte which carries the value of
    /// intChildFeature. This has version 1.
    struct TestFileFormat : babelwires::FileFormat {
        static std::string getThisIdentifier();
        static std::string getFileExtension();

        TestFileFormat();
        std::string getManufacturerName() const override;
        std::string getProductName() const override;
        std::unique_ptr<babelwires::FileFeature> loadFromFile(babelwires::DataSource& dataSource,
                                                              babelwires::UserLogger& userLogger) const override;

        static char getFileData(const std::filesystem::path& path);
        static void writeToTestFile(const std::filesystem::path& path, char testData = 3);
    };

    /// A factor for construction new file features.
    /// This is given version 3, to allow version testing.
    struct TestFileFeatureFactory : babelwires::FileFeatureFactory {
        static std::string getThisIdentifier();

        TestFileFeatureFactory();
        std::string getManufacturerName() const override;
        std::string getProductName() const override;
        std::unique_ptr<babelwires::FileFeature> createNewFeature() const override;
        void writeToFile(const babelwires::FileFeature& fileFeature, std::ostream& os,
                         babelwires::UserLogger& userLogger) const override;
    };

} // namespace libTestUtils
