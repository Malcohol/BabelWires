#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>

#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

testUtils::TestFeatureElementData::TestFeatureElementData() {
    m_factoryIdentifier = "TestFactory";
    m_factoryVersion = 1;
}

testUtils::TestFeatureElementData::TestFeatureElementData(const TestFeatureElementData& other,
                                                          babelwires::ShallowCloneContext context)
    : NodeData(other, context) {}

bool testUtils::TestFeatureElementData::checkFactoryVersion(const babelwires::ProjectContext& context,
                                                            babelwires::UserLogger& userLogger) {
    return true;
}

std::unique_ptr<babelwires::Node> testUtils::TestFeatureElementData::doCreateFeatureElement(
    const babelwires::ProjectContext& context, babelwires::UserLogger& userLogger, babelwires::ElementId newId) const {
    return std::make_unique<TestFeatureElement>(context, *this, newId);
}

void testUtils::TestFeatureElementData::serializeContents(babelwires::Serializer& serializer) const {}

void testUtils::TestFeatureElementData::deserializeContents(babelwires::Deserializer& deserializer) {}

testUtils::TestFeatureElement::TestFeatureElement(const babelwires::ProjectContext& context)
    : TestFeatureElement(context, TestFeatureElementData(), 10) {}

testUtils::TestFeatureElement::TestFeatureElement(const babelwires::ProjectContext& context,
                                                  const TestFeatureElementData& data, babelwires::ElementId newId)
    : Node(data, newId) {
    setFactoryName(data.m_factoryIdentifier);
    m_valueTreeRoot = std::make_unique<babelwires::ValueTreeRoot>(context.m_typeSystem, TestComplexRecordType::getThisType());
}

void testUtils::TestFeatureElement::doProcess(babelwires::UserLogger&) {}

const babelwires::ValueTreeNode* testUtils::TestFeatureElement::getInput() const {
    return m_valueTreeRoot.get();
}

const babelwires::ValueTreeNode* testUtils::TestFeatureElement::getOutput() const {
    return m_valueTreeRoot.get();
}

babelwires::ValueTreeNode* testUtils::TestFeatureElement::doGetInputNonConst() {
    return m_valueTreeRoot.get();
}

babelwires::ValueTreeNode* testUtils::TestFeatureElement::doGetOutputNonConst() {
    return m_valueTreeRoot.get();
}

void testUtils::TestFeatureElement::simulateFailure(const babelwires::ProjectContext& context) {
    setInternalFailure("Simulated failure");
    m_valueTreeRoot = std::make_unique<babelwires::ValueTreeRoot>(context.m_typeSystem,
                                                                 babelwires::FailureType::getThisType());
}

void testUtils::TestFeatureElement::simulateRecovery(const babelwires::ProjectContext& context) {
    m_valueTreeRoot = std::make_unique<babelwires::ValueTreeRoot>(context.m_typeSystem, TestComplexRecordType::getThisType());
    clearInternalFailure();
}
