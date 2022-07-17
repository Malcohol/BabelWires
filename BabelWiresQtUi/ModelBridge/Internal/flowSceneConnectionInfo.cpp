/**
 * Information about a connection that exists between two nodes.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/Internal/flowSceneConnectionInfo.hpp>

babelwires::FlowSceneConnectionInfo::FlowSceneConnectionInfo(const QtNodes::Connection& c)
    : m_inNode(c.getNode(QtNodes::PortType::In))
    , m_inPort(c.getPortIndex(QtNodes::PortType::In))
    , m_outNode(c.getNode(QtNodes::PortType::Out))
    , m_outPort(c.getPortIndex(QtNodes::PortType::Out)) {}

bool babelwires::FlowSceneConnectionInfo::isSameFlowSceneConnection(const QtNodes::Connection& other) const {
    return (m_inNode == other.getNode(QtNodes::PortType::In)) &&
           (m_inPort == other.getPortIndex(QtNodes::PortType::In)) &&
           (m_outNode == other.getNode(QtNodes::PortType::Out)) &&
           (m_outPort == other.getPortIndex(QtNodes::PortType::Out));
}
