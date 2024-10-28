/**
 * A ValueTreeRoot is a ValueTreeNode that owns its value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

namespace babelwires {
    class Type;
    class Value;
    class Path;

    /// A ValueTreeRoot is a ValueTreeNode that owns its value.
    class ValueTreeRoot : public ValueTreeNode {
      public:
        /// Construct a rooted ValueTreeNode which carries values of the given type.
        ValueTreeRoot(const TypeSystem& typeSystem, TypeRef typeRef);

        /// Back up the current value using a shallow clone.
        void backUpValue();

        /// Clone the value, and return a modifiable value to the value at the given path.
        /// If the type is compound, this asserts that the ValueTreeNode has been backed up already.
        // TODO Find better name.
        ValueHolder& setModifiable(const Path& pathFromHere);

        /// After changes are complete, compare the current value to the backup and set change flags.
        /// This clears the backup.
        void reconcileChangesFromBackup();

        /// Get the TypeSystem carried by this root.
        const TypeSystem& getTypeSystem() const;

      protected:
        const ValueHolder& doGetValue() const override;
        void doSetToDefault() override;
        void doSetValue(const ValueHolder& newValue) override;

      private:
        ValueHolder m_value;

        /// A backup of the value before modification.
        // This could be managed externally, but it is kept here to ensure setModifiable is only
        // called by code which knows how to manage a back-up.
        ValueHolder m_valueBackUp;

        /// Roots carry a reference to the typesystem.
        const TypeSystem* m_typeSystem = nullptr;

        // TODO: Temporary hack (hopefully): This allows values to be modified without requiring a backup.
        // _Project_ code which modifies features should be aware of the need to back-up the value,
        // but client code (e.g. in a source format) should need to bother with this.
        // I'm uncertain yet about client code in processors.
        bool m_isNew = true;
    };

    struct BackupScope {
        BackupScope(ValueTreeRoot& valueTree)
            : m_backedUpValueTree(valueTree) {
            valueTree.backUpValue();
        }

        ~BackupScope() { m_backedUpValueTree.reconcileChangesFromBackup(); }

        ValueTreeRoot& m_backedUpValueTree;
    };

} // namespace babelwires
