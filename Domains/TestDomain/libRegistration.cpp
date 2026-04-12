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
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestEnum>();
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestSubEnum>();
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestSubSubEnum1>();
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestSubSubEnum2>();
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestSimpleArrayType>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestCompoundArrayType>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestSimpleRecordType>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestComplexRecordType>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestRecordWithVariantsType>(context.getService<babelwires::TypeSystem>());    
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestTupleType>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordWithNoFields>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestSumType>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestGenericType>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestSimpleCompoundType>(context.getService<babelwires::TypeSystem>());

    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordA0>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordA1>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordAS>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordB>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordAB>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordAOpt>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordAOptFixed>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordABOpt>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordAOptS>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordABOptChild>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordAsub0>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordAsubBsup>(context.getService<babelwires::TypeSystem>());

    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordVWithNoFields>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordVA0>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordVA1>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordVAS>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordVB>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordVAB>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordVAV0>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordVABV0>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordVABV1>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordVABV01>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::RecordVAVB>(context.getService<babelwires::TypeSystem>());

    context.getService<babelwires::TypeSystem>().addType<testDomain::TestParallelProcessorInput>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestParallelProcessorOutput>(context.getService<babelwires::TypeSystem>());
    context.getService<babelwires::TypeSystem>().addType<testDomain::TestProcessorInputOutputType>(context.getService<babelwires::TypeSystem>());

    context.getService<babelwires::ProcessorFactoryRegistry>().addProcessor<TestProcessor>();
    context.getService<babelwires::ProcessorFactoryRegistry>().addProcessor<TestParallelProcessor>();

    context.getService<babelwires::TargetFileFormatRegistry>().addEntry<TestTargetFileFormat>();
    context.getService<babelwires::SourceFileFormatRegistry>().addEntry<TestSourceFileFormat>();
}
