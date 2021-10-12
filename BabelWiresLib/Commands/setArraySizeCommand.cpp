/**
 * The command which removes an entry from an array.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Commands/setArraySizeCommand.hpp"

#include "BabelWiresLib/Commands/removeAllEditsCommand.hpp"
#include "BabelWiresLib/Features/arrayFeature.hpp"
#include "BabelWiresLib/Features/recordFeature.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include <cassert>

babelwires::SetArraySizeCommand::SetArraySizeCommand(std::string commandName, ElementId elementId,
                                                     FeaturePath featurePath, int newSize)
    : CompoundCommand(commandName)
    , m_elementId(elementId)
    , m_pathToArray(std::move(featurePath))
    , m_newSize(newSize) {}

bool babelwires::SetArraySizeCommand::initializeAndExecute(Project& project) {
    const FeatureElement* elementToModify = project.getFeatureElement(m_elementId);

    if (!elementToModify) {
        return false;
    }

    const Feature* const inputFeature = elementToModify->getInputFeature();
    if (!inputFeature) {
        return false;
    }

    auto arrayFeature = m_pathToArray.tryFollow(*inputFeature)->as<const ArrayFeature>();
    if (!arrayFeature) {
        return false;
    }

    if (!arrayFeature->getSizeRange().contains(m_newSize)) {
        return false;
    }

    if (const Modifier* modifier = elementToModify->getEdits().findModifier(m_pathToArray)) {
        const auto& modifierData = modifier->getModifierData();
        if (modifier->getModifierData().as<ArraySizeModifierData>()) {
            m_wasModifier = true;
        }
    }

    m_oldSize = arrayFeature->getNumFeatures();

    for (int i = m_newSize; i < m_oldSize; ++i) {
        FeaturePath p = m_pathToArray;
        p.pushStep(PathStep(i));
        addSubCommand(std::make_unique<RemoveAllEditsCommand>("Set array size subcommand", m_elementId, p));
    }
    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }
    executeBody(project);
    return true;
}

void babelwires::SetArraySizeCommand::executeBody(Project& project) const {
    if (m_newSize < m_oldSize) {
        project.removeArrayEntries(m_elementId, m_pathToArray, m_newSize, m_oldSize - m_newSize, true);
    } else if (m_oldSize < m_newSize) {
        project.addArrayEntries(m_elementId, m_pathToArray, m_oldSize, m_newSize - m_oldSize, true);
    } else {
        ArraySizeModifierData arraySizeModifierData;
        arraySizeModifierData.m_pathToFeature = m_pathToArray;
        arraySizeModifierData.m_size = m_newSize;
        project.addModifier(m_elementId, arraySizeModifierData);
    }
}

void babelwires::SetArraySizeCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    executeBody(project);
}

void babelwires::SetArraySizeCommand::undo(Project& project) const {
    if (m_newSize < m_oldSize) {
        project.addArrayEntries(m_elementId, m_pathToArray, m_newSize, m_oldSize - m_newSize, m_wasModifier);
    } else if (m_oldSize < m_newSize) {
        project.removeArrayEntries(m_elementId, m_pathToArray, m_oldSize, m_newSize - m_oldSize, m_wasModifier);
    } else {
        project.removeModifier(m_elementId, m_pathToArray);
    }
    CompoundCommand::undo(project);
}
