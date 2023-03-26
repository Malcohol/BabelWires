#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProcessor.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <BabelWiresLib/libRegistration.hpp>

testUtils::TestEnvironment::TestEnvironment()
    // Try to ensure the tests are deterministic by fixing the random seed.
    : m_projectContext{m_deserializationReg, m_sourceFileFormatReg, m_targetFileFormatReg, m_processorReg, m_typeSystem,
                       std::default_random_engine(0x123456789abcdeful)}
    , m_project(m_projectContext, m_log) {

    babelwires::registerLib(m_projectContext);
    
    m_targetFileFormatReg.addEntry<TestTargetFileFormat>();
    m_sourceFileFormatReg.addEntry<TestSourceFileFormat>();
    m_processorReg.addEntry<TestProcessorFactory>();

    m_typeSystem.addEntry<testUtils::TestType>();
    m_typeSystem.addEntry<testUtils::TestEnum>();
    m_typeSystem.addEntry<testUtils::TestSubEnum>();
    m_typeSystem.addEntry<testUtils::TestSubSubEnum1>();
    m_typeSystem.addEntry<testUtils::TestSubSubEnum2>();

    m_typeSystem.addRelatedTypes(testUtils::TestEnum::getThisIdentifier(),
                               {{}, {testUtils::TestSubSubEnum1::getThisIdentifier()}});
    m_typeSystem.addRelatedTypes(testUtils::TestSubSubEnum2::getThisIdentifier(),
                               {{testUtils::TestEnum::getThisIdentifier()}, {}});
    m_typeSystem.addRelatedTypes(
        testUtils::TestSubEnum::getThisIdentifier(),
        {{testUtils::TestEnum::getThisIdentifier()},
         {testUtils::TestSubSubEnum1::getThisIdentifier(), testUtils::TestSubSubEnum2::getThisIdentifier()}});

    m_typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();
    m_typeSystem.addTypeConstructor<testUtils::TestBinaryTypeConstructor>();
}
