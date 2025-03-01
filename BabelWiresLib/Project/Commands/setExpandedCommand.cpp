/**
 * The command which expands or collapses a ValueTreeNode.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/setExpandedCommand.hpp>

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <cassert>

babelwires::SetExpandedCommand::SetExpandedCommand(std::string commandName, NodeId nodeId,
                                                   Path pathToCompound, bool expanded)
    : SimpleCommand(std::move(commandName))
    , m_nodeId(nodeId)
    , m_pathToCompound(std::move(pathToCompound))
    , m_expanded(expanded) {}

bool babelwires::SetExpandedCommand::initialize(const Project& project) {
    const Node* node = project.getNode(m_nodeId);
    if (!node) {
        return false;
    }

    if (node->isExpanded(m_pathToCompound) == m_expanded) {
        return false;
    }

    const ValueTreeNode* compound = nullptr;
    if (const ValueTreeNode* valueTreeNode = node->getInput()) {
        compound = m_pathToCompound.tryFollow(*valueTreeNode);
    }
    if (!compound) {
        if (const ValueTreeNode* valueTreeNode = node->getOutput()) {
            compound = m_pathToCompound.tryFollow(*valueTreeNode);
        }
    }
    if (!compound) {
        return false;
    }

    return compound->getNumChildren() > 0;
}

void babelwires::SetExpandedCommand::execute(Project& project) const {
    Node* node = project.getNode(m_nodeId);
    assert(node);
    node->setExpanded(m_pathToCompound, m_expanded);
}

void babelwires::SetExpandedCommand::undo(Project& project) const {
    Node* node = project.getNode(m_nodeId);
    assert(node);
    node->setExpanded(m_pathToCompound, !m_expanded);
}
