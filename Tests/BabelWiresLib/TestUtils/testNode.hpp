#pragma once

#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>

// TODO: Need to preserve SimulateFailure and SimulateRecovery if this gets replaced !!!
// See Commented code in NodeTest.

namespace testUtils {
    ///
    struct TestNodeData : babelwires::NodeData {
        CLONEABLE(TestNodeData);
        CUSTOM_CLONEABLE(TestNodeData);
        SERIALIZABLE(TestNodeData, "TestNodeData", babelwires::NodeData, 1);

        TestNodeData();
        TestNodeData(const TestNodeData& other) = default;
        TestNodeData(const TestNodeData& other, babelwires::ShallowCloneContext);

        // Dummy implementations.
        void serializeContents(babelwires::Serializer& serializer) const override;
        void deserializeContents(babelwires::Deserializer& deserializer) override;
        bool checkFactoryVersion(const babelwires::ProjectContext& context,
                                 babelwires::UserLogger& userLogger) override;

        // Creates a TestNode.
        std::unique_ptr<babelwires::Node> doCreateNode(const babelwires::ProjectContext& context,
                                                                           babelwires::UserLogger& userLogger,
                                                                           babelwires::NodeId newId) const override;
    };

    struct TestNode : babelwires::Node {
        TestNode(const babelwires::ProjectContext& context);
        TestNode(const babelwires::ProjectContext& context, const TestNodeData& data, babelwires::NodeId newId);
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
