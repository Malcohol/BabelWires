/**
 * A RootFeature is a feature intended to exist at the root of a feature hierarchy.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/recordFeature.hpp>

namespace babelwires {
    struct ProjectContext;

    /// RootFeatures are found at the root of a feature hierarchy, and make additional data
    /// available to the children.
    class RootFeature : public RecordFeature {
      public:
        RootFeature(const ProjectContext& projectContext);

        /// Get the project context carried by this object.
        const ProjectContext& getProjectContextAtRoot() const;

      private:
        const ProjectContext& m_projectContext;
    };
} // namespace babelwires