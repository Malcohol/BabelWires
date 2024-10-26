#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>

#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

testUtils::TestFeatureElementData::TestFeatureElementData() {
    m_factoryIdentifier = "TestFactory";
    m_factoryVersion = 1;
}

testUtils::TestFeatureElementData::TestFeatureElementData(const TestFeatureElementData& other,
                                                          babelwires::ShallowCloneContext context)
    : ElementData(other, context) {}

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
    m_feature = std::make_unique<babelwires::ValueTreeRoot>(context.m_typeSystem, TestComplexRecordType::getThisIdentifier());
}

void testUtils::TestFeatureElement::doProcess(babelwires::UserLogger&) {}

const babelwires::ValueTreeNode* testUtils::TestFeatureElement::getInputFeature() const {
    return m_feature.get();
}

const babelwires::ValueTreeNode* testUtils::TestFeatureElement::getOutputFeature() const {
    return m_feature.get();
}

babelwires::ValueTreeNode* testUtils::TestFeatureElement::doGetInputFeatureNonConst() {
    return m_feature.get();
}

babelwires::ValueTreeNode* testUtils::TestFeatureElement::doGetOutputFeatureNonConst() {
    return m_feature.get();
}

void testUtils::TestFeatureElement::simulateFailure(const babelwires::ProjectContext& context) {
    setInternalFailure("Simulated failure");
    m_feature = std::make_unique<babelwires::ValueTreeRoot>(context.m_typeSystem,
                                                                 babelwires::FailureType::getThisIdentifier());
}

void testUtils::TestFeatureElement::simulateRecovery(const babelwires::ProjectContext& context) {
    m_feature = std::make_unique<babelwires::ValueTreeRoot>(context.m_typeSystem, TestComplexRecordType::getThisIdentifier());
    clearInternalFailure();
}
