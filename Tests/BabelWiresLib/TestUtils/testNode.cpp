#include <Tests/BabelWiresLib/TestUtils/testNode.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <BaseLib/Context/context.hpp>

#include <Domains/TestDomain/testRecordType.hpp>

testUtils::TestNodeData::TestNodeData() {
    m_factoryIdentifier = "TestFactory";
    m_factoryVersion = 1;
}

testUtils::TestNodeData::TestNodeData(const TestNodeData& other, babelwires::ShallowCloneContext context)
    : NodeData(other, context) {}

bool testUtils::TestNodeData::checkFactoryVersion(const babelwires::Context& context,
                                                  babelwires::UserLogger& userLogger) {
    return true;
}

std::unique_ptr<babelwires::Node> testUtils::TestNodeData::doCreateNode(const babelwires::Context& context,
                                                                        babelwires::UserLogger& userLogger,
                                                                        babelwires::NodeId newId) const {
    return std::make_unique<TestNode>(context, *this, newId);
}

void testUtils::TestNodeData::serializeContents(babelwires::Serializer& serializer) const {}

babelwires::Result testUtils::TestNodeData::deserializeContents(babelwires::Deserializer& deserializer) {
    return {};
}

testUtils::TestNode::TestNode(const babelwires::Context& context)
    : TestNode(context, TestNodeData(), 10) {}

testUtils::TestNode::TestNode(const babelwires::Context& context, const TestNodeData& data,
                              babelwires::NodeId newId)
    : Node(data, newId) {
    setFactoryName(data.m_factoryIdentifier);
    m_valueTreeRoot = std::make_unique<babelwires::ValueTreeRoot>(
        context.get<babelwires::TypeSystem>(), context.get<babelwires::TypeSystem>().getRegisteredType<testDomain::TestComplexRecordType>());
}

void testUtils::TestNode::doProcess(babelwires::UserLogger&) {}

const babelwires::ValueTreeNode* testUtils::TestNode::getInput() const {
    return m_valueTreeRoot.get();
}

const babelwires::ValueTreeNode* testUtils::TestNode::getOutput() const {
    return m_valueTreeRoot.get();
}

babelwires::ValueTreeNode* testUtils::TestNode::doGetInputNonConst() {
    return m_valueTreeRoot.get();
}

babelwires::ValueTreeNode* testUtils::TestNode::doGetOutputNonConst() {
    return m_valueTreeRoot.get();
}

void testUtils::TestNode::simulateFailure(const babelwires::Context& context) {
    setInternalFailure("Simulated failure");
    m_valueTreeRoot = std::make_unique<babelwires::ValueTreeRoot>(
        context.get<babelwires::TypeSystem>(), context.get<babelwires::TypeSystem>().getRegisteredType<babelwires::FailureType>());
}

void testUtils::TestNode::simulateRecovery(const babelwires::Context& context) {
    m_valueTreeRoot = std::make_unique<babelwires::ValueTreeRoot>(
        context.get<babelwires::TypeSystem>(), context.get<babelwires::TypeSystem>().getRegisteredType<testDomain::TestComplexRecordType>());
    clearInternalFailure();
}
