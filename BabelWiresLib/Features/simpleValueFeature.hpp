/**
 * A SimpleValueFeature is a ValueFeature which owns its value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

namespace babelwires {
    class Type;
    class Value;

    /// A SimpleValueFeature is a feature which owns its value.
    class SimpleValueFeature : public ValueFeature {
      public:
        /// Construct a ValueFeature which carries values of the given type.
        SimpleValueFeature(TypeRef typeRef);

        /// Back up the current value using a shallow clone.
        void backUpValue();

        /// Clone the value, and return a modifiable value to the value at the given path.
        /// If the type is compound, this asserts that the feature has been backed up already.
        // TODO Find better name.
        ValueHolder& setModifiable(const FeaturePath& pathFromHere);

        /// After changes are complete, compare the current value to the backup and set change flags.
        /// This clears the backup.
        void reconcileChangesFromBackup();

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

        // TODO: Temporary hack (hopefully): This allows values to be modified without requiring a backup.
        // _Project_ code which modifies features should be aware of the need to back-up the value,
        // but client code (e.g. in a source format) should need to bother with this.
        // I'm uncertain yet about client code in processors.
        bool m_isNew = true;
    };
} // namespace babelwires
