#pragma once

#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>

#include <Tests/BabelWiresLib/TestUtils/testRootFeature.hpp>

namespace testUtils {
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

        babelwires::RootFeature* doGetInputFeatureNonConst() override;
        babelwires::RootFeature* doGetOutputFeatureNonConst() override;
        const babelwires::RootFeature* getInputFeature() const override;
        const babelwires::RootFeature* getOutputFeature() const override;

        void simulateFailure(const babelwires::ProjectContext& context);
        void simulateRecovery(const babelwires::ProjectContext& context);

        std::unique_ptr<babelwires::RootFeature> m_feature;
    };
} // namespace testUtils
