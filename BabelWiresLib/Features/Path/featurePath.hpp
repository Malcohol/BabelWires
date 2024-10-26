/**
 * Describes the steps to follow within a tree of features to reach a particular feature.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/Path/pathStep.hpp>

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace babelwires {

    class Feature;
    class SimpleValueFeature;

    /// Describes the steps to follow within a tree of features to reach a particular feature.
    class FeaturePath {
      public:
        /// Construct an empty path.
        FeaturePath();

        /// Construct a feature path which leads to the given feature.
        explicit FeaturePath(const Feature* feature);

        FeaturePath(FeaturePath&& other) = default;

        FeaturePath(const FeaturePath& other) = default;

        /// Construct a path from a vector of steps.
        FeaturePath(std::vector<PathStep> steps);

        FeaturePath& operator=(const FeaturePath& other) = default;

        FeaturePath& operator=(FeaturePath&& other) = default;

        template <typename ROOT_VALUE_FEATURE> struct RootAndPath;

        /// All value features must be below a single SimpleValueFeature.
        static RootAndPath<const SimpleValueFeature> getRootAndPath(const Feature& feature);

        /// All value features must be below a single SimpleValueFeature.
        static RootAndPath<SimpleValueFeature> getRootAndPath(Feature& feature);

        /// Add a step to the path.
        void pushStep(PathStep step);

        /// Remove a step from the end of the path.
        void popStep();

        /// Get a serializable representation of the path.
        std::string serializeToString() const;

        /// Parse a serialized representation of a path.
        static FeaturePath deserializeFromString(const std::string& pathString);

        /// Throws if the path cannot be followed.
        Feature& follow(Feature& start) const;
        const Feature& follow(const Feature& start) const;

        /// Returns nullptr if the path cannot be followed.
        Feature* tryFollow(Feature& start) const;
        const Feature* tryFollow(const Feature& start) const;

        bool operator==(const FeaturePath& other) const;
        bool operator!=(const FeaturePath& other) const;
        bool operator<(const FeaturePath& other) const;
        bool operator<=(const FeaturePath& other) const;

        /// Does the path lead to the feature described by other, or one of its ancestors.
        bool isPrefixOf(const FeaturePath& other) const;

        /// Does the path lead to one of its ancestors of the feature described by other.
        bool isStrictPrefixOf(const FeaturePath& other) const;

        unsigned int getNumSteps() const;

        /// Set the new size to be newNumSteps, which must be less than or equal to the current number of steps.
        void truncate(unsigned int newNumSteps);

        /// Get the subpath starting at the given index.
        void removePrefix(unsigned int numSteps);

        /// Append the subpath to this path.
        void append(const FeaturePath& subpath);

        /// Get the ith step of the path. Asserts that i is valid.
        const PathStep& getStep(unsigned int i) const;

        /// Get the ith step of the path. Asserts that i is valid.
        PathStep& getStep(unsigned int i);

        /// Gets the last step. Asserts that there is one.
        const PathStep& getLastStep() const;

        /// Get a hash of this path.
        std::size_t getHash() const;

        using const_iterator = std::vector<PathStep>::const_iterator;
        const_iterator begin() const { return m_steps.begin(); }
        const_iterator end() const { return m_steps.end(); }

        using iterator = std::vector<PathStep>::iterator;
        iterator begin() { return m_steps.begin(); }
        iterator end() { return m_steps.end(); }

      private:
        /// Returns -1, 0 or 1.
        int compare(const FeaturePath& other) const;

      private:
        std::vector<PathStep> m_steps;
    };

    /// Write a path to an ostream.
    std::ostream& operator<<(std::ostream& os, const FeaturePath& p);

    template <typename ROOT_VALUE_FEATURE> struct FeaturePath::RootAndPath {
        ROOT_VALUE_FEATURE& m_root;
        FeaturePath m_pathFromRoot;
    };

} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::FeaturePath> {
        inline std::size_t operator()(const babelwires::FeaturePath& path) const { return path.getHash(); }
    };
} // namespace std
