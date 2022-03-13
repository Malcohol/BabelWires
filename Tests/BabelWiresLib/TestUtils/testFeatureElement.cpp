#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"

#include "BabelWiresLib/Project/FeatureElements/failedFeature.hpp"

libTestUtils::TestFailedFeature::TestFailedFeature(const babelwires::ProjectContext& context)
    : RootFeature(context) {
    addField(std::make_unique<RecordFeature>(), babelwires::IdentifierRegistry::write()->addShortIdentifierWithMetadata(
                                                    "Failed", "Failed", "00000000-1111-2222-3333-800000000888",
                                                    babelwires::IdentifierRegistry::Authority::isAuthoritative));
}

libTestUtils::TestFeatureElementData::TestFeatureElementData() {
    m_factoryIdentifier = "TestFactory";
    m_factoryVersion = 1;
}

libTestUtils::TestFeatureElementData::TestFeatureElementData(const TestFeatureElementData& other,
                                                             babelwires::ShallowCloneContext context)
    : ElementData(other, context)
    , m_intValueLimit(other.m_intValueLimit) {}

bool libTestUtils::TestFeatureElementData::checkFactoryVersion(const babelwires::ProjectContext& context,
                                                               babelwires::UserLogger& userLogger) {
    return true;
}

std::unique_ptr<babelwires::FeatureElement> libTestUtils::TestFeatureElementData::doCreateFeatureElement(
    const babelwires::ProjectContext& context, babelwires::UserLogger& userLogger, babelwires::ElementId newId) const {
    return std::make_unique<TestFeatureElement>(context, *this, newId);
}

void libTestUtils::TestFeatureElementData::serializeContents(babelwires::Serializer& serializer) const {}

void libTestUtils::TestFeatureElementData::deserializeContents(babelwires::Deserializer& deserializer) {}

libTestUtils::TestFeatureElement::TestFeatureElement(const babelwires::ProjectContext& context)
    : TestFeatureElement(context, TestFeatureElementData(), 10) {}

libTestUtils::TestFeatureElement::TestFeatureElement(const babelwires::ProjectContext& context,
                                                     const TestFeatureElementData& data, babelwires::ElementId newId)
    : FeatureElement(data, newId) {
    setFactoryName(data.m_factoryIdentifier);
    m_feature = std::make_unique<TestRootFeature>(
        context, static_cast<const TestFeatureElementData*>(&getElementData())->m_intValueLimit);
}

void libTestUtils::TestFeatureElement::doProcess(babelwires::UserLogger&) {}

babelwires::RootFeature* libTestUtils::TestFeatureElement::getInputFeature() {
    return m_feature.get();
}

babelwires::RootFeature* libTestUtils::TestFeatureElement::getOutputFeature() {
    return m_feature.get();
}

void libTestUtils::TestFeatureElement::simulateFailure(const babelwires::ProjectContext& context) {
    setInternalFailure("Simulated failure");
    m_feature = std::make_unique<TestFailedFeature>(context);
}

void libTestUtils::TestFeatureElement::simulateRecovery(const babelwires::ProjectContext& context) {
    m_feature = std::make_unique<TestRootFeature>(
        context, static_cast<const TestFeatureElementData*>(&getElementData())->m_intValueLimit);
    clearInternalFailure();
}
