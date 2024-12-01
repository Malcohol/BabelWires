/**
 * The command which expands or collapses a ValueTreeNode.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/setExpandedCommand.hpp>

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Project/FeatureElements/node.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <cassert>

babelwires::SetExpandedCommand::SetExpandedCommand(std::string commandName, ElementId elementId,
                                                   Path pathToCompound, bool expanded)
    : SimpleCommand(std::move(commandName))
    , m_elementId(elementId)
    , m_pathToCompound(std::move(pathToCompound))
    , m_expanded(expanded) {}

bool babelwires::SetExpandedCommand::initialize(const Project& project) {
    const Node* element = project.getFeatureElement(m_elementId);
    if (!element) {
        return false;
    }

    if (element->isExpanded(m_pathToCompound) == m_expanded) {
        return false;
    }

    const ValueTreeNode* compound = nullptr;
    if (const ValueTreeNode* valueTreeNode = element->getInput()) {
        compound = m_pathToCompound.tryFollow(*valueTreeNode);
    }
    if (!compound) {
        if (const ValueTreeNode* valueTreeNode = element->getOutput()) {
            compound = m_pathToCompound.tryFollow(*valueTreeNode);
        }
    }
    if (!compound) {
        return false;
    }

    return compound->getNumChildren() > 0;
}

void babelwires::SetExpandedCommand::execute(Project& project) const {
    Node* element = project.getFeatureElement(m_elementId);
    assert(element);
    element->setExpanded(m_pathToCompound, m_expanded);
}

void babelwires::SetExpandedCommand::undo(Project& project) const {
    Node* element = project.getFeatureElement(m_elementId);
    assert(element);
    element->setExpanded(m_pathToCompound, !m_expanded);
}
