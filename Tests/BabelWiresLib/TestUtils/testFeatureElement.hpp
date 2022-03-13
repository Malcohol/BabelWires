#pragma once

#include "BabelWiresLib/Features/rootFeature.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElementData.hpp"

#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

namespace libTestUtils {
    struct TestFailedFeature : babelwires::RootFeature {
        TestFailedFeature(const babelwires::ProjectContext& context);
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
        TestFeatureElement(const babelwires::ProjectContext& context);
        TestFeatureElement(const babelwires::ProjectContext& context, const TestFeatureElementData& data, babelwires::ElementId newId);
        void doProcess(babelwires::UserLogger&) override;

        babelwires::RootFeature* getInputFeature() override;
        babelwires::RootFeature* getOutputFeature() override;
        using babelwires::FeatureElement::getInputFeature;
        using babelwires::FeatureElement::getOutputFeature;

        void simulateFailure();
        void simulateRecovery();

        babelwires::RootFeature* m_feature;
        std::unique_ptr<babelwires::RootFeature> m_actualFeature;
        std::unique_ptr<TestFailedFeature> m_failedFeature;
    };
} // namespace libTestUtils
