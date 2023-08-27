#include <gtest/gtest.h>

#include <Tests/BabelWiresLib/TestUtils/testFeatureWithUnion.hpp>

const babelwires::FeaturePath testUtils::TestFeatureWithUnion::s_pathToUnionFeature =
    babelwires::FeaturePath::deserializeFromString(testUtils::TestFeatureWithUnion::s_unionFeatureIdInitializer);
const babelwires::FeaturePath testUtils::TestFeatureWithUnion::s_pathToFf0 =
    babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithUnion::s_unionFeatureIdInitializer) + "/" + testUtils::TestFeatureWithUnion::s_ff0IdInitializer);
const babelwires::FeaturePath testUtils::TestFeatureWithUnion::s_pathToFf1 =
    babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithUnion::s_unionFeatureIdInitializer) + "/" + testUtils::TestFeatureWithUnion::s_ff1IdInitializer);
const babelwires::FeaturePath testUtils::TestFeatureWithUnion::s_pathToFieldA0 =
    babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithUnion::s_unionFeatureIdInitializer) + "/" + testUtils::TestFeatureWithUnion::s_fieldA0IdInitializer);
const babelwires::FeaturePath testUtils::TestFeatureWithUnion::s_pathToFieldA1 =
    babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithUnion::s_unionFeatureIdInitializer) + "/" + testUtils::TestFeatureWithUnion::s_fieldA1IdInitializer);
const babelwires::FeaturePath testUtils::TestFeatureWithUnion::s_pathToFieldB0 =
    babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithUnion::s_unionFeatureIdInitializer) + "/" + testUtils::TestFeatureWithUnion::s_fieldB0IdInitializer);
const babelwires::FeaturePath testUtils::TestFeatureWithUnion::s_pathToFieldAB =
    babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithUnion::s_unionFeatureIdInitializer) + "/" + testUtils::TestFeatureWithUnion::s_fieldABIdInitializer);
const babelwires::FeaturePath testUtils::TestFeatureWithUnion::s_pathToFieldBC =
    babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithUnion::s_unionFeatureIdInitializer) + "/" + testUtils::TestFeatureWithUnion::s_fieldBCIdInitializer);

const babelwires::FeaturePath testUtils::TestFeatureWithUnion::s_pathToFieldB0_Array_1 = babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithUnion::s_unionFeatureIdInitializer) + "/" + testUtils::TestFeatureWithUnion::s_fieldB0IdInitializer + "/" + testUtils::TestRecordFeature::s_arrayIdInitializer + "/1");
const babelwires::FeaturePath testUtils::TestFeatureWithUnion::s_pathToFieldB0_Int2 = babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestFeatureWithUnion::s_unionFeatureIdInitializer) + "/" + testUtils::TestFeatureWithUnion::s_fieldB0IdInitializer + "/" + testUtils::TestRecordFeature::s_recordIdInitializer + "/" + testUtils::TestRecordFeature::s_int2IdInitializer);

