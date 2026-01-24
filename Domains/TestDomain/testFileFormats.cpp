#include <Domains/TestDomain/testFileFormats.hpp>

#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Types/File/fileTypeConstructor.hpp>
#include <BabelWiresLib/Types/File/fileType.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <BaseLib/IO/fileDataSource.hpp>
#include <BaseLib/Identifiers/identifierRegistry.hpp>
#include <BaseLib/Identifiers/registeredIdentifier.hpp>
#include <BaseLib/exceptions.hpp>

#include <Domains/TestDomain/testRecordType.hpp>

#include <fstream>

namespace {
    const char s_fileFormat[] = "testFileFormat";
    const char s_manufacturer[] = "Test Manufacturer";
    const char s_product[] = "Test Product";
} // namespace

babelwires::TypeExp testDomain::getTestFileType() {
    return babelwires::FileTypeConstructor::makeTypeExp(TestSimpleRecordType::getThisIdentifier());
}

babelwires::Path testDomain::getTestFileElementPathToInt0() {
    babelwires::Path path;
    path.pushStep(babelwires::FileType::getStepToContents());
    path.pushStep(TestSimpleRecordType::getInt0Id());
    return path;
}

babelwires::LongId testDomain::TestSourceFileFormat::getThisIdentifier() {
    return BW_LONG_ID("TestSourceFormat", "Test Source File Format", "c2e45d49-2707-4109-8642-a0a87143c315");
}

std::string testDomain::TestSourceFileFormat::getFileExtension() {
    return s_fileFormat;
}

testDomain::TestSourceFileFormat::TestSourceFileFormat()
    : SourceFileFormat(getThisIdentifier(), 1, {s_fileFormat}) {}

std::string testDomain::TestSourceFileFormat::getManufacturerName() const {
    return s_manufacturer;
}

std::string testDomain::TestSourceFileFormat::getProductName() const {
    return s_product;
}

void testDomain::TestSourceFileFormat::writeToTestFile(const std::filesystem::path& path, int r0, int r1) {
    std::ofstream fs(path);
    fs << s_fileFormat << " " << r0 << " " << r1 << "\n";
}

std::tuple<int, int> testDomain::TestSourceFileFormat::getFileData(const std::filesystem::path& path) {
    std::string formatId;
    int r0, r1;
    try {
        std::ifstream is(path);
        is.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        is >> formatId >> r0 >> r1;
    } catch (...) {
        throw babelwires::ParseException() << "Failed to parse file at " << path;
    }
    if (formatId != s_fileFormat) {
        throw babelwires::ParseException() << "File at " << path << " was not in the expected format";
    }
    return {r0, r1};
}

std::unique_ptr<babelwires::ValueTreeRoot>
testDomain::TestSourceFileFormat::loadFromFile(const std::filesystem::path& path,
                                              const babelwires::ProjectContext& projectContext,
                                              babelwires::UserLogger& userLogger) const {
    auto [r0, r1] = getFileData(path);
    auto newFeature = std::make_unique<babelwires::ValueTreeRoot>(projectContext.m_typeSystem, getTestFileType());
    newFeature->setToDefault();
    TestSimpleRecordType::Instance instance{newFeature->getChild(0)->as<babelwires::ValueTreeNode>()};
    instance.getintR0().set(r0);
    instance.getintR1().set(r1);
    return newFeature;
}

testDomain::TestTargetFileFormat::TestTargetFileFormat()
    : TargetFileFormat(getThisIdentifier(),
                       3, {s_fileFormat}) {}

babelwires::LongId testDomain::TestTargetFileFormat::getThisIdentifier() {
    return BW_LONG_ID("TestTargetFormat", "Test Target File Format", "0e0bf791-c161-41d0-9690-223d05a057bd");
}

std::string testDomain::TestTargetFileFormat::getManufacturerName() const {
    return s_manufacturer;
}

std::string testDomain::TestTargetFileFormat::getProductName() const {
    return s_product;
}

std::unique_ptr<babelwires::ValueTreeRoot>
testDomain::TestTargetFileFormat::createNewValue(const babelwires::ProjectContext& projectContext) const {
    return std::make_unique<babelwires::ValueTreeRoot>(projectContext.m_typeSystem, getTestFileType());
}

void testDomain::TestTargetFileFormat::writeToFile(const babelwires::ProjectContext& projectContext,
                                                  babelwires::UserLogger& userLogger,
                                                  const babelwires::ValueTreeRoot& contents,
                                                  const std::filesystem::path& path) const {
    std::ofstream os(path);    
    TestSimpleRecordType::ConstInstance instance{contents.getChild(0)->as<babelwires::ValueTreeNode>()};
    TestSourceFileFormat::writeToTestFile(path, instance.getintR0().get(), instance.getintR1().get());
}
