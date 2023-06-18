#include <gtest/gtest.h>

#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/valueElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/valueElementData.hpp>
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
    assignmentData.m_pathToFeature.pushStep(babelwires::PathStep("value"));

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

    const babelwires::RootFeature* const inputFeature = valueElement->getInputFeature();
    const babelwires::RootFeature* const outputFeature = valueElement->getOutputFeature();

    ASSERT_NE(inputFeature, nullptr);
    ASSERT_NE(outputFeature, nullptr);
    EXPECT_EQ(inputFeature, outputFeature);

    ASSERT_EQ(inputFeature->getNumFeatures(), 1);

    const babelwires::Feature* const child = inputFeature->getFeature(0);
    ASSERT_NE(child, nullptr);
    const babelwires::ValueFeature* const childAsValue = child->as<babelwires::ValueFeature>();
    ASSERT_NE(childAsValue, nullptr);

    EXPECT_EQ(childAsValue->getTypeRef(), babelwires::DefaultIntType::getThisIdentifier());
    const babelwires::ValueHolder value = childAsValue->getValue();
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
