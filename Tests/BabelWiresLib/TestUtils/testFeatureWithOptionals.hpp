#pragma once

#include <gtest/gtest.h>

#include <BabelWiresLib/Features/recordWithOptionalsFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>

#include <Tests/BabelWiresLib/TestUtils/testRecord.hpp>

namespace testUtils {
    /// A record with optional fields.
    struct TestFeatureWithOptionals : babelwires::RootFeature {
        TestFeatureWithOptionals(const babelwires::ProjectContext& context);

        static constexpr char s_subrecordIdInitializer[] = "subrd";
        static constexpr char s_ff0IdInitializer[] = "ff0";
        static constexpr char s_ff1IdInitializer[] = "ff1";
        static constexpr char s_op0IdInitializer[] = "op0";
        static constexpr char s_op1IdInitializer[] = "op1";

        static constexpr char s_subrecordFieldName[] = "subrecord with optionals";
        static constexpr char s_ff0FieldName[] = "fixed field 0";
        static constexpr char s_ff1FieldName[] = "fixed field 1";
        static constexpr char s_op0FieldName[] = "optional field 0";
        static constexpr char s_op1FieldName[] = "optional field 1";

        static constexpr char s_subrecordUuid[] = "00000000-1111-2222-3333-880000000000";
        static constexpr char s_ff0Uuid[] = "00000000-1111-2222-3333-880000000001";
        static constexpr char s_ff1Uuid[] = "00000000-1111-2222-3333-880000000002";
        static constexpr char s_op0Uuid[] = "00000000-1111-2222-3333-880000000003";
        static constexpr char s_op1Uuid[] = "00000000-1111-2222-3333-880000000004";

        babelwires::ShortId m_subrecordId;
        babelwires::ShortId m_ff0Id;
        babelwires::ShortId m_ff1Id;
        babelwires::ShortId m_op0Id;
        babelwires::ShortId m_op1Id;

        babelwires::RecordWithOptionalsFeature* m_subrecord;
        babelwires::IntFeature* m_ff0Feature;
        testUtils::TestRecordFeature* m_ff1Feature;
        babelwires::IntFeature* m_op0Feature;
        testUtils::TestRecordFeature* m_op1Feature;

        // For convenience
        static const babelwires::FeaturePath s_pathToSubrecord;
        static const babelwires::FeaturePath s_pathToFf0;
        static const babelwires::FeaturePath s_pathToFf1;
        static const babelwires::FeaturePath s_pathToOp0;
        static const babelwires::FeaturePath s_pathToOp1;

        static const babelwires::FeaturePath s_pathToOp1_Array_1;
        static const babelwires::FeaturePath s_pathToOp1_Int2;
    };

    struct TestFeatureElementWithOptionalsData : babelwires::ElementData {
        CLONEABLE(TestFeatureElementWithOptionalsData);
        CUSTOM_CLONEABLE(TestFeatureElementWithOptionalsData);
        SERIALIZABLE(TestFeatureElementWithOptionalsData, "TestFeatureElementWithOptionalsData",
                     babelwires::ElementData, 1);

        // By default, create a TestRecordFeature.
        TestFeatureElementWithOptionalsData();
        TestFeatureElementWithOptionalsData(const TestFeatureElementWithOptionalsData& other) = default;
        TestFeatureElementWithOptionalsData(const TestFeatureElementWithOptionalsData& other,
                                            babelwires::ShallowCloneContext);

        // Dummy implementations.
        void serializeContents(babelwires::Serializer& serializer) const override;
        void deserializeContents(babelwires::Deserializer& deserializer) override;
        bool checkFactoryVersion(const babelwires::ProjectContext& context,
                                 babelwires::UserLogger& userLogger) override;

        // Creates a TestFeatureElement.
        std::unique_ptr<babelwires::FeatureElement> doCreateFeatureElement(const babelwires::ProjectContext& context,
                                                                           babelwires::UserLogger& userLogger,
                                                                           babelwires::ElementId newId) const override;
    };

    struct TestFeatureElementWithOptionals : babelwires::FeatureElement {
        TestFeatureElementWithOptionals(const babelwires::ProjectContext& context);
        TestFeatureElementWithOptionals(const babelwires::ProjectContext& context, const TestFeatureElementWithOptionalsData& data, babelwires::ElementId newId);
        void doProcess(babelwires::UserLogger&) override;

        babelwires::RootFeature* getInputFeature() override;
        babelwires::RootFeature* getOutputFeature() override;
        using babelwires::FeatureElement::getInputFeature;
        using babelwires::FeatureElement::getOutputFeature;

        std::unique_ptr<testUtils::TestFeatureWithOptionals> m_feature;
    };

} // namespace testUtils
