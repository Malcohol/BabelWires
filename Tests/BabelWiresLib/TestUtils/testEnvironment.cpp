#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProcessor.hpp>

testUtils::TestEnvironment::TestEnvironment()
    // Try to ensure the tests are deterministic by fixing the random seed.
    : m_projectContext{m_sourceFileFormatReg, m_targetFileFormatReg, m_processorReg, m_deserializationReg, m_typeSystem,
                       std::default_random_engine(0x123456789abcdeful)}
    , m_project(m_projectContext, m_log) {
    m_targetFileFormatReg.addEntry<TestTargetFileFormat>();
    m_sourceFileFormatReg.addEntry<TestSourceFileFormat>();
    m_processorReg.addEntry<TestProcessorFactory>();
}
