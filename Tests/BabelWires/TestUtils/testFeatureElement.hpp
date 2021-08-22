#pragma once

#include "BabelWiresLib/Features/recordFeature.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElementData.hpp"

#include "Tests/BabelWires/TestUtils/testRecord.hpp"

namespace libTestUtils {
    struct TestFailedFeature : babelwires::RecordFeature {
        TestFailedFeature();
    };

    ///
    struct TestFeatureElementData : babelwires::ElementData {
        CLONEABLE(TestFeatureElementData);
        CUSTOM_CLONEABLE(TestFeatureElementData);
        SERIALIZABLE(TestFeatureElementData, "TestFeatureElementData", babelwires::ElementData, 1);

        // By default, create a TestRecordFeature.
        TestFeatureElementData();
        TestFeatureElementData(const TestFeatureElementData& other) = default;
        TestFeatureElementData(const TestFeatureElementData& other, babelwires::ShallowCloneContext);

        // Dummy implementations.
        void serializeContents(babelwires::Serializer& serializer) const override;
        void deserializeContents(babelwires::Deserializer& deserializer) override;
        bool checkFactoryVersion(const babelwires::ProjectContext& context,
                                 babelwires::UserLogger& userLogger) override;

        // Creates a TestFeatureElement.
        std::unique_ptr<babelwires::FeatureElement> doCreateFeatureElement(const babelwires::ProjectContext& context,
                                                                           babelwires::UserLogger& userLogger,
                                                                           babelwires::ElementId newId) const override;

        // The limit on int value features in the TestRecordFeature.
        // This is used to test failed applications.
        int m_intValueLimit = 255;
    };

    struct TestFeatureElement : babelwires::FeatureElement {
        TestFeatureElement();
        TestFeatureElement(const TestFeatureElementData& data, babelwires::ElementId newId);
        void doProcess(babelwires::UserLogger&) override;

        babelwires::RecordFeature* getInputFeature() override;
        babelwires::RecordFeature* getOutputFeature() override;
        using babelwires::FeatureElement::getInputFeature;
        using babelwires::FeatureElement::getOutputFeature;

        void simulateFailure();
        void simulateRecovery();

        std::unique_ptr<babelwires::RecordFeature> m_feature;
    };
} // namespace libTestUtils
