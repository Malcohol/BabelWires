#pragma once

#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>

// TODO: Need to preserve SimulateFailure and SimulateRecovery if this gets replaced !!!
// See Commented code in FeatureElementTest.

namespace testUtils {
    ///
    struct TestFeatureElementData : babelwires::ElementData {
        CLONEABLE(TestFeatureElementData);
        CUSTOM_CLONEABLE(TestFeatureElementData);
        SERIALIZABLE(TestFeatureElementData, "TestFeatureElementData", babelwires::ElementData, 1);

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
    };

    struct TestFeatureElement : babelwires::FeatureElement {
        TestFeatureElement(const babelwires::ProjectContext& context);
        TestFeatureElement(const babelwires::ProjectContext& context, const TestFeatureElementData& data, babelwires::ElementId newId);
        void doProcess(babelwires::UserLogger&) override;

        babelwires::Feature* doGetInputFeatureNonConst() override;
        babelwires::Feature* doGetOutputFeatureNonConst() override;
        const babelwires::Feature* getInputFeature() const override;
        const babelwires::Feature* getOutputFeature() const override;

        void simulateFailure(const babelwires::ProjectContext& context);
        void simulateRecovery(const babelwires::ProjectContext& context);

        std::unique_ptr<babelwires::Feature> m_feature;
    };
} // namespace testUtils
