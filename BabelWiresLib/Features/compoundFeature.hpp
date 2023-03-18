/**
 * A CompoundFeature is a feature which contains other features.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/features.hpp>

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
        virtual Feature* tryGetChildFromStep(const PathStep& step) = 0;

        /// Should return nullptr if the step does not lead to a child.
        virtual const Feature* tryGetChildFromStep(const PathStep& step) const = 0;

        /// Throws a ModelException if the step does not lead to a child.
        Feature& getChildFromStep(const PathStep& step);

        /// Throws a ModelException if the step does not lead to a child.
        const Feature& getChildFromStep(const PathStep& step) const;

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