/**
 * The command which sets the size of an array.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/setArraySizeCommand.hpp>

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreeHelper.hpp>
#include <BabelWiresLib/Project/Commands/Subcommands/removeAllEditsSubcommand.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <cassert>

babelwires::SetArraySizeCommand::SetArraySizeCommand(std::string commandName, NodeId nodeId,
                                                     Path featurePath, int newSize)
    : CompoundCommand(commandName)
    , m_nodeId(nodeId)
    , m_pathToArray(std::move(featurePath))
    , m_newSize(newSize) {}

bool babelwires::SetArraySizeCommand::initializeAndExecute(Project& project) {
    const Node* nodeToModify = project.getNode(m_nodeId);

    if (!nodeToModify) {
        return false;
    }

    const ValueTreeNode* const input = nodeToModify->getInput();
    if (!input) {
        return false;
    }

    auto [compoundFeature, currentSize, range, initialSize] = ValueTreeHelper::getInfoFromArray(tryFollowPath(m_pathToArray, *input));

    if (!compoundFeature) {
        return false;
    }

    if (!range.contains(m_newSize)) {
        return false;
    }

    if (const Modifier* modifier = nodeToModify->getEdits().findModifier(m_pathToArray)) {
        const auto& modifierData = modifier->getModifierData();
        if (modifier->getModifierData().as<ArraySizeModifierData>()) {
            m_wasModifier = true;
        }
    }

    m_oldSize = compoundFeature->getNumChildren();

    for (int i = m_newSize; i < m_oldSize; ++i) {
        Path p = m_pathToArray;
        p.pushStep(i);
        addSubCommand(std::make_unique<RemoveAllEditsSubcommand>(m_nodeId, p));
    }
    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }
    executeBody(project);
    return true;
}

void babelwires::SetArraySizeCommand::executeBody(Project& project) const {
    if (m_newSize < m_oldSize) {
        project.removeArrayEntries(m_nodeId, m_pathToArray, m_newSize, m_oldSize - m_newSize, true);
    } else if (m_oldSize < m_newSize) {
        project.addArrayEntries(m_nodeId, m_pathToArray, m_oldSize, m_newSize - m_oldSize, true);
    } else {
        ArraySizeModifierData arraySizeModifierData;
        arraySizeModifierData.m_targetPath = m_pathToArray;
        arraySizeModifierData.m_size = m_newSize;
        project.addModifier(m_nodeId, arraySizeModifierData);
    }
}

void babelwires::SetArraySizeCommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    executeBody(project);
}

void babelwires::SetArraySizeCommand::undo(Project& project) const {
    if (m_newSize < m_oldSize) {
        project.addArrayEntries(m_nodeId, m_pathToArray, m_newSize, m_oldSize - m_newSize, m_wasModifier);
    } else if (m_oldSize < m_newSize) {
        project.removeArrayEntries(m_nodeId, m_pathToArray, m_oldSize, m_newSize - m_oldSize, m_wasModifier);
    } else {
        project.removeModifier(m_nodeId, m_pathToArray);
    }
    CompoundCommand::undo(project);
}
