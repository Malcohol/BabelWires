/**
 * A CompoundFeature is a feature which contains other features.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/feature.hpp>

namespace babelwires {

    /// A feature which contains other features.
    /// See function subfeatures in modelUtilities for support for iterators and ranged for loops.
    class CompoundFeature : public Feature {
      public:
        virtual int getNumFeatures() const = 0;
        Feature* getFeature(int i);
        const Feature* getFeature(int i) const;

        virtual PathStep getStepToChild(const Feature* child) const = 0;

        /// Should return nullptr if the step does not lead to a child.
        Feature* tryGetChildFromStep(const PathStep& step);

        /// Should return nullptr if the step does not lead to a child.
        const Feature* tryGetChildFromStep(const PathStep& step) const;

        /// Throws a ModelException if the step does not lead to a child.
        Feature& getChildFromStep(const PathStep& step);

        /// Throws a ModelException if the step does not lead to a child.
        const Feature& getChildFromStep(const PathStep& step) const;

        /// Returns -1 if not found.
        /// Sets the descriminator of identifier on a match.
        virtual int getChildIndexFromStep(const PathStep& step) const = 0;

      protected:
        /// Clears the changes of this class and all children.
        virtual void doClearChanges() override;
        virtual Feature* doGetFeature(int i) = 0;
        virtual const Feature* doGetFeature(int i) const = 0;

      protected:
        /// Call setToDefault on each subfeature.
        void setSubfeaturesToDefault();
    };

}