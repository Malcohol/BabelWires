/**
 * Register factories etc. for BabelWires-Music into BabelWires.
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
#include <Domains/TestDomain/testGenericType.hpp>

#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Processors/processorFactoryRegistry.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

void testDomain::registerLib(babelwires::ProjectContext& context) {
    context.m_typeSystem.addType<testDomain::TestEnum>();
    context.m_typeSystem.addType<testDomain::TestSubEnum>();
    context.m_typeSystem.addType<testDomain::TestSubSubEnum1>();
    context.m_typeSystem.addType<testDomain::TestSubSubEnum2>();
    context.m_typeSystem.addType<testDomain::TestSimpleArrayType>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::TestCompoundArrayType>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::TestSimpleRecordType>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::TestComplexRecordType>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::TestRecordWithVariantsType>(context.m_typeSystem);    
    context.m_typeSystem.addType<testDomain::TestTupleType>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordWithNoFields>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::TestSumType>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::TestGenericType>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::TestSimpleCompoundType>(context.m_typeSystem);

    context.m_typeSystem.addType<testDomain::RecordA0>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordA1>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordAS>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordB>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordAB>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordAOpt>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordAOptFixed>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordABOpt>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordAOptS>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordABOptChild>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordAsub0>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordAsubBsup>(context.m_typeSystem);

    context.m_typeSystem.addType<testDomain::RecordVWithNoFields>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordVA0>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordVA1>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordVAS>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordVB>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordVAB>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordVAV0>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordVABV0>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordVABV1>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordVABV01>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::RecordVAVB>(context.m_typeSystem);

    context.m_typeSystem.addType<testDomain::TestParallelProcessorInput>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::TestParallelProcessorOutput>(context.m_typeSystem);
    context.m_typeSystem.addType<testDomain::TestProcessorInputOutputType>(context.m_typeSystem);

    context.m_processorReg.addProcessor<TestProcessor>();
    context.m_processorReg.addProcessor<TestParallelProcessor>();

    context.m_targetFileFormatReg.addEntry<TestTargetFileFormat>();
    context.m_sourceFileFormatReg.addEntry<TestSourceFileFormat>();
}
