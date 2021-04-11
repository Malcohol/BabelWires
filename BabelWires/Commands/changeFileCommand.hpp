/**
 * The command which changes the file of a FileElement.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Commands/commands.hpp"

namespace babelwires {

    /// Change the source file of a FileElement.
    class ChangeFileCommand : public SimpleCommand {
      public:
        ChangeFileCommand(std::string commandName, ElementId elementId, std::string newFilePath);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ElementId m_elementId;
        std::string m_newFilePath;
        std::string m_oldFilePath;
    };

} // namespace babelwires
