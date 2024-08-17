#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Processors/valueProcessor.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>
#include <BabelWiresLib/Instance/instance.hpp>

#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRootFeature.hpp>

namespace testUtils {
    struct TestRecordFeature;

    /// Carries a TestRecord for input and output.
    /// The effect of the processor ensures:
    /// * the array of the output has a size equal to the int at "int0" in the input.
    /// * the array elements have values counting up from input "int0".
    /// * the int at "int" in the output equals to the int at "int" in the input.
    // TODO The inconsistent sizing of the output and input arrays may violate assumptions
    // about adding and removing array elements via the project. It might be better if
    // the input had no array at all.

    class TestProcessorInputOutputType : public babelwires::RecordType {
      public:
        TestProcessorInputOutputType();

        PRIMITIVE_TYPE("TestProcInOut", "TestProcInputOutput", "a6dd948b-e9f4-4f03-bd5d-ecf2d608f026", 1);

        DECLARE_INSTANCE_BEGIN(TestProcessorInputOutputType)
        DECLARE_INSTANCE_FIELD(Int, babelwires::IntType)
        DECLARE_INSTANCE_FIELD_OPTIONAL(OpInt, babelwires::IntType)
        DECLARE_INSTANCE_ARRAY_FIELD(Array, babelwires::IntType)
        DECLARE_INSTANCE_FIELD(Rec, TestSimpleRecordType)
        DECLARE_INSTANCE_END()
    };

    struct TestProcessor2 : babelwires::ValueProcessor {
        BW_PROCESSOR_WITH_DEFAULT_FACTORY("TestProcessor2", "TestProcessor", "8ec4249a-dc7f-4cd5-931b-cc83aaf7287b");

        TestProcessor2(const babelwires::ProjectContext& context);

        void processValue(babelwires::UserLogger& userLogger, const babelwires::ValueFeature& inputFeature,
                          babelwires::ValueFeature& outputFeature) const override;
    };

    struct TestProcessor : babelwires::Processor {
        TestProcessor(const babelwires::ProjectContext& context);

        void process(babelwires::UserLogger& userLogger) override;
        babelwires::RootFeature* getInputFeature() override;
        babelwires::RootFeature* getOutputFeature() override;
        std::unique_ptr<TestRootFeature> m_inputFeature;
        std::unique_ptr<TestRootFeature> m_outputFeature;
    };

    /// Constructs test processors.
    /// This is given version 2, to allow version testing.
    struct TestProcessorFactory : babelwires::ProcessorFactory {
        TestProcessorFactory();

        static babelwires::LongId getThisIdentifier();

        std::unique_ptr<babelwires::Processor>
        createNewProcessor(const babelwires::ProjectContext& projectContext) const override;
    };

} // namespace testUtils
