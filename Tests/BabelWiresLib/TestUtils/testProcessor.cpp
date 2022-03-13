#include "Tests/BabelWiresLib/TestUtils/testProcessor.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

#include "BabelWiresLib/Features/Utilities/modelUtilities.hpp"

#include "BabelWiresLib/Features/featureMixins.hpp"

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
    : ProcessorFactory(babelwires::IdentifierRegistry::write()->addLongIdentifierWithMetadata(
                           "testProcessor", "Test processor", "714b6684-ad20-43e6-abda-c0d308586bf4",
                           babelwires::IdentifierRegistry::Authority::isAuthoritative),
                       2) {}

babelwires::LongIdentifier testUtils::TestProcessorFactory::getThisIdentifier() {
    return "testProcessor";
}

std::unique_ptr<babelwires::Processor> testUtils::TestProcessorFactory::createNewProcessor(const babelwires::ProjectContext& projectContext) const {
    return std::make_unique<TestProcessor>(projectContext);
}
