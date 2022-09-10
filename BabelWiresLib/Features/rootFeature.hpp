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

        /// Return the RootFeature if feature has a RootFeature as its root.
        static const RootFeature* tryGetRootFeatureAt(const Feature& feature);

        /// Get a reference to the ProjectContext from the RootFeature at the root of the feature
        /// hierarchy. Asserts if there is no root.
        static const ProjectContext& getProjectContextAt(const Feature& feature);

        /// The root is not collapsable.
        Style getStyle() const override;

      private:
        const ProjectContext& m_projectContext;
    };
} // namespace babelwires