#include <Tests/BabelWiresLib/TestUtils/testProcessor.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecord.hpp>

#include <BabelWiresLib/Features/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>

testUtils::TestProcessorInputOutputType::TestProcessorInputOutputType()
    : RecordType({{BW_SHORT_ID("Int", "Int", "aa45c937-c9b0-4736-839f-1060c9c944ec"),
                   babelwires::DefaultIntType::getThisIdentifier()},
                  {BW_SHORT_ID("OptInt", "OptInt", "a8b6b36b-a8f3-44d8-a685-a29108831ff3"),
                   babelwires::DefaultIntType::getThisIdentifier(), Optionality::optionalDefaultInactive},
                  {BW_SHORT_ID("Array", "Array", "35a1a000-9653-4fa0-b58a-b70a910a1c5c"),
                   babelwires::TypeRef{babelwires::ArrayTypeConstructor::getThisIdentifier(),
                                       {{babelwires::DefaultIntType::getThisIdentifier()},
                                        {babelwires::IntValue(2), babelwires::IntValue(8), babelwires::IntValue(2)}}}},
                  {BW_SHORT_ID("Rec", "Rec", "3968dfc5-b193-4339-8cb9-5069e693008a"),
                   TestSimpleRecordType::getThisIdentifier()}}) {}

testUtils::TestProcessor2::TestProcessor2(const babelwires::ProjectContext& context)
    : babelwires::ValueProcessor(context, testUtils::TestProcessorInputOutputType::getThisIdentifier(),
                                 testUtils::TestProcessorInputOutputType::getThisIdentifier()) {}

void testUtils::TestProcessor2::processValue(babelwires::UserLogger& userLogger,
                                             const babelwires::ValueFeature& inputFeature,
                                             babelwires::ValueFeature& outputFeature) const {
    TestProcessorInputOutputType::ConstInstance input{inputFeature};
    TestProcessorInputOutputType::Instance output{outputFeature};

    output.getInt().set(input.getInt().get());
    output.getRec()->setValue(input.getRec()->getValue());
    if (input.tryGetOpInt()) {
        output.activateAndGetOpInt().set(input.tryGetOpInt()->get());
    } else {
        output.deactivateOpInt();
    }
    output.getArray().setSize(2 + input.getInt().get());
    for (int i = 0; i < input.getInt().get(); ++i) {
        output.getArray().getEntry(i).set(i);
    }
}

testUtils::TestProcessor::TestProcessor(const babelwires::ProjectContext& context) {
    m_inputFeature = std::make_unique<TestRootFeature>(context);
    m_outputFeature = std::make_unique<TestRootFeature>(context);
}

void testUtils::TestProcessor::process(babelwires::UserLogger& userLogger) {
    m_outputFeature->m_arrayFeature->setSize(2 + m_inputFeature->m_intFeature->get());
    m_outputFeature->m_intFeature2->set(m_inputFeature->m_intFeature2->get());
    int index = m_inputFeature->m_intFeature->get();
    for (auto c : babelwires::subfeatures(m_outputFeature->m_arrayFeature)) {
        c->as<babelwires::IntFeature>()->set(index);
        ++index;
    }
}

babelwires::RootFeature* testUtils::TestProcessor::getInputFeature() {
    return m_inputFeature.get();
}

babelwires::RootFeature* testUtils::TestProcessor::getOutputFeature() {
    return m_outputFeature.get();
}

testUtils::TestProcessorFactory::TestProcessorFactory()
    : ProcessorFactory(babelwires::IdentifierRegistry::write()->addLongIdWithMetadata(
                           "testProcessor", "Test processor", "714b6684-ad20-43e6-abda-c0d308586bf4",
                           babelwires::IdentifierRegistry::Authority::isAuthoritative),
                       2) {}

babelwires::LongId testUtils::TestProcessorFactory::getThisIdentifier() {
    return "testProcessor";
}

std::unique_ptr<babelwires::Processor>
testUtils::TestProcessorFactory::createNewProcessor(const babelwires::ProjectContext& projectContext) const {
    return std::make_unique<TestProcessor>(projectContext);
}
