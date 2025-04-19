#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <BabelWiresLib/libRegistration.hpp>

#include <Domains/TestDomain/libRegistration.hpp>

#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

testUtils::TestEnvironment::TestEnvironment()
    // Try to ensure the tests are deterministic by fixing the random seed.
    : m_projectContext{m_deserializationReg, m_sourceFileFormatReg, m_targetFileFormatReg,
                       m_processorReg,       m_typeSystem,          std::default_random_engine(0x123456789abcdeful)}
    , m_project(m_projectContext, m_log) {

    babelwires::registerLib(m_projectContext);
    testDomain::registerLib(m_projectContext);

    {
        // Also register some irrelevant field names.
        babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
            "Flum", "Flum", "41000000-1111-2222-3333-800000000100",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
            "Zarg", "Zarg", "41000000-1111-2222-3333-800000000101",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);
    }

    m_typeSystem.addEntry<testUtils::TestType>();

    m_typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();
    m_typeSystem.addTypeConstructor<testUtils::TestBinaryTypeConstructor>();
    m_typeSystem.addTypeConstructor<testUtils::TestMixedTypeConstructor>();
}
