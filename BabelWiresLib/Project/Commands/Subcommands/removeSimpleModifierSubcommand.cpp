/**
 * Commands which removes modifiers from an element.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/Subcommands/removeSimpleModifierSubcommand.hpp>

#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/recordWithOptionalsFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Features/valueFeatureHelper.hpp>
#include <BabelWiresLib/Project/Commands/addEntriesToArrayCommand.hpp>
#include <BabelWiresLib/Project/Commands/deactivateOptionalCommand.hpp>
#include <BabelWiresLib/Project/Commands/removeEntryFromArrayCommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

#include <cassert>

babelwires::RemoveSimpleModifierSubcommand::RemoveSimpleModifierSubcommand(ElementId targetId,
                                                                     FeaturePath featurePath)
    : SimpleCommand("RemoveSimpleModifierSubcommand")
    , m_targetElementId(targetId)
    , m_featurePath(std::move(featurePath)) {}

bool babelwires::RemoveSimpleModifierSubcommand::initialize(const Project& project) {
    const FeatureElement* element = project.getFeatureElement(m_targetElementId);
    if (!element) {
        return false;
    }

    const Modifier* modifier = element->findModifier(m_featurePath);
    if (!modifier) {
        return false;
    }

    m_modifierToRestore = modifier->getModifierData().clone();
    return true;
}

void babelwires::RemoveSimpleModifierSubcommand::execute(Project& project) const {
    project.removeModifier(m_targetElementId, m_featurePath);
}

void babelwires::RemoveSimpleModifierSubcommand::undo(Project& project) const {
    project.addModifier(m_targetElementId, *m_modifierToRestore);
}

babelwires::ElementId babelwires::RemoveSimpleModifierSubcommand::getTargetElementId() const {
    return m_targetElementId;
}

const babelwires::FeaturePath& babelwires::RemoveSimpleModifierSubcommand::getFeaturePath() const {
    return m_featurePath;
}
