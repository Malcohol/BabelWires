/**
 * The command which removes an entry from an array.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Commands/Subcommands/adjustModifiersInArraySubcommand.hpp>

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Project/Commands/Subcommands/removeAllEditsSubcommand.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/projectUtilities.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

#include <cassert>

babelwires::AdjustModifiersInArraySubcommand::AdjustModifiersInArraySubcommand(
    NodeId elementId, const babelwires::Path& pathToArray,
    babelwires::ArrayIndex startIndex, int adjustment)
    : CompoundCommand("AdjustModifiersInArraySubcommand")
    , m_nodeId(elementId)
    , m_pathToArray(std::move(pathToArray))
    , m_startIndex(startIndex)
    , m_adjustment(adjustment) {
    assert((m_adjustment != 0) && "There must be some adjustment");
}

bool babelwires::AdjustModifiersInArraySubcommand::initializeAndExecute(Project& project) {
    const Node* nodeToModify = project.getNode(m_nodeId);

    if (!nodeToModify) {
        return false;
    }

    if (m_adjustment < 0) {
        for (int i = 0; i < -m_adjustment; ++i) {
            Path p = m_pathToArray;
            p.pushStep(PathStep(m_startIndex + i));
            addSubCommand(std::make_unique<RemoveAllEditsSubcommand>(m_nodeId, p));
        }
    }

    auto derivedArrays = projectUtilities::getDerivedValues(project, m_nodeId, m_pathToArray);

    for (auto a : derivedArrays) {
        const NodeId elementId = std::get<0>(a);
        const Path& pathToArray = std::get<1>(a);
        addSubCommand(std::make_unique<AdjustModifiersInArraySubcommand>(elementId, pathToArray, m_startIndex, m_adjustment));
    }

    if (!CompoundCommand::initializeAndExecute(project)) {
        return false;
    }

    project.adjustModifiersInArrayElements(m_nodeId, m_pathToArray, m_startIndex, m_adjustment);

    return true;
}

void babelwires::AdjustModifiersInArraySubcommand::execute(Project& project) const {
    CompoundCommand::execute(project);
    project.adjustModifiersInArrayElements(m_nodeId, m_pathToArray, m_startIndex, m_adjustment);
}

void babelwires::AdjustModifiersInArraySubcommand::undo(Project& project) const {
    project.adjustModifiersInArrayElements(m_nodeId, m_pathToArray, m_startIndex, -m_adjustment);
    CompoundCommand::undo(project);
}
