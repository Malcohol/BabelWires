#include <Tests/BabelWiresLib/TestUtils/testRootFeature.hpp>

const babelwires::FeaturePath testUtils::TestRootFeature::s_pathToInt =
    babelwires::FeaturePath::deserializeFromString(testUtils::TestRootFeature::s_intIdInitializer);
const babelwires::FeaturePath testUtils::TestRootFeature::s_pathToRecord =
    babelwires::FeaturePath::deserializeFromString(testUtils::TestRootFeature::s_recordIdInitializer);
const babelwires::FeaturePath testUtils::TestRootFeature::s_pathToInt2 =
    babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestRootFeature::s_recordIdInitializer) +
                                                   "/" + testUtils::TestRootFeature::s_int2IdInitializer);
const babelwires::FeaturePath testUtils::TestRootFeature::s_pathToExtraInt =
    babelwires::FeaturePath::deserializeFromString(std::string(testUtils::TestRootFeature::s_recordIdInitializer) +
                                                   "/" + testUtils::TestRootFeature::s_extraIntIdInitializer);

namespace {
    struct LimitedIntFeature : babelwires::IntFeature {
        LimitedIntFeature(int intValueLimit) : IntFeature(-intValueLimit, intValueLimit) {}
    };
} // namespace

testUtils::TestRootFeature::TestRootFeature(const babelwires::ProjectContext& context, int intValueLimit, bool addExtraInt)
    : RootFeature(context)
    , m_intId(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_intIdInitializer, s_intFieldName, s_intUuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_recordId(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_recordIdInitializer, s_recordFieldName, s_recordUuid,
          babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_int2Id(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_int2IdInitializer, s_int2FieldName, s_int2Uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_extraIntId(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
          s_extraIntIdInitializer, s_extraIntFieldName, s_extraIntUuid,
          babelwires::IdentifierRegistry::Authority::isAuthoritative))
    , m_intValueLimit(intValueLimit) {
    auto intFeaturePtr = std::make_unique<LimitedIntFeature>(m_intValueLimit);
    m_intFeature = intFeaturePtr.get();
    addField(std::move(intFeaturePtr), m_intId);

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
