/**
 * Commands which removes modifiers from a node.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/Subcommands/removeSimpleModifierSubcommand.hpp>

#include <BabelWiresLib/Project/Commands/addEntriesToArrayCommand.hpp>
#include <BabelWiresLib/Project/Commands/deactivateOptionalCommand.hpp>
#include <BabelWiresLib/Project/Commands/removeEntryFromArrayCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/ValueTree/valueTreeHelper.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

#include <cassert>

babelwires::RemoveSimpleModifierSubcommand::RemoveSimpleModifierSubcommand(NodeId targetId, Path featurePath)
    : SimpleCommand("RemoveSimpleModifierSubcommand")
    , m_targetNodeId(targetId)
    , m_targetPath(std::move(featurePath)) {}

babelwires::RemoveSimpleModifierSubcommand::RemoveSimpleModifierSubcommand(const RemoveSimpleModifierSubcommand& other)
    : SimpleCommand(other)
    , m_targetNodeId(other.m_targetNodeId)
    , m_targetPath(other.m_targetPath)
    , m_modifierToRestore(other.m_modifierToRestore ? other.m_modifierToRestore->clone() : nullptr) {}

bool babelwires::RemoveSimpleModifierSubcommand::initialize(const Project& project) {
    const Node* node = project.getNode(m_targetNodeId);
    if (!node) {
        return false;
    }

    const Modifier* modifier = node->findModifier(m_targetPath);
    if (!modifier) {
        return false;
    }

    m_modifierToRestore = modifier->getModifierData().clone();
    return true;
}

void babelwires::RemoveSimpleModifierSubcommand::execute(Project& project) const {
    project.removeModifier(m_targetNodeId, m_targetPath);
}

void babelwires::RemoveSimpleModifierSubcommand::undo(Project& project) const {
    project.addModifier(m_targetNodeId, *m_modifierToRestore);
}

babelwires::NodeId babelwires::RemoveSimpleModifierSubcommand::getTargetNodeId() const {
    return m_targetNodeId;
}

const babelwires::Path& babelwires::RemoveSimpleModifierSubcommand::getTargetPath() const {
    return m_targetPath;
}
