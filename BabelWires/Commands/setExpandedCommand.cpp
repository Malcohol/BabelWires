/**
 * The command which expands or collapses a CompoundFeature of a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Commands/setExpandedCommand.hpp"

#include "BabelWires/Features/features.hpp"
#include "BabelWires/Features/recordFeature.hpp"
#include "BabelWires/Project/FeatureElements/featureElement.hpp"
#include "BabelWires/Project/project.hpp"

#include <cassert>

babelwires::SetExpandedCommand::SetExpandedCommand(std::string commandName, ElementId elementId,
                                                   FeaturePath pathToCompound, bool expanded)
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

    const CompoundFeature* compoundFeature = nullptr;
    if (const Feature* feature = element->getInputFeature()) {
        compoundFeature = m_pathToCompound.tryFollow(*feature)->asA<const CompoundFeature>();
    }
    if (!compoundFeature) {
        if (const Feature* feature = element->getOutputFeature()) {
            compoundFeature = m_pathToCompound.tryFollow(*feature)->asA<const CompoundFeature>();
        }
    }
    if (!compoundFeature) {
        return false;
    }

    return true;
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