testUtils::TestFeatureWithUnion::TestFeatureWithUnion(const babelwires::ProjectContext& context)
    : RootFeature(context)
    , m_tagAId(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_tagAIdInitializer, s_tagAFieldName, s_tagAUuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_tagBId(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_tagBIdInitializer, s_tagBFieldName, s_tagBUuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_tagCId(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_tagCIdInitializer, s_tagCFieldName, s_tagCUuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_unionFeatureId(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_unionFeatureIdInitializer, s_unionFeatureFieldName, s_unionFeatureUuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_ff0Id(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_ff0IdInitializer, s_ff0FieldName, s_ff0Uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_ff1Id(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_ff1IdInitializer, s_ff1FieldName, s_ff1Uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_fieldA0Id(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_fieldA0IdInitializer, s_fieldA0FieldName, s_fieldA0Uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_fieldA1Id(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_fieldA1IdInitializer, s_fieldA1FieldName, s_fieldA1Uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_fieldB0Id(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_fieldB0IdInitializer, s_fieldB0FieldName, s_fieldB0Uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_fieldABId(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_fieldABIdInitializer, s_fieldABFieldName, s_fieldABUuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_fieldBCId(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_fieldBCIdInitializer, s_fieldBCFieldName, s_fieldBCUuid, babelwires::IdentifierRegistry::Authority::isAuthoritative)) {
    {
        auto testUnionFeaturePtr = std::make_unique<babelwires::UnionFeature>(babelwires::UnionFeature::TagValues{m_tagAId, m_tagBId, m_tagCId}, 1);
        m_unionFeature = testUnionFeaturePtr.get();
        addField(std::move(testUnionFeaturePtr), m_unionFeatureId);
    }
    {
        auto intFeaturePtr = std::make_unique<babelwires::IntFeature>();
        m_fieldA0Feature = intFeaturePtr.get();
        m_unionFeature->addFieldInBranch(m_tagAId, std::move(intFeaturePtr), m_fieldA0Id);
    }
    {
        auto intFeaturePtr = std::make_unique<babelwires::IntFeature>();
        m_ff0Feature = intFeaturePtr.get();
        m_unionFeature->addField(std::move(intFeaturePtr), m_ff0Id);
    }
    {
        auto testRecordFeaturePtr = std::make_unique<testUtils::TestRecordFeature>();
        m_fieldB0Feature = testRecordFeaturePtr.get();
        m_unionFeature->addFieldInBranch(m_tagBId, std::move(testRecordFeaturePtr), m_fieldB0Id);
    }
    {
        auto intFeaturePtr = std::make_unique<babelwires::IntFeature>();
        m_fieldABFeature = intFeaturePtr.get();
        m_unionFeature->addFieldInBranches({m_tagAId, m_tagBId}, std::move(intFeaturePtr), m_fieldABId);
    }
    {
        auto intFeaturePtr = std::make_unique<babelwires::IntFeature>();
        m_fieldA1Feature = intFeaturePtr.get();
        m_unionFeature->addFieldInBranch(m_tagAId, std::move(intFeaturePtr), m_fieldA1Id);
    }
    {
        auto testRecordFeaturePtr = std::make_unique<testUtils::TestRecordFeature>();
        m_ff1Feature = testRecordFeaturePtr.get();
        m_unionFeature->addField(std::move(testRecordFeaturePtr), m_ff1Id);
    }
    {
        auto intFeaturePtr = std::make_unique<babelwires::IntFeature>();
        m_fieldBCFeature = intFeaturePtr.get();
        m_unionFeature->addFieldInBranches({m_tagBId, m_tagCId}, std::move(intFeaturePtr), m_fieldBCId);
    }
}

testUtils::TestFeatureElementWithUnionData::TestFeatureElementWithUnionData() {
    m_factoryIdentifier = "TestWithOptsFactory";
    m_factoryVersion = 1;
}

testUtils::TestFeatureElementWithUnionData::TestFeatureElementWithUnionData(const TestFeatureElementWithUnionData& other,
                                                             babelwires::ShallowCloneContext context)
    : ElementData(other, context)
    {}

bool testUtils::TestFeatureElementWithUnionData::checkFactoryVersion(const babelwires::ProjectContext& context,
                                                               babelwires::UserLogger& userLogger) {
    return true;
}

std::unique_ptr<babelwires::FeatureElement> testUtils::TestFeatureElementWithUnionData::doCreateFeatureElement(
    const babelwires::ProjectContext& context, babelwires::UserLogger& userLogger, babelwires::ElementId newId) const {
    return std::make_unique<TestFeatureElementWithUnion>(context, *this, newId);
}

void testUtils::TestFeatureElementWithUnionData::serializeContents(babelwires::Serializer& serializer) const {}

void testUtils::TestFeatureElementWithUnionData::deserializeContents(babelwires::Deserializer& deserializer) {}

testUtils::TestFeatureElementWithUnion::TestFeatureElementWithUnion(const babelwires::ProjectContext& context)
    : TestFeatureElementWithUnion(context, TestFeatureElementWithUnionData(), 10) {}

testUtils::TestFeatureElementWithUnion::TestFeatureElementWithUnion(const babelwires::ProjectContext& context, const TestFeatureElementWithUnionData& data, babelwires::ElementId newId)
    : FeatureElement(data, newId) {
    setFactoryName(data.m_factoryIdentifier);
    m_feature = std::make_unique<TestFeatureWithUnion>(context);
}

void testUtils::TestFeatureElementWithUnion::doProcess(babelwires::UserLogger&) {}

const babelwires::RootFeature* testUtils::TestFeatureElementWithUnion::getInputFeature() const {
    return m_feature.get();
}

const babelwires::RootFeature* testUtils::TestFeatureElementWithUnion::getOutputFeature() const {
    return m_feature.get();
}

babelwires::RootFeature* testUtils::TestFeatureElementWithUnion::getInputFeatureNonConst() {
    return m_feature.get();
}

babelwires::RootFeature* testUtils::TestFeatureElementWithUnion::getOutputFeatureNonConst() {
    return m_feature.get();
}
