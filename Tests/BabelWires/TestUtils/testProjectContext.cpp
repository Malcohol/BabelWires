#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWires/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWires/TestUtils/testProcessor.hpp"

libTestUtils::TestProjectContext::TestProjectContext()
    // Try to ensure the tests are deterministic by fixing the random seed.
    : m_projectContext{m_sourceFileFormatReg, m_targetFileFormatReg, m_processorReg, m_deserializationReg,
                       std::default_random_engine(0x123456789abcdeful)}
    , m_project(m_projectContext, m_log) {
    m_targetFileFormatReg.addEntry(std::make_unique<TestTargetFileFormat>());
    m_sourceFileFormatReg.addEntry(std::make_unique<TestSourceFileFormat>());
    m_processorReg.addEntry(std::make_unique<TestProcessorFactory>());
}
