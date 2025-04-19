#include <BabelWiresLib/Instance/instance.hpp>
#include <BabelWiresLib/Processors/parallelProcessor.hpp>
#include <BabelWiresLib/Processors/processor.hpp>
#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>

#include <Domains/TestDomain/testRecordType.hpp>

namespace testDomain {

    class TestParallelProcessorInput : public babelwires::ParallelProcessorInputBase {
      public:
        PRIMITIVE_TYPE("TestProcIn", "TestProcIn", "2b414a3b-fe41-41dc-894f-889e8e15f0ff", 1);

        TestParallelProcessorInput();
    };

    class TestParallelProcessorOutput : public babelwires::ParallelProcessorOutputBase {
      public:
        PRIMITIVE_TYPE("TestProcOut", "TestProcOut", "b61c5cf6-cada-416e-8fca-57f360c346e9", 1);

        TestParallelProcessorOutput();
    };

    struct TestParallelProcessor : babelwires::ParallelProcessor {
        BW_PROCESSOR_WITH_DEFAULT_FACTORY("testParallelProc", "Test Parallel Processor", "d993345a-4127-41e6-b090-33848f4c5da2");

        TestParallelProcessor(const babelwires::ProjectContext& context);

        static babelwires::ShortId getCommonArrayId();

        void processEntry(babelwires::UserLogger& userLogger, const babelwires::ValueTreeNode& input,
                          const babelwires::ValueTreeNode& inputEntry,
                          babelwires::ValueTreeNode& outputEntry) const override;
    };
} // namespace testDomain
