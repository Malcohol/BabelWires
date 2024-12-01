#pragma once

#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>

// TODO: Need to preserve SimulateFailure and SimulateRecovery if this gets replaced !!!
// See Commented code in FeatureElementTest.

namespace testUtils {
    ///
    struct TestFeatureElementData : babelwires::NodeData {
        CLONEABLE(TestFeatureElementData);
        CUSTOM_CLONEABLE(TestFeatureElementData);
        SERIALIZABLE(TestFeatureElementData, "TestFeatureElementData", babelwires::NodeData, 1);

        TestFeatureElementData();
        TestFeatureElementData(const TestFeatureElementData& other) = default;
        TestFeatureElementData(const TestFeatureElementData& other, babelwires::ShallowCloneContext);

        // Dummy implementations.
        void serializeContents(babelwires::Serializer& serializer) const override;
        void deserializeContents(babelwires::Deserializer& deserializer) override;
        bool checkFactoryVersion(const babelwires::ProjectContext& context,
                                 babelwires::UserLogger& userLogger) override;

        // Creates a TestFeatureElement.
        std::unique_ptr<babelwires::Node> doCreateFeatureElement(const babelwires::ProjectContext& context,
                                                                           babelwires::UserLogger& userLogger,
                                                                           babelwires::ElementId newId) const override;
    };

    struct TestFeatureElement : babelwires::Node {
        TestFeatureElement(const babelwires::ProjectContext& context);
        TestFeatureElement(const babelwires::ProjectContext& context, const TestFeatureElementData& data, babelwires::ElementId newId);
        void doProcess(babelwires::UserLogger&) override;

        babelwires::ValueTreeNode* doGetInputNonConst() override;
        babelwires::ValueTreeNode* doGetOutputNonConst() override;
        const babelwires::ValueTreeNode* getInput() const override;
        const babelwires::ValueTreeNode* getOutput() const override;

        void simulateFailure(const babelwires::ProjectContext& context);
        void simulateRecovery(const babelwires::ProjectContext& context);

        std::unique_ptr<babelwires::ValueTreeNode> m_valueTreeRoot;
    };
} // namespace testUtils
