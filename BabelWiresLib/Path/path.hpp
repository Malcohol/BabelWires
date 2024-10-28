/**
 * Describes the steps to follow within a ValueTree to reach a particular ValueTreeNode.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Path/pathStep.hpp>

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace babelwires {

    class ValueTreeNode;
    class ValueTreeRoot;

    /// Describes the steps to follow within a ValueTree to reach a particular ValueTreeNode.
    class Path {
      public:
        /// Construct an empty path.
        Path();

        /// Construct a feature path which leads to the given feature.
        explicit Path(const ValueTreeNode* valueTreeNode);

        Path(Path&& other) = default;

        Path(const Path& other) = default;

        /// Construct a path from a vector of steps.
        Path(std::vector<PathStep> steps);

        Path& operator=(const Path& other) = default;

        Path& operator=(Path&& other) = default;

        template <typename VALUE_TREE_ROOT> struct RootAndPath;

        /// Get a path and the root.
        static RootAndPath<const ValueTreeRoot> getRootAndPath(const ValueTreeNode& valueTreeNode);

        /// Get a path and the root.
        static RootAndPath<ValueTreeRoot> getRootAndPath(ValueTreeNode& valueTreeNode);

        /// Add a step to the path.
        void pushStep(PathStep step);

        /// Remove a step from the end of the path.
        void popStep();

        /// Get a serializable representation of the path.
        std::string serializeToString() const;

        /// Parse a serialized representation of a path.
        static Path deserializeFromString(const std::string& pathString);

        /// Throws if the path cannot be followed.
        ValueTreeNode& follow(ValueTreeNode& start) const;
        const ValueTreeNode& follow(const ValueTreeNode& start) const;

        /// Returns nullptr if the path cannot be followed.
        ValueTreeNode* tryFollow(ValueTreeNode& start) const;
        const ValueTreeNode* tryFollow(const ValueTreeNode& start) const;

        bool operator==(const Path& other) const;
        bool operator!=(const Path& other) const;
        bool operator<(const Path& other) const;
        bool operator<=(const Path& other) const;

        /// Does the path lead to the ValueTreeNode described by other, or one of its ancestors.
        bool isPrefixOf(const Path& other) const;

        /// Does the path lead to one of its ancestors of the ValueTreeNode described by other.
        bool isStrictPrefixOf(const Path& other) const;

        unsigned int getNumSteps() const;

        /// Set the new size to be newNumSteps, which must be less than or equal to the current number of steps.
        void truncate(unsigned int newNumSteps);

        /// Get the subpath starting at the given index.
        void removePrefix(unsigned int numSteps);

        /// Append the subpath to this path.
        void append(const Path& subpath);

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
        int compare(const Path& other) const;

      private:
        std::vector<PathStep> m_steps;
    };

    /// Write a path to an ostream.
    std::ostream& operator<<(std::ostream& os, const Path& p);

    template <typename VALUE_TREE_ROOT> struct Path::RootAndPath {
        VALUE_TREE_ROOT& m_root;
        Path m_pathFromRoot;
    };

} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::Path> {
        inline std::size_t operator()(const babelwires::Path& path) const { return path.getHash(); }
    };
} // namespace std
