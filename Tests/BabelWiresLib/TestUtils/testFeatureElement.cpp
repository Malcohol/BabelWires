#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>

#include <BabelWiresLib/Project/FeatureElements/failedFeature.hpp>

testUtils::TestFailedFeature::TestFailedFeature(const babelwires::ProjectContext& context)
    : RootFeature(context) {
    addField(std::make_unique<RecordFeature>(), babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
                                                    "Failed", "Failed", "00000000-1111-2222-3333-800000000888",
                                                    babelwires::IdentifierRegistry::Authority::isAuthoritative));
}

testUtils::TestFeatureElementData::TestFeatureElementData() {
    m_factoryIdentifier = "TestFactory";
    m_factoryVersion = 1;
}

testUtils::TestFeatureElementData::TestFeatureElementData(const TestFeatureElementData& other,
                                                             babelwires::ShallowCloneContext context)
    : ElementData(other, context)
    , m_intValueLimit(other.m_intValueLimit) {}

bool testUtils::TestFeatureElementData::checkFactoryVersion(const babelwires::ProjectContext& context,
                                                               babelwires::UserLogger& userLogger) {
    return true;
}

std::unique_ptr<babelwires::FeatureElement> testUtils::TestFeatureElementData::doCreateFeatureElement(
    const babelwires::ProjectContext& context, babelwires::UserLogger& userLogger, babelwires::ElementId newId) const {
    return std::make_unique<TestFeatureElement>(context, *this, newId);
}

void testUtils::TestFeatureElementData::serializeContents(babelwires::Serializer& serializer) const {}

void testUtils::TestFeatureElementData::deserializeContents(babelwires::Deserializer& deserializer) {}

testUtils::TestFeatureElement::TestFeatureElement(const babelwires::ProjectContext& context)
    : TestFeatureElement(context, TestFeatureElementData(), 10) {}

testUtils::TestFeatureElement::TestFeatureElement(const babelwires::ProjectContext& context,
                                                     const TestFeatureElementData& data, babelwires::ElementId newId)
    : FeatureElement(data, newId) {
    setFactoryName(data.m_factoryIdentifier);
    m_feature = std::make_unique<TestRootFeature>(
        context, static_cast<const TestFeatureElementData*>(&getElementData())->m_intValueLimit);
}

void testUtils::TestFeatureElement::doProcess(babelwires::UserLogger&) {}

const babelwires::RootFeature* testUtils::TestFeatureElement::getInputFeature() const {
    return m_feature.get();
}

const babelwires::RootFeature* testUtils::TestFeatureElement::getOutputFeature() const {
    return m_feature.get();
}

babelwires::RootFeature* testUtils::TestFeatureElement::getInputFeatureNonConst() {
    return m_feature.get();
}

babelwires::RootFeature* testUtils::TestFeatureElement::getOutputFeatureNonConst() {
    return m_feature.get();
}

void testUtils::TestFeatureElement::simulateFailure(const babelwires::ProjectContext& context) {
    setInternalFailure("Simulated failure");
    m_feature = std::make_unique<TestFailedFeature>(context);
}

void testUtils::TestFeatureElement::simulateRecovery(const babelwires::ProjectContext& context) {
    m_feature = std::make_unique<TestRootFeature>(
        context, static_cast<const TestFeatureElementData*>(&getElementData())->m_intValueLimit);
    clearInternalFailure();
}
