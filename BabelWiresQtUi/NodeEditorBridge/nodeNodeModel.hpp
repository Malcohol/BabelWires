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
#include <unordered_set>

namespace babelwires {
    class ValueTreeNode;
    class Node;
    class ContentsCache;
    class Path;

    class NodeContentsView;
    class NodeContentsModel;
    class RowModelDelegate;
    class AccessModelScope;
    class ProjectGraphModel;

    /// This manages the View, Model and Delegate of a Node's embedded widget.
    /// TODO: Right now, it also manages access to a bunch of other node information.
    /// However, I think that second role is not necessary: The ProjectGraphModel can
    /// go to the Project for almost all of the information.
    class NodeNodeModel : public QObject {
        Q_OBJECT

      public:
        NodeNodeModel(ProjectGraphModel& project, NodeId nodeId);
        ~NodeNodeModel();

        unsigned int nPorts(const AccessModelScope& scope, QtNodes::PortType portType) const;
        QtNodes::NodeDataType dataType(const AccessModelScope& scope, QtNodes::PortType portType, QtNodes::PortIndex portIndex) const;

        const QWidget* getEmbeddedWidget() const;
        QWidget* getEmbeddedWidget();

        QString caption(const AccessModelScope& scope) const;

        const Path& getPathAtPort(const AccessModelScope& scope, QtNodes::PortType portType,
                                         QtNodes::PortIndex portIndex) const;
        QtNodes::PortIndex getPortAtPath(const AccessModelScope& scope, QtNodes::PortType portType,
                                         const Path& path) const;

        //TODO ResizableAxes resizable() const { return HorizontallyResizable; }

        int getHeight() const;
        void setHeight(int newHeight);

        /// Return the model for the embedded widget.
        NodeContentsModel& getModel();

        void addInConnection(const QtNodes::ConnectionId& connectionId);
        void addOutConnection(const QtNodes::ConnectionId& connectionId);
        void removeInConnection(const QtNodes::ConnectionId& connectionId);
        void removeOutConnection(const QtNodes::ConnectionId& connectionId);

        std::unordered_set<QtNodes::ConnectionId> getAllConnectionIds() const;
        std::unordered_set<QtNodes::ConnectionId> getConnections(QtNodes::PortType portType, QtNodes::PortIndex index) const;
        bool isInConnection(const QtNodes::ConnectionId& connectionId) const;
        bool isOutConnection(const QtNodes::ConnectionId& connectionId) const;

      public slots:
        void customContextMenuRequested(const QPoint& pos);

      protected:
        static QtNodes::NodeDataType getDataTypeFromTreeValueNode(const ValueTreeNode* f);

        const ValueTreeNode* getInput(const AccessModelScope& scope, int portIndex) const;
        const ValueTreeNode* getOutput(const AccessModelScope& scope, int portIndex) const;

      protected:
        babelwires::NodeId m_nodeId;

        // Cache knowledge about connections: Collapsed rows makes it awkward to extract from the model
        // TODO: Push knowledge about collapsed collections in to the data layer.
        std::vector<QtNodes::ConnectionId> m_inConnections;
        std::vector<QtNodes::ConnectionId> m_outConnections;

        NodeContentsView* m_view;
        NodeContentsModel* m_model;
        RowModelDelegate* m_delegate;

        /// Width is user-editable, so it's stored in the data model. Height is calculated by NodeEditor code, but
        /// needs to be cached.
        int m_height;
    };

} // namespace babelwires
