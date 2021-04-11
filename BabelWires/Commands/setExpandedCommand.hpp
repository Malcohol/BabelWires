/**
 * The command which expands or collapses a CompoundFeature of a FeatureElement.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Commands/commands.hpp"

namespace babelwires {

    class SetExpandedCommand : public SimpleCommand {
      public:
        SetExpandedCommand(std::string commandName, ElementId elementId, FeaturePath pathToCompound, bool expanded);

        virtual bool initialize(const Project& project) override;
        virtual void execute(Project& project) const override;
        virtual void undo(Project& project) const override;

      private:
        ElementId m_elementId;
        FeaturePath m_pathToCompound;
        bool m_expanded;
    };

} // namespace babelwires
