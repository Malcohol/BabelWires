#include <gtest/gtest.h>

#include <BabelWiresLib/Features/valueTreeNode.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>

#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(ValueElementTest, simpleType) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueAssignmentData assignmentData(babelwires::EditableValueHolder::makeValue<babelwires::IntValue>(-4));

    babelwires::ValueElementData data(babelwires::DefaultIntType::getThisIdentifier());
    data.m_modifiers.emplace_back(assignmentData.clone());

    auto newElement = data.createFeatureElement(testEnvironment.m_projectContext, testEnvironment.m_log, 10);

    ASSERT_NE(newElement, nullptr);
    babelwires::ValueElement* const valueElement = newElement->as<babelwires::ValueElement>();
    ASSERT_NE(valueElement, nullptr);

    const babelwires::ValueElementData* const valueElementData =
        valueElement->getElementData().as<babelwires::ValueElementData>();
    ASSERT_NE(valueElementData, nullptr);
    EXPECT_EQ(valueElement->getElementData().m_id, 10);
    EXPECT_EQ(valueElementData->getTypeRef(), babelwires::DefaultIntType::getThisIdentifier());

    const babelwires::ValueTreeNode* const inputFeature = valueElement->getInputFeature();
    const babelwires::ValueTreeNode* const outputFeature = valueElement->getOutputFeature();

    ASSERT_NE(inputFeature, nullptr);
    ASSERT_NE(outputFeature, nullptr);
    EXPECT_EQ(inputFeature, outputFeature);

    EXPECT_EQ(inputFeature->getTypeRef(), babelwires::DefaultIntType::getThisIdentifier());
    const babelwires::ValueHolder value = inputFeature->getValue();
    const babelwires::IntValue* intValue = value->as<babelwires::IntValue>();
    ASSERT_NE(intValue, nullptr);
    EXPECT_EQ(intValue->get(), -4);
}

TEST(ValueElementTest, valueElementDataSerialization) {
    testUtils::TestEnvironment testEnvironment;

    std::string serializedContents;
    {
        babelwires::ValueElementData data(babelwires::DefaultIntType::getThisIdentifier());
        data.m_id = 2;
        data.m_uiData.m_uiSize.m_width = 300;
        data.m_modifiers.emplace_back(std::make_unique<babelwires::ValueAssignmentData>(
            babelwires::EditableValueHolder::makeValue<babelwires::IntValue>(12)));

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
    auto dataPtr = deserializer.deserializeObject<babelwires::ValueElementData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_id, 2);
    EXPECT_EQ(dataPtr->m_uiData.m_uiSize.m_width, 300);
    EXPECT_EQ(dataPtr->getTypeRef(), babelwires::DefaultIntType::getThisIdentifier());
    ASSERT_EQ(dataPtr->m_modifiers.size(), 1);
    const babelwires::ValueAssignmentData* const assignmentData =
        dataPtr->m_modifiers[0]->as<babelwires::ValueAssignmentData>();
    ASSERT_NE(assignmentData, nullptr);
    const babelwires::EditableValueHolder value = assignmentData->getValue();
    const babelwires::IntValue* intValue = value->as<babelwires::IntValue>();
    ASSERT_NE(intValue, nullptr);
    EXPECT_EQ(intValue->get(), 12);
}
