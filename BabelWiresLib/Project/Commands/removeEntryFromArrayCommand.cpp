/**
 * The command which removes an entry from an array.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/removeEntryFromArrayCommand.hpp>

#include <BabelWiresLib/Project/Commands/Subcommands/adjustModifiersInArraySubcommand.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/projectUtilities.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/ValueTree/valueTreeHelper.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <cassert>

babelwires::RemoveEntryFromArrayCommand::RemoveEntryFromArrayCommand(std::string commandName, NodeId nodeId,
                                                                     Path pathToArray,
                                                                     unsigned int indexOfEntryToRemove,
                                                                     unsigned int numEntriesToRemove)
    : CompoundCommand(commandName)
    , m_nodeId(nodeId)
    , m_pathToArray(std::move(pathToArray))
    , m_indexOfEntryToRemove(indexOfEntryToRemove)
    , m_numEntriesToRemove(numEntriesToRemove) {
    assert((numEntriesToRemove > 0) && "numEntriesToRemove must be strictly positive");
}

bool babelwires::RemoveEntryFromArrayCommand::initializeAndExecute(Project& project) {
    const Node* nodeToModify = project.getNode(m_nodeId);

    if (!nodeToModify) {
        return false;
    }

    const ValueTreeNode* const input = nodeToModify->getInput();
    if (!input) {
        return false;
    }

    auto [compoundFeature, currentSize, range, initialSize] =
        ValueTreeHelper::getInfoFromArray(tryFollowPath(m_pathToArray, *input));

    if (!compoundFeature) {
        return false;
    }

    const unsigned int minimumSize = range.m_min;

    if (currentSize < 0) {
        return false;
    }

    if (currentSize - m_numEntriesToRemove < minimumSize) {
        return false;
    }

    if (m_indexOfEntryToRemove > currentSize - 1) {
        return false;
    }

    if (const Modifier* modifier = nodeToModify->getEdits().findModifier(m_pathToArray)) {
        const auto& modifierData = modifier->getModifierData();
        if (modifier->getModifierData().tryAs<ArraySizeModifierData>()) {
            m_wasModifier = true;
        }
    }

    addSubCommand(std::make_unique<AdjustModifiersInArraySubcommand>(m_nodeId, m_pathToArray, m_indexOfEntryToRemove,
                                                                     -m_numEntriesToRemove));

    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }
    project.removeArrayEntries(m_nodeId, m_pathToArray, m_indexOfEntryToRemove, m_numEntriesToRemove, !m_isSubcommand);
    return true;
}

void babelwires::RemoveEntryFromArrayCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    project.removeArrayEntries(m_nodeId, m_pathToArray, m_indexOfEntryToRemove, m_numEntriesToRemove, !m_isSubcommand);
}

void babelwires::RemoveEntryFromArrayCommand::undo(Project& project) const {
    project.addArrayEntries(m_nodeId, m_pathToArray, m_indexOfEntryToRemove, m_numEntriesToRemove, m_wasModifier);
    CompoundCommand::undo(project);
}
