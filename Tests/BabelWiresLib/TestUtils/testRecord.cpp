#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

#include "BabelWiresLib/Features/featureMixins.hpp"

const babelwires::FeaturePath libTestUtils::TestRecordFeature::s_pathToInt =
    babelwires::FeaturePath::deserializeFromString(libTestUtils::TestRecordFeature::s_intIdInitializer);
const babelwires::FeaturePath libTestUtils::TestRecordFeature::s_pathToArray =
    babelwires::FeaturePath::deserializeFromString(libTestUtils::TestRecordFeature::s_arrayIdInitializer);
const babelwires::FeaturePath libTestUtils::TestRecordFeature::s_pathToArray_0 =
    babelwires::FeaturePath::deserializeFromString(std::string(libTestUtils::TestRecordFeature::s_arrayIdInitializer) +
                                                   "/0");
const babelwires::FeaturePath libTestUtils::TestRecordFeature::s_pathToArray_1 =
    babelwires::FeaturePath::deserializeFromString(std::string(libTestUtils::TestRecordFeature::s_arrayIdInitializer) +
                                                   "/1");
const babelwires::FeaturePath libTestUtils::TestRecordFeature::s_pathToArray_2 =
    babelwires::FeaturePath::deserializeFromString(std::string(libTestUtils::TestRecordFeature::s_arrayIdInitializer) +
                                                   "/2");
const babelwires::FeaturePath libTestUtils::TestRecordFeature::s_pathToArray_3 =
    babelwires::FeaturePath::deserializeFromString(std::string(libTestUtils::TestRecordFeature::s_arrayIdInitializer) +
                                                   "/3");
const babelwires::FeaturePath libTestUtils::TestRecordFeature::s_pathToArray_4 =
    babelwires::FeaturePath::deserializeFromString(std::string(libTestUtils::TestRecordFeature::s_arrayIdInitializer) +
                                                   "/4");
const babelwires::FeaturePath libTestUtils::TestRecordFeature::s_pathToRecord =
    babelwires::FeaturePath::deserializeFromString(libTestUtils::TestRecordFeature::s_recordIdInitializer);
const babelwires::FeaturePath libTestUtils::TestRecordFeature::s_pathToInt2 =
    babelwires::FeaturePath::deserializeFromString(std::string(libTestUtils::TestRecordFeature::s_recordIdInitializer) +
                                                   "/" + libTestUtils::TestRecordFeature::s_int2IdInitializer);
const babelwires::FeaturePath libTestUtils::TestRecordFeature::s_pathToExtraInt =
    babelwires::FeaturePath::deserializeFromString(std::string(libTestUtils::TestRecordFeature::s_recordIdInitializer) +
                                                   "/" + libTestUtils::TestRecordFeature::s_extraIntIdInitializer);

namespace {
    struct LimitedIntFeature : babelwires::IntFeature {
        LimitedIntFeature(int intValueLimit)
            : m_intValueLimit(intValueLimit) {}

        babelwires::Range<int> getRange() const override { return {-m_intValueLimit, m_intValueLimit}; }

        int m_intValueLimit;
    };

    struct LimitedArrayFeature : babelwires::ArrayFeature {
        LimitedArrayFeature(int intValueLimit)
            : m_intValueLimit(intValueLimit) {}

        virtual std::unique_ptr<Feature> createNextEntry() const override {
            return std::make_unique<LimitedIntFeature>(m_intValueLimit);
        }

        virtual babelwires::Range<unsigned int> doGetSizeRange() const override { return {2, 8}; }

        int m_intValueLimit;
    };
} // namespace

libTestUtils::TestRecordFeature::TestRecordFeature(int intValueLimit, bool addExtraInt)
    : m_intId(babelwires::IdentifierRegistry::write()->addFieldName(
          s_intIdInitializer, s_intFieldName, s_intUuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_arrayId(babelwires::IdentifierRegistry::write()->addFieldName(
          s_arrayIdInitializer, s_arrayFieldName, s_arrayUuid,
          babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_recordId(babelwires::IdentifierRegistry::write()->addFieldName(
          s_recordIdInitializer, s_recordFieldName, s_recordUuid,
          babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_int2Id(babelwires::IdentifierRegistry::write()->addFieldName(
          s_int2IdInitializer, s_int2FieldName, s_int2Uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_extraIntId(babelwires::IdentifierRegistry::write()->addFieldName(
          s_extraIntIdInitializer, s_extraIntFieldName, s_extraIntUuid,
          babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_intValueLimit(intValueLimit) {
    auto intFeaturePtr = std::make_unique<LimitedIntFeature>(m_intValueLimit);
    m_intFeature = intFeaturePtr.get();
    addField(std::move(intFeaturePtr), m_intId);

    auto arrayFeaturePtr = std::make_unique<LimitedArrayFeature>(m_intValueLimit);
    m_arrayFeature = arrayFeaturePtr.get();
    addField(std::move(arrayFeaturePtr), m_arrayId)->setToDefault();
    m_elem0 = static_cast<babelwires::IntFeature*>(&m_arrayFeature->getChildFromStep(0));
    m_elem1 = static_cast<babelwires::IntFeature*>(&m_arrayFeature->getChildFromStep(1));

    auto subRecordPtr = std::make_unique<babelwires::RecordFeature>();
    m_subRecordFeature = subRecordPtr.get();
    addField(std::move(subRecordPtr), m_recordId);

    auto intFeature2Ptr = std::make_unique<LimitedIntFeature>(m_intValueLimit);
    m_intFeature2 = intFeature2Ptr.get();
    m_subRecordFeature->addField(std::move(intFeature2Ptr), m_int2Id);

    if (addExtraInt) {
        auto extraIntPtr = std::make_unique<LimitedIntFeature>(m_intValueLimit);
        m_extraIntFeature = extraIntPtr.get();
        m_subRecordFeature->addField(std::move(extraIntPtr), m_extraIntId);
    }
}
