#include "BabelWiresLib/Features/numericFeature.hpp"
#include "BabelWiresLib/Processors/processor.hpp"
#include "BabelWiresLib/Processors/processorFactory.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElementData.hpp"

namespace libTestUtils {
    struct TestRecordFeature;

    /// Carries a TestRecord for input and output.
    /// The effect of the processor ensures:
    /// * the array of the output has a size equal to the int at "int0" in the input.
    /// * the array elements have values counting up from input "int0".
    /// * the int at "int" in the output equals to the int at "int" in the input.
    // TODO The inconsistent sizing of the output and input arrays may violate assumptions
    // about adding and removing array elements via the project. It might be better if
    // the input had no array at all.
    struct TestProcessor : babelwires::Processor {
        TestProcessor();

        void process(babelwires::UserLogger& userLogger) override;
        babelwires::RecordFeature* getInputFeature() override;
        babelwires::RecordFeature* getOutputFeature() override;
        std::unique_ptr<TestRecordFeature> m_inputFeature;
        std::unique_ptr<TestRecordFeature> m_outputFeature;
    };

    /// Constructs test processors.
    /// This is given version 2, to allow version testing.
    struct TestProcessorFactory : babelwires::ProcessorFactory {
        TestProcessorFactory();

        static std::string getThisIdentifier();

        std::unique_ptr<babelwires::Processor> createNewProcessor() const override;
    };

} // namespace libTestUtils
