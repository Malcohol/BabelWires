/**
 * The command which expands or collapses a Feature of a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/setExpandedCommand.hpp>

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <cassert>

babelwires::SetExpandedCommand::SetExpandedCommand(std::string commandName, ElementId elementId,
                                                   Path pathToCompound, bool expanded)
    : SimpleCommand(std::move(commandName))
    , m_elementId(elementId)
    , m_pathToCompound(std::move(pathToCompound))
    , m_expanded(expanded) {}

bool babelwires::SetExpandedCommand::initialize(const Project& project) {
    const FeatureElement* element = project.getFeatureElement(m_elementId);
    if (!element) {
        return false;
    }

    if (element->isExpanded(m_pathToCompound) == m_expanded) {
        return false;
    }

    const ValueTreeNode* compoundFeature = nullptr;
    if (const ValueTreeNode* feature = element->getInput()) {
        compoundFeature = m_pathToCompound.tryFollow(*feature);
    }
    if (!compoundFeature) {
        if (const ValueTreeNode* feature = element->getOutput()) {
            compoundFeature = m_pathToCompound.tryFollow(*feature);
        }
    }
    if (!compoundFeature) {
        return false;
    }

    return compoundFeature->getNumChildren() > 0;
}

void babelwires::SetExpandedCommand::execute(Project& project) const {
    FeatureElement* element = project.getFeatureElement(m_elementId);
    assert(element);
    element->setExpanded(m_pathToCompound, m_expanded);
}

void babelwires::SetExpandedCommand::undo(Project& project) const {
    FeatureElement* element = project.getFeatureElement(m_elementId);
    assert(element);
    element->setExpanded(m_pathToCompound, !m_expanded);
}
