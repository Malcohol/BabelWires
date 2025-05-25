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

#include <QSize>

#include <vector>
#include <unordered_set>

namespace babelwires {
    class ValueTreeNode;
    class Node;
    class ContentsCache;
    class Path;
    struct ConnectionDescription;
    class Type;

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

        const Type* getInputType(const AccessModelScope& scope, QtNodes::PortIndex portIndex) const;
        const Type* getOutputType(const AccessModelScope& scope, QtNodes::PortIndex portIndex) const;

        const QWidget* getEmbeddedWidget() const;
        QWidget* getEmbeddedWidget();

        QString caption(const AccessModelScope& scope) const;

        const Path& getPathAtPort(const AccessModelScope& scope, QtNodes::PortType portType,
                                         QtNodes::PortIndex portIndex) const;
        QtNodes::PortIndex getPortAtPath(const AccessModelScope& scope, QtNodes::PortType portType,
                                         const Path& path) const;

        QSize getCachedSize() const;
        void setCachedSize(QSize newSize);

        void addInConnection(const QtNodes::ConnectionId& connectionId, const ConnectionDescription& connectionDescription);
        void addOutConnection(const QtNodes::ConnectionId& connectionId, const ConnectionDescription& connectionDescription);
        
        QtNodes::ConnectionId removeInConnection(const ConnectionDescription& connectionDescription);
        QtNodes::ConnectionId removeOutConnection(const ConnectionDescription& connectionDescription);

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
        std::vector<std::tuple<QtNodes::ConnectionId, ConnectionDescription>> m_inConnections;
        std::vector<std::tuple<QtNodes::ConnectionId, ConnectionDescription>> m_outConnections;

        NodeContentsView* m_view;
        NodeContentsModel* m_model;
        RowModelDelegate* m_delegate;

        /// The model is expected to cache the size.
        QSize m_cachedSize;
    };

} // namespace babelwires
