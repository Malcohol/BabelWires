/**
 * The command which adds FeatureElements to the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <BabelWiresLib/Commands/commands.hpp>
#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>

namespace babelwires {
    class Project;
    struct ElementData;

    /// Add a feature element to the project.
    class AddElementCommand : public SimpleCommand<Project> {
      public:
        AddElementCommand(std::string commandName, std::unique_ptr<ElementData> elementToAdd);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

        /// An add can subsume a MoveElementCommand.
        // TODO This would not be necessary if the UI waited for the engine to add elements before it added nodes.
        virtual bool shouldSubsume(const Command& subsequentCommand, bool thisIsAlreadyExecuted) const override;
        virtual void subsume(std::unique_ptr<Command> subsequentCommand) override;

        /// For use after addition.
        ElementId getElementId() const;

      private:
        std::unique_ptr<ElementData> m_elementToAdd;
    };

} // namespace babelwires
