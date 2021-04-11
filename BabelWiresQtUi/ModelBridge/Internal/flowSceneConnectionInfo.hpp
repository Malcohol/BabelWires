/**
 * Information about a connection that exists between two nodes.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <nodes/Node>

namespace babelwires {

    /// Information about a connection that exists between two nodes, used when
    /// interpreting the onConnectionXXX events.
    /// These infos exist for a subset of the QtNodes::connection objects, since
    /// the latter can exist when only one end is connected.
    struct FlowSceneConnectionInfo {
        /// Construct an info from an existing connection.
        FlowSceneConnectionInfo(const QtNodes::Connection& c);

        /// Compare this info with the given connection.
        bool isSameFlowSceneConnection(const QtNodes::Connection& other) const;

        const QtNodes::Node* m_inNode;
        QtNodes::PortIndex m_inPort;
        const QtNodes::Node* m_outNode;
        QtNodes::PortIndex m_outPort;
    };

} // namespace babelwires
