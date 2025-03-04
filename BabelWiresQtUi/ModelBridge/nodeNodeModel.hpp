/**
 * NodeNodeModel is the NodeDataModels corresponding to Nodes.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresQtUi/ModelBridge/baseNodeModel.hpp>

namespace babelwires {
    class ValueTreeNode;
    class Node;
    class ContentsCache;
    class Path;

    class NodeContentsView;
    class NodeContentsModel;
    class RowModelDelegate;
    class AccessModelScope;

    struct UiSize;

    /// The NodeDataModel which presents a BabelWires Node as a QtNode.
    class NodeNodeModel : public BaseNodeModel {
        Q_OBJECT

      public:
        NodeNodeModel(ProjectBridge& project, NodeId elementId);
        ~NodeNodeModel();

        virtual unsigned int nPorts(QtNodes::PortType portType) const override;
        virtual QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
        virtual QWidget* embeddedWidget() override;

        babelwires::NodeId getNodeId() const;
        virtual QString caption() const override;

        const Path& getPathAtPort(AccessModelScope& scope, QtNodes::PortType portType,
                                         QtNodes::PortIndex portIndex) const;
        QtNodes::PortIndex getPortAtPath(AccessModelScope& scope, QtNodes::PortType portType,
                                         const Path& path) const;

        ResizableAxes resizable() const override { return HorizontallyResizable; }

        void setSize(const UiSize& newSize);

        /// Return the model for the embedded widget.
        NodeContentsModel& getModel();

      public slots:
        void customContextMenuRequested(const QPoint& pos);

      protected:
        /// Set up the contents to match the state of the node.
        void setContents(std::string label, NodeId nodeId);

        static QtNodes::NodeDataType getDataTypeFromTreeValueNode(const ValueTreeNode* f);

        const ValueTreeNode* getInput(AccessModelScope& scope, int portIndex) const;
        const ValueTreeNode* getOutput(AccessModelScope& scope, int portIndex) const;

      protected:
        NodeId m_nodeId;

        NodeContentsView* m_view;
        NodeContentsModel* m_model;
        RowModelDelegate* m_delegate;

        int m_numRows = 0;
    };

} // namespace babelwires
