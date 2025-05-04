/**
 * NodeNodeModel is the NodeDataModels corresponding to Nodes.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectIds.hpp>

#include <QtNodes/Definitions>
#include <QtNodes/NodeData>

#include <vector>

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

    /// This manages the View, Model and Delegate of a Node's embedded widget.
    /// TODO: Right now, it also manages access to a bunch of other node information.
    /// However, I think that second role is not necessary: The ProjectGraphModel can
    /// go to the Project for almost all of the information.
    class NodeNodeModel : public QObject {
        Q_OBJECT

      public:
        NodeNodeModel(ProjectBridge& project, NodeId elementId);
        ~NodeNodeModel();

        unsigned int nPorts(QtNodes::PortType portType) const;
        QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const;
        QWidget* embeddedWidget();

        babelwires::NodeId getNodeId() const;
        QString caption() const;

        const Path& getPathAtPort(AccessModelScope& scope, QtNodes::PortType portType,
                                         QtNodes::PortIndex portIndex) const;
        QtNodes::PortIndex getPortAtPath(AccessModelScope& scope, QtNodes::PortType portType,
                                         const Path& path) const;

        ResizableAxes resizable() const { return HorizontallyResizable; }

        void setSize(const UiSize& newSize);

        /// Return the model for the embedded widget.
        NodeContentsModel& getModel();

        void addInConnection(const ConnectionId& connectionId);
        void addOutConnection(const ConnectionId& connectionId);
        std::unordered_set<ConnectionId> getAllConnectionIds() const;
        std::unordered_set<ConnectionId> getConnections(PortType portType, PortIndex index) const;
        bool isInConnection(const ConnectionId& connectionId) const;
        bool isOutConnection(const ConnectionId& connectionId) const;

      public slots:
        void customContextMenuRequested(const QPoint& pos);

      protected:
        /// Set up the contents to match the state of the node.
        void setContents(std::string label, NodeId nodeId);

        static QtNodes::NodeDataType getDataTypeFromTreeValueNode(const ValueTreeNode* f);

        const ValueTreeNode* getInput(AccessModelScope& scope, int portIndex) const;
        const ValueTreeNode* getOutput(AccessModelScope& scope, int portIndex) const;

      protected:
        babelwires::NodeId m_nodeId;

        // Cache knowledge about connections: Collapsed rows makes it awkward to extract from the model
        // TODO: Push knowledge about collapsed collections in to the data layer.
        std::vector<QtNodes::ConnectionId> m_inConnections;
        std::vector<QtNodes::ConnectionId> m_outConnections;

        NodeContentsView* m_view;
        NodeContentsModel* m_model;
        RowModelDelegate* m_delegate;
    };

} // namespace babelwires
