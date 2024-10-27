#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>

#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Types/File/fileTypeConstructor.hpp>
#include <BabelWiresLib/Types/File/fileType.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <Common/IO/fileDataSource.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Identifiers/registeredIdentifier.hpp>
#include <Common/exceptions.hpp>

#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

#include <fstream>

namespace {
    const char s_fileFormatId[] = "testFileFormat";
    const char s_factoryFormatId[] = "testFactoryFormat";
    const char s_manufacturer[] = "Test Manufacturer";
    const char s_product[] = "Test Product";
} // namespace

babelwires::TypeRef testUtils::getTestFileType() {
    return babelwires::FileTypeConstructor::makeTypeRef(TestSimpleRecordType::getThisIdentifier());
}

babelwires::Path testUtils::getTestFileElementPathToInt0() {
    babelwires::Path path;
    path.pushStep(babelwires::PathStep(babelwires::FileType::getStepToContents()));
    path.pushStep(babelwires::PathStep(TestSimpleRecordType::getInt0Id()));
    return path;
}

babelwires::LongId testUtils::TestSourceFileFormat::getThisIdentifier() {
    return s_fileFormatId;
}

std::string testUtils::TestSourceFileFormat::getFileExtension() {
    return s_fileFormatId;
}

testUtils::TestSourceFileFormat::TestSourceFileFormat()
    : SourceFileFormat(babelwires::IdentifierRegistry::write()->addLongIdWithMetadata(
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

std::unique_ptr<babelwires::ValueTreeRoot>
testUtils::TestSourceFileFormat::loadFromFile(babelwires::DataSource& dataSource,
                                              const babelwires::ProjectContext& projectContext,
                                              babelwires::UserLogger& userLogger) const {
    const int value = getFileDataInternal(dataSource);
    auto newFeature = std::make_unique<babelwires::ValueTreeRoot>(projectContext.m_typeSystem, getTestFileType());
    newFeature->setToDefault();
    TestSimpleRecordType::Instance instance{newFeature->getChild(0)->is<babelwires::ValueTreeNode>()};
    instance.getintR0().set(value);
    return newFeature;
}

void testUtils::TestSourceFileFormat::writeToTestFile(const std::filesystem::path& path, char testData) {
    std::ofstream fs(path);
    fs << s_fileFormatId << testData;
}

testUtils::TestTargetFileFormat::TestTargetFileFormat()
    : TargetFileFormat(babelwires::IdentifierRegistry::write()->addLongIdWithMetadata(
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

std::unique_ptr<babelwires::ValueTreeRoot>
testUtils::TestTargetFileFormat::createNewFeature(const babelwires::ProjectContext& projectContext) const {
    return std::make_unique<babelwires::ValueTreeRoot>(projectContext.m_typeSystem, getTestFileType());
}

void testUtils::TestTargetFileFormat::writeToFile(const babelwires::ProjectContext& projectContext,
                                                  babelwires::UserLogger& userLogger,
                                                  const babelwires::ValueTreeRoot& contents,
                                                  std::ostream& os) const {
    TestSimpleRecordType::ConstInstance instance{contents.getChild(0)->is<babelwires::ValueTreeNode>()};
    os << s_fileFormatId << char(instance.getintR0().get());
}
