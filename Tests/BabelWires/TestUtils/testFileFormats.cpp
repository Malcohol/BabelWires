#include "Tests/BabelWires/TestUtils/testFileFormats.hpp"

#include "BabelWiresLib/Features/Path/fieldNameRegistry.hpp"
#include "BabelWiresLib/Features/featureMixins.hpp"

#include "Common/IO/fileDataSource.hpp"

#include "Common/exceptions.hpp"

#include <fstream>

const babelwires::FeaturePath libTestUtils::TestFileFeature::s_pathToIntChild =
    babelwires::FeaturePath::deserializeFromString(libTestUtils::TestFileFeature::s_intChildInitializer);
namespace {
    const char s_fileFormatId[] = "testFileFormat";
    const char s_factoryFormatId[] = "testFactoryFormat";
    const char s_manufacturer[] = "Test Manufacturer";
    const char s_product[] = "Test Product";
} // namespace

libTestUtils::TestFileFeature::TestFileFeature()
    : babelwires::FileFeature(s_fileFormatId)
    , m_intChildId(babelwires::FieldNameRegistry::write()->addFieldName(
          s_intChildInitializer, s_intChildFieldName, s_intChildUuid,
          babelwires::FieldNameRegistry::Authority::isAuthoritative)) {
    m_intChildFeature =
        addField(std::make_unique<babelwires::HasStaticRange<babelwires::IntFeature, 0, 255>>(), m_intChildId);
}

std::string libTestUtils::TestSourceFileFormat::getThisIdentifier() {
    return s_fileFormatId;
}

std::string libTestUtils::TestSourceFileFormat::getFileExtension() {
    return s_fileFormatId;
}

libTestUtils::TestSourceFileFormat::TestSourceFileFormat()
    : SourceFileFormat(s_fileFormatId, s_fileFormatId, 1, {s_fileFormatId}) {}

std::string libTestUtils::TestSourceFileFormat::getManufacturerName() const {
    return s_manufacturer;
}

std::string libTestUtils::TestSourceFileFormat::getProductName() const {
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

char libTestUtils::TestSourceFileFormat::getFileData(const std::filesystem::path& path) {
    babelwires::FileDataSource dataSource(path);
    return getFileDataInternal(dataSource);
}

std::unique_ptr<babelwires::FileFeature>
libTestUtils::TestSourceFileFormat::loadFromFile(babelwires::DataSource& dataSource,
                                           babelwires::UserLogger& userLogger) const {
    const int value = getFileDataInternal(dataSource);
    auto newFeature = std::make_unique<TestFileFeature>();
    newFeature->m_intChildFeature->set(value);
    return newFeature;
}

void libTestUtils::TestSourceFileFormat::writeToTestFile(const std::filesystem::path& path, char testData) {
    std::ofstream fs(path);
    fs << s_fileFormatId << testData;
}

libTestUtils::TestTargetFileFormat::TestTargetFileFormat()
    : TargetFileFormat(s_factoryFormatId, s_factoryFormatId, 3, {s_fileFormatId}) {}

std::string libTestUtils::TestTargetFileFormat::getThisIdentifier() {
    return s_factoryFormatId;
}

std::string libTestUtils::TestTargetFileFormat::getManufacturerName() const {
    return s_manufacturer;
}

std::string libTestUtils::TestTargetFileFormat::getProductName() const {
    return s_product;
}

std::unique_ptr<babelwires::FileFeature> libTestUtils::TestTargetFileFormat::createNewFeature() const {
    return std::make_unique<TestFileFeature>();
}

void libTestUtils::TestTargetFileFormat::writeToFile(const babelwires::FileFeature& fileFeature, std::ostream& os,
                                                       babelwires::UserLogger& userLogger) const {
    const TestFileFeature& testFileFeature = *fileFeature.as<TestFileFeature>();
    os << s_fileFormatId << char(testFileFeature.m_intChildFeature->get());
}
