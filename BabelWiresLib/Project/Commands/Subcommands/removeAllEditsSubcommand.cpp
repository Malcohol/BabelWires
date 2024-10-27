/**
 * The command which removes an entry from an array.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/Subcommands/removeAllEditsSubcommand.hpp>

#include <BabelWiresLib/Project/Commands/removeModifierCommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <cassert>

babelwires::RemoveAllEditsSubcommand::RemoveAllEditsSubcommand(ElementId elementId,
                                                                     Path pathToFeatureToRemove)
    : CompoundCommand("RemoveAllEditsSubcommand")
    , m_elementId(elementId)
    , m_pathToFeature(std::move(pathToFeatureToRemove)) {
}

bool babelwires::RemoveAllEditsSubcommand::initializeAndExecute(Project& project) {
    FeatureElement* elementToModify = project.getFeatureElement(m_elementId);

    if (!elementToModify) {
        return false;
    }

    std::vector<std::unique_ptr<Command>> subcommands;

    for (const auto& modifier : elementToModify->getEdits().modifierRange(m_pathToFeature)) {
        const auto& modifierData = modifier->getModifierData();
        subcommands.emplace_back(std::make_unique<RemoveModifierCommand>("Remove modifier subcommand", m_elementId, modifierData.m_targetPath));
    }

    {
        const Project::ConnectionInfo& connectionInfo = project.getConnectionInfo();
        auto it = connectionInfo.m_requiredFor.find(elementToModify);
        if (it != connectionInfo.m_requiredFor.end()) {
            for (auto&& connection : it->second) {
                const ConnectionModifier* const cmod = std::get<0>(connection);
                const ConnectionModifierData& modifierData = cmod->getModifierData();
                const Path& modifierPath = modifierData.m_sourcePath;
                if (m_pathToFeature.isPrefixOf(modifierPath)) {
                    const FeatureElement* const target = std::get<1>(connection);
                    subcommands.emplace_back(std::make_unique<RemoveModifierCommand>("Remove modifier subcommand", target->getElementId(), modifierData.m_targetPath));
                }
            }
        }
    }

    for (auto it = subcommands.rbegin(); it != subcommands.rend(); ++it) {
        addSubCommand(std::move(*it));
    }

    const auto pathsInThisEntry = elementToModify->getEdits().getAllExplicitlyExpandedPaths(m_pathToFeature);
    m_expandedPathsRemoved.insert(m_expandedPathsRemoved.end(), pathsInThisEntry.begin(),
                                               pathsInThisEntry.end());

    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }
    // This may not seem necessary, but it means we won't assert when expanding the
    // moved down entries.
    for (const auto& p : m_expandedPathsRemoved) {
        elementToModify->getEdits().setExpanded(p, false);
    }

    return true;
}

void babelwires::RemoveAllEditsSubcommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    FeatureElement* elementToModify = project.getFeatureElement(m_elementId);
    assert(elementToModify && "The element must exist");
    // This may not seem necessary, but it means we won't assert when expanding the
    // moved down entries.
    for (const auto& p : m_expandedPathsRemoved) {
        elementToModify->getEdits().setExpanded(p, false);
    }
}

void babelwires::RemoveAllEditsSubcommand::undo(Project& project) const {
    FeatureElement* elementToModify = project.getFeatureElement(m_elementId);
    assert(elementToModify && "The element must exist");
    for (const auto& p : m_expandedPathsRemoved) {
        elementToModify->getEdits().setExpanded(p, true);
    }
    CompoundCommand::undo(project);
}
