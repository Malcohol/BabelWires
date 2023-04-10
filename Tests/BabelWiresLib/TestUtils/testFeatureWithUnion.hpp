#pragma once

#include <gtest/gtest.h>

#include <BabelWiresLib/Features/unionFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>

#include <Tests/BabelWiresLib/TestUtils/testRecord.hpp>

namespace testUtils {
    /// A record with optional fields.
    struct TestFeatureWithUnion : babelwires::RootFeature {
        TestFeatureWithUnion(const babelwires::ProjectContext& context);

        static constexpr char s_tagAIdInitializer[] = "tagA";
        static constexpr char s_tagBIdInitializer[] = "tagB";
        static constexpr char s_tagCIdInitializer[] = "tagC";
        static constexpr char s_unionFeatureIdInitializer[] = "subrd";
        static constexpr char s_ff0IdInitializer[] = "ff0";
        static constexpr char s_ff1IdInitializer[] = "ff1";
        static constexpr char s_fieldA0IdInitializer[] = "A0";
        static constexpr char s_fieldA1IdInitializer[] = "A1";
        static constexpr char s_fieldB0IdInitializer[] = "B0";
        static constexpr char s_fieldABIdInitializer[] = "AB";
        static constexpr char s_fieldBCIdInitializer[] = "BC";
        
        static constexpr char s_tagAFieldName[] = "tagA";
        static constexpr char s_tagBFieldName[] = "tagB";
        static constexpr char s_tagCFieldName[] = "tagC";
        static constexpr char s_unionFeatureFieldName[] = "union";
        static constexpr char s_ff0FieldName[] = "fixed field 0";
        static constexpr char s_ff1FieldName[] = "fixed field 1";
        static constexpr char s_fieldA0FieldName[] = "branch A field 0";
        static constexpr char s_fieldA1FieldName[] = "branch A field 1";
        static constexpr char s_fieldB0FieldName[] = "branch B field 0";
        static constexpr char s_fieldABFieldName[] = "branch AB";
        static constexpr char s_fieldBCFieldName[] = "branch BC";

        static constexpr char s_tagAUuid[] = "00000000-1111-2222-3333-88000000000A";
        static constexpr char s_tagBUuid[] = "00000000-1111-2222-3333-88000000000B";
        static constexpr char s_tagCUuid[] = "00000000-1111-2222-3333-88000000000C";
        static constexpr char s_unionFeatureUuid[] = "00000000-1111-2222-3333-880000000000";
        static constexpr char s_ff0Uuid[] = "00000000-1111-2222-3333-880000000001";
        static constexpr char s_ff1Uuid[] = "00000000-1111-2222-3333-880000000002";
        static constexpr char s_fieldA0Uuid[] = "00000000-1111-2222-3333-8800000000A0";
        static constexpr char s_fieldA1Uuid[] = "00000000-1111-2222-3333-8800000000A1";
        static constexpr char s_fieldB0Uuid[] = "00000000-1111-2222-3333-8800000000B0";
        static constexpr char s_fieldABUuid[] = "00000000-1111-2222-3333-8800000000AB";
        static constexpr char s_fieldBCUuid[] = "00000000-1111-2222-3333-8800000000BC";

        // Note: tagB is the default.
        babelwires::ShortId m_tagAId;
        babelwires::ShortId m_tagBId;
        babelwires::ShortId m_tagCId;
        babelwires::ShortId m_unionFeatureId;
        babelwires::ShortId m_ff0Id;
        babelwires::ShortId m_ff1Id;
        babelwires::ShortId m_fieldA0Id;
        babelwires::ShortId m_fieldA1Id;
        babelwires::ShortId m_fieldB0Id;
        babelwires::ShortId m_fieldABId;
        babelwires::ShortId m_fieldBCId;

        babelwires::UnionFeature* m_unionFeature;
        babelwires::IntFeature* m_ff0Feature;
        testUtils::TestRecordFeature* m_ff1Feature;
        babelwires::IntFeature* m_fieldA0Feature;
        babelwires::IntFeature* m_fieldA1Feature;
        testUtils::TestRecordFeature* m_fieldB0Feature;
        babelwires::IntFeature* m_fieldABFeature;
        babelwires::IntFeature* m_fieldBCFeature;

        // For convenience
        static const babelwires::FeaturePath s_pathToUnionFeature;
        static const babelwires::FeaturePath s_pathToFf0;
        static const babelwires::FeaturePath s_pathToFf1;
        static const babelwires::FeaturePath s_pathToFieldA0;
        static const babelwires::FeaturePath s_pathToFieldA1;
        static const babelwires::FeaturePath s_pathToFieldB0;

        static const babelwires::FeaturePath s_pathToFieldB0_Array_1;
        static const babelwires::FeaturePath s_pathToFieldB0_Int2;

        static const babelwires::FeaturePath s_pathToFieldAB;
        static const babelwires::FeaturePath s_pathToFieldBC;
    };

    struct TestFeatureElementWithUnionData : babelwires::ElementData {
        CLONEABLE(TestFeatureElementWithUnionData);
        CUSTOM_CLONEABLE(TestFeatureElementWithUnionData);
        SERIALIZABLE(TestFeatureElementWithUnionData, "TestFeatureElementWithUnionData",
                     babelwires::ElementData, 1);

        // By default, create a TestRecordFeature.
        TestFeatureElementWithUnionData();
        TestFeatureElementWithUnionData(const TestFeatureElementWithUnionData& other) = default;
        TestFeatureElementWithUnionData(const TestFeatureElementWithUnionData& other,
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

    struct TestFeatureElementWithUnion : babelwires::FeatureElement {
        TestFeatureElementWithUnion(const babelwires::ProjectContext& context);
        TestFeatureElementWithUnion(const babelwires::ProjectContext& context, const TestFeatureElementWithUnionData& data, babelwires::ElementId newId);
        void doProcess(babelwires::UserLogger&) override;

        babelwires::RootFeature* getInputFeature() override;
        babelwires::RootFeature* getOutputFeature() override;
        using babelwires::FeatureElement::getInputFeature;
        using babelwires::FeatureElement::getOutputFeature;

        std::unique_ptr<testUtils::TestFeatureWithUnion> m_feature;
    };

} // namespace testUtils
