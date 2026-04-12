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
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <BaseLib/Context/context.hpp>

void testDomain::registerLib(babelwires::Context& context) {
    babelwires::TypeSystem& typeSystem = context.get<babelwires::TypeSystem>();
    typeSystem.addType<testDomain::TestEnum>();
    typeSystem.addType<testDomain::TestSubEnum>();
    typeSystem.addType<testDomain::TestSubSubEnum1>();
    typeSystem.addType<testDomain::TestSubSubEnum2>();
    typeSystem.addType<testDomain::TestSimpleArrayType>(typeSystem);
    typeSystem.addType<testDomain::TestCompoundArrayType>(typeSystem);
    typeSystem.addType<testDomain::TestSimpleRecordType>(typeSystem);
    typeSystem.addType<testDomain::TestComplexRecordType>(typeSystem);
    typeSystem.addType<testDomain::TestRecordWithVariantsType>(typeSystem);    
    typeSystem.addType<testDomain::TestTupleType>(typeSystem);
    typeSystem.addType<testDomain::RecordWithNoFields>(typeSystem);
    typeSystem.addType<testDomain::TestSumType>(typeSystem);
    typeSystem.addType<testDomain::TestGenericType>(typeSystem);
    typeSystem.addType<testDomain::TestSimpleCompoundType>(typeSystem);

    typeSystem.addType<testDomain::RecordA0>(typeSystem);
    typeSystem.addType<testDomain::RecordA1>(typeSystem);
    typeSystem.addType<testDomain::RecordAS>(typeSystem);
    typeSystem.addType<testDomain::RecordB>(typeSystem);
    typeSystem.addType<testDomain::RecordAB>(typeSystem);
    typeSystem.addType<testDomain::RecordAOpt>(typeSystem);
    typeSystem.addType<testDomain::RecordAOptFixed>(typeSystem);
    typeSystem.addType<testDomain::RecordABOpt>(typeSystem);
    typeSystem.addType<testDomain::RecordAOptS>(typeSystem);
    typeSystem.addType<testDomain::RecordABOptChild>(typeSystem);
    typeSystem.addType<testDomain::RecordAsub0>(typeSystem);
    typeSystem.addType<testDomain::RecordAsubBsup>(typeSystem);

    typeSystem.addType<testDomain::RecordVWithNoFields>(typeSystem);
    typeSystem.addType<testDomain::RecordVA0>(typeSystem);
    typeSystem.addType<testDomain::RecordVA1>(typeSystem);
    typeSystem.addType<testDomain::RecordVAS>(typeSystem);
    typeSystem.addType<testDomain::RecordVB>(typeSystem);
    typeSystem.addType<testDomain::RecordVAB>(typeSystem);
    typeSystem.addType<testDomain::RecordVAV0>(typeSystem);
    typeSystem.addType<testDomain::RecordVABV0>(typeSystem);
    typeSystem.addType<testDomain::RecordVABV1>(typeSystem);
    typeSystem.addType<testDomain::RecordVABV01>(typeSystem);
    typeSystem.addType<testDomain::RecordVAVB>(typeSystem);

    typeSystem.addType<testDomain::TestParallelProcessorInput>(typeSystem);
    typeSystem.addType<testDomain::TestParallelProcessorOutput>(typeSystem);
    typeSystem.addType<testDomain::TestProcessorInputOutputType>(typeSystem);

    babelwires::ProcessorFactoryRegistry& processorFactoryRegistry = context.get<babelwires::ProcessorFactoryRegistry>();
    processorFactoryRegistry.addProcessor<TestProcessor>();
    processorFactoryRegistry.addProcessor<TestParallelProcessor>();

    context.get<babelwires::TargetFileFormatRegistry>().addEntry<TestTargetFileFormat>();
    context.get<babelwires::SourceFileFormatRegistry>().addEntry<TestSourceFileFormat>();
}
