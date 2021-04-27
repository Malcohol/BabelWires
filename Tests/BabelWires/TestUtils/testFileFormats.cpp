#include "Tests/BabelWires/TestUtils/testFileFormats.hpp"

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Features/featureMixins.hpp"

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

std::string libTestUtils::TestSourceFileFactory::getThisIdentifier() {
    return s_fileFormatId;
}

std::string libTestUtils::TestSourceFileFactory::getFileExtension() {
    return s_fileFormatId;
}

libTestUtils::TestSourceFileFactory::TestSourceFileFactory()
    : SourceFileFormat(s_fileFormatId, s_fileFormatId, 1, {s_fileFormatId}) {}

std::string libTestUtils::TestSourceFileFactory::getManufacturerName() const {
    return s_manufacturer;
}

std::string libTestUtils::TestSourceFileFactory::getProductName() const {
    return s_product;
}

namespace {
    char getFileDataInternal(babelwires::DataSource& dataSource) {
        int value = 0;
        for (char c : s_fileFormatId) {
            babelwires::Byte d = dataSource.getNextByte();
            if (c != 0) {
                if (d != c) {
                    throw babelwires::ParseException() << "Invalid TestSourceFileFactory file";
                }
            } else {
                value = d;
            }
        }
        if (!dataSource.isEof()) {
            throw babelwires::ParseException() << "Invalid TestSourceFileFactory file";
        }
        return value;
    }
} // namespace

char libTestUtils::TestSourceFileFactory::getFileData(const std::filesystem::path& path) {
    babelwires::FileDataSource dataSource(path.c_str());
    return getFileDataInternal(dataSource);
}

std::unique_ptr<babelwires::FileFeature>
libTestUtils::TestSourceFileFactory::loadFromFile(babelwires::DataSource& dataSource,
                                           babelwires::UserLogger& userLogger) const {
    const int value = getFileDataInternal(dataSource);
    auto newFeature = std::make_unique<TestFileFeature>();
    newFeature->m_intChildFeature->set(value);
    return newFeature;
}

void libTestUtils::TestSourceFileFactory::writeToTestFile(const std::filesystem::path& path, char testData) {
    std::ofstream fs(path);
    fs << s_fileFormatId << testData;
}

libTestUtils::TestTargetFileFactory::TestTargetFileFactory()
    : TargetFileFactory(s_factoryFormatId, s_factoryFormatId, 3, {s_fileFormatId}) {}

std::string libTestUtils::TestTargetFileFactory::getThisIdentifier() {
    return s_factoryFormatId;
}

std::string libTestUtils::TestTargetFileFactory::getManufacturerName() const {
    return s_manufacturer;
}

std::string libTestUtils::TestTargetFileFactory::getProductName() const {
    return s_product;
}

std::unique_ptr<babelwires::FileFeature> libTestUtils::TestTargetFileFactory::createNewFeature() const {
    return std::make_unique<TestFileFeature>();
}

void libTestUtils::TestTargetFileFactory::writeToFile(const babelwires::FileFeature& fileFeature, std::ostream& os,
                                                       babelwires::UserLogger& userLogger) const {
    const TestFileFeature& testFileFeature = dynamic_cast<const TestFileFeature&>(fileFeature);
    os << s_fileFormatId << char(testFileFeature.m_intChildFeature->get());
}
