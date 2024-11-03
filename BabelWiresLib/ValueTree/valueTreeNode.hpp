/**
 * A ValueTreeNode is the base class of nodes in the ValueTree.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Utilities/enumFlags.hpp>
#include <Common/multiKeyMap.hpp>
#include <Common/types.hpp>

#include <BabelWiresLib/Path/pathStep.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

#include <cassert>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace babelwires {
    class Type;
    class ValueTreeRoot;
    class ValueTreeChild;
    class Path;

    /// A ValueTreeNode is the base class of nodes in the ValueTree.
    /// A ValueTree holds a Value and adds:
    /// 1. Change management (i.e. recording when a value has changed)
    /// 2. Ownership (i.e. storing a parent pointer, which allows the paths to values to be constructed)
    /// Note that the underlying values can be shared, so they cannot carry this data.
    class ValueTreeNode {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(ValueTreeNode);

        ValueTreeNode(TypeRef typeRef, ValueHolder value);
        virtual ~ValueTreeNode();

        void setOwner(ValueTreeNode* owner);
        const ValueTreeNode* getOwner() const;
        ValueTreeNode* getOwnerNonConst();

        /// Set to the default value.
        void setToDefault();

        /// Describes the way a node may have changed.
        enum class Changes : unsigned int {
            NothingChanged = 0b0000,
            ValueChanged = 0b0001,
            StructureChanged = 0b0010,
            SomethingChanged = 0b0011
        };

        /// Has this node or its children changed in any of the ways described by changes?
        bool isChanged(Changes changes) const;

        /// Clear the change flags of this node and all its children.
        void clearChanges();

        /// Get a hash of the value.
        /// Note: The hash is not required to distinguish the contents of values of different types.
        std::size_t getHash() const;

        /// Get the TypeRef which describes the type of the value.
        const TypeRef& getTypeRef() const;

        /// Get the value currently held by this node.
        const ValueHolder& getValue() const;

        /// Set this node to hold a new value.
        void setValue(const ValueHolder& newValue);

        /// The root of a ValueTree carries a reference to the TypeSystem, so it can be found from any node.
        const TypeSystem& getTypeSystem() const;

        /// This is a convenience method which resolves the typeRef in the context of the TypeSystem
        /// carried by the root.
        const Type& getType() const;

        /// This is a convenience method which calls getType().getKind().
        /// The need for nodes to provide a string description is not fundamental to the data model:
        /// it is imposed by the (current) UI.
        /// Returning the empty string tells the project that values of this kind cannot be wired together.
        std::string getKind() const;

        /// Set this to hold the same value as other.
        /// This will throw a ModelException if the assignment failed.
        void assign(const ValueTreeNode& other);

      public:
        int getNumChildren() const;

        ValueTreeNode* getChild(int i);
        const ValueTreeNode* getChild(int i) const;

        PathStep getStepToChild(const ValueTreeNode* child) const;

        /// Should return nullptr if the step does not lead to a child.
        ValueTreeNode* tryGetChildFromStep(const PathStep& step);

        /// Should return nullptr if the step does not lead to a child.
        const ValueTreeNode* tryGetChildFromStep(const PathStep& step) const;

        /// Throws a ModelException if the step does not lead to a child.
        ValueTreeNode& getChildFromStep(const PathStep& step);

        /// Throws a ModelException if the step does not lead to a child.
        const ValueTreeNode& getChildFromStep(const PathStep& step) const;

        /// Returns -1 if not found.
        /// Sets the descriminator of identifier on a match.
        int getChildIndexFromStep(const PathStep& step) const;

      protected:
        /// Initialize the tree of nodes beneath this node.
        void initializeChildren();

        /// Update change flags and ensure the children match the value in other.
        void reconcileChangesAndSynchronizeChildren(const ValueHolder& other);

        /// Update change flags and ensure the children match the value in other.
        /// In this special case, the changes are known to lie at the end of path p.
        void reconcileChangesAndSynchronizeChildren(const ValueHolder& other, const Path& p);
 
      private:
        void reconcileChangesAndSynchronizeChildren(const ValueHolder& other, const Path& p, unsigned int pathIndex);

      protected:
        /// Set the isChanged flag and that of all parents.
        void setChanged(Changes changes);

      protected:
        virtual void doSetToDefault() = 0;
        virtual void doSetValue(const ValueHolder& newValue) = 0;

      private:
        // For now.
        ValueTreeNode(const ValueTreeNode&) = delete;
        ValueTreeNode& operator=(const ValueTreeNode&) = delete;

      private:
        /// The type of the value at this ValueTreeNode.
        TypeRef m_typeRef;

        /// The value at this ValueTreeNode. 
        /// Note: This should not be modified directly: all modifications should be managed via the ValueTreeRoot.
        ValueHolder m_value;

        ValueTreeNode* m_owner = nullptr;
        Changes m_changes = Changes::SomethingChanged;

        using ChildMap = MultiKeyMap<PathStep, unsigned int, std::unique_ptr<ValueTreeChild>>;
        ChildMap m_children;
    };

    DEFINE_ENUM_FLAG_OPERATORS(ValueTreeNode::Changes);

} // namespace babelwires
