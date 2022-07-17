#include <gtest/gtest.h>

#include <Tests/BabelWiresLib/TestUtils/testFeatureWithOptionals.hpp>

const babelwires::FeaturePath testUtils::TestFeatureWithOptionals::s_pathToSubrecord =
    babelwires::FeaturePath::deserializeFromString(testUtils::TestFeatureWithOptionals::s_subrecordIdInitializer);
const babelwires::FeaturePath testUtils::TestFeatureWithOptionals::s_pathToFf0 =
    babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithOptionals::s_subrecordIdInitializer) + "/" + testUtils::TestFeatureWithOptionals::s_ff0IdInitializer);
const babelwires::FeaturePath testUtils::TestFeatureWithOptionals::s_pathToFf1 =
    babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithOptionals::s_subrecordIdInitializer) + "/" + testUtils::TestFeatureWithOptionals::s_ff1IdInitializer);
const babelwires::FeaturePath testUtils::TestFeatureWithOptionals::s_pathToOp0 =
    babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithOptionals::s_subrecordIdInitializer) + "/" + testUtils::TestFeatureWithOptionals::s_op0IdInitializer);
const babelwires::FeaturePath testUtils::TestFeatureWithOptionals::s_pathToOp1 =
    babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithOptionals::s_subrecordIdInitializer) + "/" + testUtils::TestFeatureWithOptionals::s_op1IdInitializer);

const babelwires::FeaturePath testUtils::TestFeatureWithOptionals::s_pathToOp1_Array_1 = babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithOptionals::s_subrecordIdInitializer) + "/" + testUtils::TestFeatureWithOptionals::s_op1IdInitializer + "/" + testUtils::TestRecordFeature::s_arrayIdInitializer + "/1");
const babelwires::FeaturePath testUtils::TestFeatureWithOptionals::s_pathToOp1_Int2 = babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithOptionals::s_subrecordIdInitializer) + "/" + testUtils::TestFeatureWithOptionals::s_op1IdInitializer + "/" + testUtils::TestRecordFeature::s_recordIdInitializer + "/" + testUtils::TestRecordFeature::s_int2IdInitializer);

testUtils::TestFeatureWithOptionals::TestFeatureWithOptionals(const babelwires::ProjectContext& context)
    : RootFeature(context)
    , m_subrecordId(babelwires::IdentifierRegistry::write()->addShortIdentifierWithMetadata(
          s_subrecordIdInitializer, s_subrecordFieldName, s_subrecordUuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_ff0Id(babelwires::IdentifierRegistry::write()->addShortIdentifierWithMetadata(
          s_ff0IdInitializer, s_ff0FieldName, s_ff0Uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_ff1Id(babelwires::IdentifierRegistry::write()->addShortIdentifierWithMetadata(
          s_ff1IdInitializer, s_ff1FieldName, s_ff1Uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_op0Id(babelwires::IdentifierRegistry::write()->addShortIdentifierWithMetadata(
          s_op0IdInitializer, s_op0FieldName, s_op0Uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_op1Id(babelwires::IdentifierRegistry::write()->addShortIdentifierWithMetadata(
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
        auto testRecordFeaturePtr = std::make_unique<testUtils::TestRecordFeature>();
        m_ff1Feature = testRecordFeaturePtr.get();
        m_subrecord->addField(std::move(testRecordFeaturePtr), m_ff1Id);
    }
    {
        auto testRecordFeaturePtr = std::make_unique<testUtils::TestRecordFeature>();
        m_op1Feature = testRecordFeaturePtr.get();
        m_subrecord->addOptionalField(std::move(testRecordFeaturePtr), m_op1Id);
    }
}

testUtils::TestFeatureElementWithOptionalsData::TestFeatureElementWithOptionalsData() {
    m_factoryIdentifier = "TestWithOptsFactory";
    m_factoryVersion = 1;
}

testUtils::TestFeatureElementWithOptionalsData::TestFeatureElementWithOptionalsData(const TestFeatureElementWithOptionalsData& other,
                                                             babelwires::ShallowCloneContext context)
    : ElementData(other, context)
    {}

bool testUtils::TestFeatureElementWithOptionalsData::checkFactoryVersion(const babelwires::ProjectContext& context,
                                                               babelwires::UserLogger& userLogger) {
    return true;
}

std::unique_ptr<babelwires::FeatureElement> testUtils::TestFeatureElementWithOptionalsData::doCreateFeatureElement(
    const babelwires::ProjectContext& context, babelwires::UserLogger& userLogger, babelwires::ElementId newId) const {
    return std::make_unique<TestFeatureElementWithOptionals>(context, *this, newId);
}

void testUtils::TestFeatureElementWithOptionalsData::serializeContents(babelwires::Serializer& serializer) const {}

void testUtils::TestFeatureElementWithOptionalsData::deserializeContents(babelwires::Deserializer& deserializer) {}

testUtils::TestFeatureElementWithOptionals::TestFeatureElementWithOptionals(const babelwires::ProjectContext& context)
    : TestFeatureElementWithOptionals(context, TestFeatureElementWithOptionalsData(), 10) {}

testUtils::TestFeatureElementWithOptionals::TestFeatureElementWithOptionals(const babelwires::ProjectContext& context, const TestFeatureElementWithOptionalsData& data, babelwires::ElementId newId)
    : FeatureElement(data, newId) {
    setFactoryName(data.m_factoryIdentifier);
    m_feature = std::make_unique<TestFeatureWithOptionals>(context);
}

void testUtils::TestFeatureElementWithOptionals::doProcess(babelwires::UserLogger&) {}

babelwires::RootFeature* testUtils::TestFeatureElementWithOptionals::getInputFeature() {
    return m_feature.get();
}

babelwires::RootFeature* testUtils::TestFeatureElementWithOptionals::getOutputFeature() {
    return m_feature.get();
}
