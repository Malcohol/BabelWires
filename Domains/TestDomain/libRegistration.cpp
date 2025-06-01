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
#include <Domains/TestDomain/testParallelProcessor.hpp>
#include <Domains/TestDomain/testProcessor.hpp>
#include <Domains/TestDomain/testRecordType.hpp>
#include <Domains/TestDomain/testRecordTypeHierarchy.hpp>
#include <Domains/TestDomain/testRecordWithVariantsType.hpp>
#include <Domains/TestDomain/testRecordWithVariantsTypeHierarchy.hpp>
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
    context.m_typeSystem.addEntry<testDomain::TestTupleType>();
    context.m_typeSystem.addEntry<testDomain::RecordWithNoFields>();
    context.m_typeSystem.addEntry<testDomain::TestSumType>();

    context.m_typeSystem.addEntry<testDomain::RecordA0>();
    context.m_typeSystem.addEntry<testDomain::RecordA1>();
    context.m_typeSystem.addEntry<testDomain::RecordAS>();
    context.m_typeSystem.addEntry<testDomain::RecordB>();
    context.m_typeSystem.addEntry<testDomain::RecordAB>();
    context.m_typeSystem.addEntry<testDomain::RecordAOpt>();
    context.m_typeSystem.addEntry<testDomain::RecordAOptFixed>();
    context.m_typeSystem.addEntry<testDomain::RecordABOpt>();
    context.m_typeSystem.addEntry<testDomain::RecordAOptS>();
    context.m_typeSystem.addEntry<testDomain::RecordABOptChild>(context.m_typeSystem);
    context.m_typeSystem.addEntry<testDomain::RecordAsub0>();
    context.m_typeSystem.addEntry<testDomain::RecordAsubBsup>();

    context.m_typeSystem.addEntry<testDomain::RecordVWithNoFields>();
    context.m_typeSystem.addEntry<testDomain::RecordVA0>();
    context.m_typeSystem.addEntry<testDomain::RecordVA1>();
    context.m_typeSystem.addEntry<testDomain::RecordVAS>();
    context.m_typeSystem.addEntry<testDomain::RecordVB>();
    context.m_typeSystem.addEntry<testDomain::RecordVAB>();
    context.m_typeSystem.addEntry<testDomain::RecordVAV0>();
    context.m_typeSystem.addEntry<testDomain::RecordVABV0>();
    context.m_typeSystem.addEntry<testDomain::RecordVABV1>();
    context.m_typeSystem.addEntry<testDomain::RecordVABV01>();
    context.m_typeSystem.addEntry<testDomain::RecordVAVB>();

    context.m_typeSystem.addEntry<testDomain::TestParallelProcessorInput>();
    context.m_typeSystem.addEntry<testDomain::TestParallelProcessorOutput>();
    context.m_typeSystem.addEntry<testDomain::TestProcessorInputOutputType>();

    context.m_processorReg.addProcessor<TestProcessor>();
    context.m_processorReg.addProcessor<TestParallelProcessor>();

    context.m_targetFileFormatReg.addEntry<TestTargetFileFormat>();
    context.m_sourceFileFormatReg.addEntry<TestSourceFileFormat>();
}
