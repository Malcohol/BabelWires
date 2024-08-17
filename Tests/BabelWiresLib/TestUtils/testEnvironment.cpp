#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <BabelWiresLib/libRegistration.hpp>

#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProcessor.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecord.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordWithVariantsType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

testUtils::TestEnvironment::TestEnvironment()
    // Try to ensure the tests are deterministic by fixing the random seed.
    : m_projectContext{m_deserializationReg, m_sourceFileFormatReg, m_targetFileFormatReg,
                       m_processorReg,       m_typeSystem,          std::default_random_engine(0x123456789abcdeful)}
    , m_project(m_projectContext, m_log) {

    babelwires::registerLib(m_projectContext);

    {
        // Ensure some of the test discriminators are not default.
        babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
            testUtils::TestRecordFeature::s_intIdInitializer, "test int", "41000000-1111-2222-3333-800000000001",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
            testUtils::TestRecordFeature::s_intIdInitializer, "test int 1", "42000000-1111-2222-3333-800000000001",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
            testUtils::TestRecordFeature::s_intIdInitializer, "test int 2", "43000000-1111-2222-3333-800000000001",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
            testUtils::TestRecordFeature::s_arrayIdInitializer, "test array", "41000000-1111-2222-3333-800000000002",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
            testUtils::TestRecordFeature::s_arrayIdInitializer, "test array 1", "42000000-1111-2222-3333-800000000002",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
            testUtils::TestRecordFeature::s_recordIdInitializer, "test record", "41000000-1111-2222-3333-800000000003",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);

        // Also register some irrelevant field names.
        babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
            "Flum", "Flum", "41000000-1111-2222-3333-800000000100",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
            "Zarg", "Zarg", "41000000-1111-2222-3333-800000000101",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);
    }

    m_targetFileFormatReg.addEntry<TestTargetFileFormat>();
    m_sourceFileFormatReg.addEntry<TestSourceFileFormat>();

    m_typeSystem.addEntry<testUtils::TestType>();
    m_typeSystem.addEntry<testUtils::TestEnum>();
    m_typeSystem.addEntry<testUtils::TestSubEnum>();
    m_typeSystem.addEntry<testUtils::TestSubSubEnum1>();
    m_typeSystem.addEntry<testUtils::TestSubSubEnum2>();
    m_typeSystem.addEntry<testUtils::TestSimpleArrayType>();
    m_typeSystem.addEntry<testUtils::TestCompoundArrayType>();
    m_typeSystem.addEntry<testUtils::TestSimpleRecordType>();
    m_typeSystem.addEntry<testUtils::TestComplexRecordType>();
    m_typeSystem.addEntry<testUtils::TestRecordWithVariantsType>();

    m_typeSystem.addEntry<testUtils::TestProcessorInputOutputType>();

    m_processorReg.addProcessor<TestProcessor>();

    m_typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();
    m_typeSystem.addTypeConstructor<testUtils::TestBinaryTypeConstructor>();
    m_typeSystem.addTypeConstructor<testUtils::TestMixedTypeConstructor>();
}
