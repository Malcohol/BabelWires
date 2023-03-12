#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>

#include <BabelWiresLib/Features/featureMixins.hpp>

#include <Common/IO/fileDataSource.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Identifiers/registeredIdentifier.hpp>
#include <Common/exceptions.hpp>

#include <fstream>

const babelwires::FeaturePath testUtils::TestFileFeature::s_pathToIntChild =
    babelwires::FeaturePath::deserializeFromString(testUtils::TestFileFeature::s_intChildInitializer);
namespace {
    const char s_fileFormatId[] = "testFileFormat";
    const char s_factoryFormatId[] = "testFactoryFormat";
    const char s_manufacturer[] = "Test Manufacturer";
    const char s_product[] = "Test Product";
} // namespace

testUtils::TestFileFeature::TestFileFeature(const babelwires::ProjectContext& context)
    : babelwires::FileFeature(context, s_fileFormatId)
    , m_intChildId(babelwires::IdentifierRegistry::write()->addShortIdentifierWithMetadata(
          s_intChildInitializer, s_intChildFieldName, s_intChildUuid,
          babelwires::IdentifierRegistry::Authority::isAuthoritative)) {
    m_intChildFeature =
        addField(std::make_unique<babelwires::HasStaticRange<babelwires::IntFeature, 0, 255>>(), m_intChildId);
}

babelwires::LongId testUtils::TestSourceFileFormat::getThisIdentifier() {
    return s_fileFormatId;
}

std::string testUtils::TestSourceFileFormat::getFileExtension() {
    return s_fileFormatId;
}

testUtils::TestSourceFileFormat::TestSourceFileFormat()
    : SourceFileFormat(babelwires::IdentifierRegistry::write()->addLongIdentifierWithMetadata(
                           s_fileFormatId, s_fileFormatId, "f557b89a-2499-465a-a605-5ef7f69284c4",
                           babelwires::IdentifierRegistry::Authority::isAuthoritative),
                       1, {s_fileFormatId}) {}

std::string testUtils::TestSourceFileFormat::getManufacturerName() const {
    return s_manufacturer;
}

std::string testUtils::TestSourceFileFormat::getProductName() const {
    return s_product;
}

namespace {
    char getFileDataInternal(babelwires::DataSource& dataSource) {
        int value = 0;
        for (char c : s_fileFormatId) {
            babelwires::Byte d = dataSource.getNextByte();
            if (c != 0) {
                if (d != c) {
                    throw babelwires::ParseException() << "Invalid TestSourceFileFormat file";
                }
            } else {
                value = d;
            }
        }
        if (!dataSource.isEof()) {
            throw babelwires::ParseException() << "Invalid TestSourceFileFormat file";
        }
        return value;
    }
} // namespace

char testUtils::TestSourceFileFormat::getFileData(const std::filesystem::path& path) {
    babelwires::FileDataSource dataSource(path);
    return getFileDataInternal(dataSource);
}

std::unique_ptr<babelwires::FileFeature>
testUtils::TestSourceFileFormat::loadFromFile(babelwires::DataSource& dataSource,
                                              const babelwires::ProjectContext& projectContext,
                                              babelwires::UserLogger& userLogger) const {
    const int value = getFileDataInternal(dataSource);
    auto newFeature = std::make_unique<TestFileFeature>(projectContext);
    newFeature->m_intChildFeature->set(value);
    return newFeature;
}

void testUtils::TestSourceFileFormat::writeToTestFile(const std::filesystem::path& path, char testData) {
    std::ofstream fs(path);
    fs << s_fileFormatId << testData;
}

testUtils::TestTargetFileFormat::TestTargetFileFormat()
    : TargetFileFormat(babelwires::IdentifierRegistry::write()->addLongIdentifierWithMetadata(
                           s_factoryFormatId, s_factoryFormatId, "a9a603aa-9d83-4f12-ac35-de0056d5a568",
                           babelwires::IdentifierRegistry::Authority::isAuthoritative),
                       3, {s_fileFormatId}) {}

babelwires::LongId testUtils::TestTargetFileFormat::getThisIdentifier() {
    return s_factoryFormatId;
}

std::string testUtils::TestTargetFileFormat::getManufacturerName() const {
    return s_manufacturer;
}

std::string testUtils::TestTargetFileFormat::getProductName() const {
    return s_product;
}

std::unique_ptr<babelwires::FileFeature>
testUtils::TestTargetFileFormat::createNewFeature(const babelwires::ProjectContext& projectContext) const {
    return std::make_unique<TestFileFeature>(projectContext);
}

void testUtils::TestTargetFileFormat::writeToFile(const babelwires::ProjectContext& projectContext,
                                                  babelwires::UserLogger& userLogger,
                                                  const babelwires::FileFeature& fileFeature, std::ostream& os) const {
    const TestFileFeature& testFileFeature = *fileFeature.as<TestFileFeature>();
    os << s_fileFormatId << char(testFileFeature.m_intChildFeature->get());
}
