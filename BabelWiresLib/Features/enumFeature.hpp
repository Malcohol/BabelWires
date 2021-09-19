/**
 * An EnumFeature exposes an enum in the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Features/Path/fieldIdentifier.hpp"
#include "BabelWiresLib/Features/features.hpp"

#include <vector>

namespace babelwires {
    class Enum;

    /// A feature offering a choice from a set of named values.
    /// Note: This differs from ValueNames which allow names or hints to be given to specific integer values.
    /// TODO: Do we still need ValueNames?
    class EnumFeature : public ValueFeature {
      public:
        EnumFeature(const Enum& e);

        const Enum& getEnum() const;

        /// Get the current value of the feature.
        FieldIdentifier get() const;

        /// Set the value in the feature.
        void set(FieldIdentifier value);

      protected:
        std::string doGetValueType() const override;
        void doAssign(const ValueFeature& other) override;
        void doSetToDefault() override;
        std::size_t doGetHash() const override;

      protected:
        ///
        const Enum& m_enum;

        ///
        FieldIdentifier m_value;
    };

    /// This provides convenience functions which allow the contained value to be queried or set from
    /// the corresponding native enum (assuming the latter was defined with ENUM_DEFINE_CPP_ENUM).
    template <typename E> class EnumFeatureImpl : public EnumFeature {
      public:
        EnumFeatureImpl()
            : EnumFeature(*E::getRegisteredInstance()) {}

        /// Get the stored value as a native enum value.
        typename E::Value getAsValue() const { return static_cast<const E&>(m_enum).getValueFromIdentifier(m_value); }

        /// Set the value using a native enum value.
        void setFromValue(typename E::Value value) {
            set(static_cast<const E*>(m_enum)->getIdentifierFromValue(value));
        }
    };

} // namespace babelwires
