#include <gtest/gtest.h>

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>

#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(ValueNodeTest, simpleType) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueAssignmentData assignmentData(babelwires::ValueHolder::makeValue<babelwires::IntValue>(-4));

    babelwires::ValueNodeData data(babelwires::DefaultIntType::getThisType());
    data.m_modifiers.emplace_back(assignmentData.clone());

    auto newElement = data.createNode(testEnvironment.m_projectContext, testEnvironment.m_log, 10);

    ASSERT_NE(newElement, nullptr);
    babelwires::ValueNode* const valueNode = newElement->as<babelwires::ValueNode>();
    ASSERT_NE(valueNode, nullptr);

    const babelwires::ValueNodeData* const valueNodeData =
        valueNode->getNodeData().as<babelwires::ValueNodeData>();
    ASSERT_NE(valueNodeData, nullptr);
    EXPECT_EQ(valueNode->getNodeData().m_id, 10);
    EXPECT_EQ(valueNodeData->getTypeExp(), babelwires::DefaultIntType::getThisType());

    const babelwires::ValueTreeNode* const input = valueNode->getInput();
    const babelwires::ValueTreeNode* const output = valueNode->getOutput();

    ASSERT_NE(input, nullptr);
    ASSERT_NE(output, nullptr);
    EXPECT_EQ(input, output);

    EXPECT_EQ(input->getTypeExp(), babelwires::DefaultIntType::getThisType());
    const babelwires::ValueHolder value = input->getValue();
    const babelwires::IntValue* intValue = value->as<babelwires::IntValue>();
    ASSERT_NE(intValue, nullptr);
    EXPECT_EQ(intValue->get(), -4);
}

TEST(ValueNodeTest, valueNodeDataSerialization) {
    testUtils::TestEnvironment testEnvironment;

    std::string serializedContents;
    {
        babelwires::ValueNodeData data(babelwires::DefaultIntType::getThisType());
        data.m_id = 2;
        data.m_uiData.m_uiSize.m_width = 300;
        data.m_modifiers.emplace_back(std::make_unique<babelwires::ValueAssignmentData>(
            babelwires::ValueHolder::makeValue<babelwires::IntValue>(12)));

        // Note: We want to be able to serialize when entries do not match the types, as in this case.
        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::ValueNodeData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_id, 2);
    EXPECT_EQ(dataPtr->m_uiData.m_uiSize.m_width, 300);
    EXPECT_EQ(dataPtr->getTypeExp(), babelwires::DefaultIntType::getThisType());
    ASSERT_EQ(dataPtr->m_modifiers.size(), 1);
    const babelwires::ValueAssignmentData* const assignmentData =
        dataPtr->m_modifiers[0]->as<babelwires::ValueAssignmentData>();
    ASSERT_NE(assignmentData, nullptr);
    const babelwires::ValueHolder value = assignmentData->getValue();
    const babelwires::IntValue* intValue = value->as<babelwires::IntValue>();
    ASSERT_NE(intValue, nullptr);
    EXPECT_EQ(intValue->get(), 12);
}
