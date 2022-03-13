#include "Tests/BabelWiresLib/TestUtils/testProcessor.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

#include "BabelWiresLib/Features/Utilities/modelUtilities.hpp"

#include "BabelWiresLib/Features/featureMixins.hpp"

libTestUtils::TestProcessor::TestProcessor(const babelwires::ProjectContext& context) {
    m_inputFeature = std::make_unique<TestRecordFeature>();
    m_outputFeature = std::make_unique<TestRecordFeature>();
}

void libTestUtils::TestProcessor::process(babelwires::UserLogger& userLogger) {
    m_outputFeature->m_arrayFeature->setSize(2 + m_inputFeature->m_intFeature->get());
    m_outputFeature->m_intFeature2->set(m_inputFeature->m_intFeature2->get());
    int index = m_inputFeature->m_intFeature->get();
    for (auto c : babelwires::subfeatures(m_outputFeature->m_arrayFeature)) {
        c->as<babelwires::IntFeature>()->set(index);
        ++index;
    }
}

babelwires::RootFeature* libTestUtils::TestProcessor::getInputFeature() {
    return m_inputFeature.get();
}

babelwires::RootFeature* libTestUtils::TestProcessor::getOutputFeature() {
    return m_outputFeature.get();
}

libTestUtils::TestProcessorFactory::TestProcessorFactory()
    : ProcessorFactory(babelwires::IdentifierRegistry::write()->addLongIdentifierWithMetadata(
                           "testProcessor", "Test processor", "714b6684-ad20-43e6-abda-c0d308586bf4",
                           babelwires::IdentifierRegistry::Authority::isAuthoritative),
                       2) {}

babelwires::LongIdentifier libTestUtils::TestProcessorFactory::getThisIdentifier() {
    return "testProcessor";
}

std::unique_ptr<babelwires::Processor> libTestUtils::TestProcessorFactory::createNewProcessor(const babelwires::ProjectContext& projectContext) const {
    return std::make_unique<TestProcessor>(projectContext);
}
