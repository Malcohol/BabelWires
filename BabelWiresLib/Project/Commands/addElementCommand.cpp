/**
 * The command which adds FeatureElements to the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWiresLib/Project/Commands/addElementCommand.hpp"

#include "BabelWiresLib/Project/Commands/moveElementCommand.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElementData.hpp"
#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include <cassert>

babelwires::AddElementCommand::AddElementCommand(std::string commandName, std::unique_ptr<ElementData> elementToAdd)
    : SimpleCommand(std::move(commandName))
    , m_elementToAdd(std::move(elementToAdd)) {}

void babelwires::AddElementCommand::execute(Project& project) const {
    ElementId newId = project.addFeatureElement(*m_elementToAdd);
    m_elementToAdd->m_id = newId;
}

bool babelwires::AddElementCommand::initialize(const Project& project) {
    return true;
}

babelwires::ElementId babelwires::AddElementCommand::getElementId() const {
    return m_elementToAdd->m_id;
}

void babelwires::AddElementCommand::undo(Project& project) const {
    project.removeElement(getElementId());
}

bool babelwires::AddElementCommand::shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const {
    const auto* moveElementCommand = subsequentCommand.as<MoveElementCommand>();
    if (!moveElementCommand) {
        return false;
    }
    if (!moveElementCommand->getPositionForOnlyNode(m_elementToAdd->m_id)) {
        return false;
    }
    if (!thisIsAlreadyExecuted) {
        return true;
    }

    // Subsume the command only if the commands happened close together.
    using namespace std::literals::chrono_literals;
    return ((moveElementCommand->getTimestamp() - getTimestamp()) < 500ms);
}

void babelwires::AddElementCommand::subsume(std::unique_ptr<Command> subsequentCommand) {
    assert(subsequentCommand->as<MoveElementCommand>() && "subsume should not have been called");
    MoveElementCommand* moveElementCommand = static_cast<MoveElementCommand*>(subsequentCommand.get());
    if (auto optionalPosition = moveElementCommand->getPositionForOnlyNode(m_elementToAdd->m_id)) {
        m_elementToAdd->m_uiData.m_uiPosition = *optionalPosition;
    }
}
