#include <gtest/gtest.h>

#include "Tests/BabelWiresLib/TestUtils/testFeatureWithOptionals.hpp"

const babelwires::FeaturePath libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord =
    babelwires::FeaturePath::deserializeFromString(libTestUtils::TestFeatureWithOptionals::s_subrecordIdInitializer);
const babelwires::FeaturePath libTestUtils::TestFeatureWithOptionals::s_pathToFf0 =
    babelwires::FeaturePath::deserializeFromString(std::string(libTestUtils::TestFeatureWithOptionals::s_subrecordIdInitializer) + "/" + libTestUtils::TestFeatureWithOptionals::s_ff0IdInitializer);
const babelwires::FeaturePath libTestUtils::TestFeatureWithOptionals::s_pathToFf1 =
    babelwires::FeaturePath::deserializeFromString(std::string(libTestUtils::TestFeatureWithOptionals::s_subrecordIdInitializer) + "/" + libTestUtils::TestFeatureWithOptionals::s_ff1IdInitializer);
const babelwires::FeaturePath libTestUtils::TestFeatureWithOptionals::s_pathToOp0 =
    babelwires::FeaturePath::deserializeFromString(std::string(libTestUtils::TestFeatureWithOptionals::s_subrecordIdInitializer) + "/" + libTestUtils::TestFeatureWithOptionals::s_op0IdInitializer);
const babelwires::FeaturePath libTestUtils::TestFeatureWithOptionals::s_pathToOp1 =
    babelwires::FeaturePath::deserializeFromString(std::string(libTestUtils::TestFeatureWithOptionals::s_subrecordIdInitializer) + "/" + libTestUtils::TestFeatureWithOptionals::s_op1IdInitializer);

const babelwires::FeaturePath libTestUtils::TestFeatureWithOptionals::s_pathToOp1_Array_1 = babelwires::FeaturePath::deserializeFromString(std::string(libTestUtils::TestFeatureWithOptionals::s_subrecordIdInitializer) + "/" + libTestUtils::TestFeatureWithOptionals::s_op1IdInitializer + "/" + libTestUtils::TestRecordFeature::s_arrayIdInitializer + "/1");
const babelwires::FeaturePath libTestUtils::TestFeatureWithOptionals::s_pathToOp1_Int2 = babelwires::FeaturePath::deserializeFromString(std::string(libTestUtils::TestFeatureWithOptionals::s_subrecordIdInitializer) + "/" + libTestUtils::TestFeatureWithOptionals::s_op1IdInitializer + "/" + libTestUtils::TestRecordFeature::s_recordIdInitializer + "/" + libTestUtils::TestRecordFeature::s_int2IdInitializer);

libTestUtils::TestFeatureWithOptionals::TestFeatureWithOptionals()
    : m_subrecordId(babelwires::IdentifierRegistry::write()->addFieldName(
          s_subrecordIdInitializer, s_subrecordFieldName, s_subrecordUuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_ff0Id(babelwires::IdentifierRegistry::write()->addFieldName(
          s_ff0IdInitializer, s_ff0FieldName, s_ff0Uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_ff1Id(babelwires::IdentifierRegistry::write()->addFieldName(
          s_ff1IdInitializer, s_ff1FieldName, s_ff1Uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_op0Id(babelwires::IdentifierRegistry::write()->addFieldName(
          s_op0IdInitializer, s_op0FieldName, s_op0Uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_op1Id(babelwires::IdentifierRegistry::write()->addFieldName(
          s_op1IdInitializer, s_op1FieldName, s_op1Uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative)) {
    {
        auto testRecordWithOptionalsFeaturePtr = std::make_unique<babelwires::RecordWithOptionalsFeature>();
        m_subrecord = testRecordWithOptionalsFeaturePtr.get();
        addField(std::move(testRecordWithOptionalsFeaturePtr), m_subrecordId);
    }
    {
        auto intFeaturePtr = std::make_unique<babelwires::IntFeature>();
        m_ff0Feature = intFeaturePtr.get();
        m_subrecord->addField(std::move(intFeaturePtr), m_ff0Id);
    }
    {
        auto intFeaturePtr = std::make_unique<babelwires::IntFeature>();
        m_op0Feature = intFeaturePtr.get();
        m_subrecord->addOptionalField(std::move(intFeaturePtr), m_op0Id);
    }
    {
        auto testRecordFeaturePtr = std::make_unique<libTestUtils::TestRecordFeature>();
        m_ff1Feature = testRecordFeaturePtr.get();
        m_subrecord->addField(std::move(testRecordFeaturePtr), m_ff1Id);
    }
    {
        auto testRecordFeaturePtr = std::make_unique<libTestUtils::TestRecordFeature>();
        m_op1Feature = testRecordFeaturePtr.get();
        m_subrecord->addOptionalField(std::move(testRecordFeaturePtr), m_op1Id);
    }
}

libTestUtils::TestFeatureElementWithOptionalsData::TestFeatureElementWithOptionalsData() {
    m_factoryIdentifier = "TestFeatureElementWithOptionalsFactory";
    m_factoryVersion = 1;
}

libTestUtils::TestFeatureElementWithOptionalsData::TestFeatureElementWithOptionalsData(const TestFeatureElementWithOptionalsData& other,
                                                             babelwires::ShallowCloneContext context)
    : ElementData(other, context)
    {}

bool libTestUtils::TestFeatureElementWithOptionalsData::checkFactoryVersion(const babelwires::ProjectContext& context,
                                                               babelwires::UserLogger& userLogger) {
    return true;
}

std::unique_ptr<babelwires::FeatureElement> libTestUtils::TestFeatureElementWithOptionalsData::doCreateFeatureElement(
    const babelwires::ProjectContext& context, babelwires::UserLogger& userLogger, babelwires::ElementId newId) const {
    return std::make_unique<TestFeatureElementWithOptionals>(*this, newId);
}

void libTestUtils::TestFeatureElementWithOptionalsData::serializeContents(babelwires::Serializer& serializer) const {}

void libTestUtils::TestFeatureElementWithOptionalsData::deserializeContents(babelwires::Deserializer& deserializer) {}

libTestUtils::TestFeatureElementWithOptionals::TestFeatureElementWithOptionals()
    : TestFeatureElementWithOptionals(TestFeatureElementWithOptionalsData(), 10) {}

libTestUtils::TestFeatureElementWithOptionals::TestFeatureElementWithOptionals(const TestFeatureElementWithOptionalsData& data, babelwires::ElementId newId)
    : FeatureElement(data, newId) {
    setFactoryName(data.m_factoryIdentifier);
    m_feature = std::make_unique<TestFeatureWithOptionals>();
}

void libTestUtils::TestFeatureElementWithOptionals::doProcess(babelwires::UserLogger&) {}

babelwires::RecordFeature* libTestUtils::TestFeatureElementWithOptionals::getInputFeature() {
    return m_feature.get();
}

babelwires::RecordFeature* libTestUtils::TestFeatureElementWithOptionals::getOutputFeature() {
    return m_feature.get();
}
