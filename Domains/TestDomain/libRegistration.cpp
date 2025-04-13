/**
 * Register factories etc. for SeqWires into BabelWires.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <Domains/TestDomain/libRegistration.hpp>

#include <Domains/TestDomain/testArrayType.hpp>
#include <Domains/TestDomain/testEnum.hpp>
#include <Domains/TestDomain/testFileFormats.hpp>
#include <Domains/TestDomain/testProcessor.hpp>
#include <Domains/TestDomain/testRecordType.hpp>
#include <Domains/TestDomain/testRecordWithVariantsType.hpp>
#include <Domains/TestDomain/testSumType.hpp>
#include <Domains/TestDomain/testTupleType.hpp>

#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Processors/processorFactoryRegistry.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

void testDomain::registerLib(babelwires::ProjectContext& context) {
    context.m_typeSystem.addEntry<testDomain::TestEnum>();
    context.m_typeSystem.addEntry<testDomain::TestSubEnum>();
    context.m_typeSystem.addEntry<testDomain::TestSubSubEnum1>();
    context.m_typeSystem.addEntry<testDomain::TestSubSubEnum2>();
    context.m_typeSystem.addEntry<testDomain::TestSimpleArrayType>();
    context.m_typeSystem.addEntry<testDomain::TestCompoundArrayType>();
    context.m_typeSystem.addEntry<testDomain::TestSimpleRecordType>();
    context.m_typeSystem.addEntry<testDomain::TestComplexRecordType>();
    context.m_typeSystem.addEntry<testDomain::TestRecordWithVariantsType>();
    context.m_typeSystem.addEntry<testDomain::TestSumType>();
    context.m_typeSystem.addEntry<testDomain::TestTupleType>();

    context.m_typeSystem.addEntry<testDomain::TestProcessorInputOutputType>();

    context.m_processorReg.addProcessor<TestProcessor>();
}
