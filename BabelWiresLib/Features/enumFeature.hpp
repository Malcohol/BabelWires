/**
 * An EnumFeature exposes an enum in the project, and allows the user to make a choice from a set of named values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Identifiers/identifier.hpp"
#include "BabelWiresLib/Features/features.hpp"

#include <vector>

namespace babelwires {
    class Enum;

    /// A feature offering a choice from a set of named values.
    /// The user should care only about the selected value, and not about the particular index, which is
    /// an implementation detail and could change in future.
    /// If the mapping between names and their index is meaningful to the user, consider using an IntFeature
    /// with ValueNames.
    /// The particular advantage of an EnumFeature is that values can be reordered and new values can be added
    /// without requiring versioning code to adapt old serialized data.
    class EnumFeature : public ValueFeature {
      public:
        EnumFeature(const Enum& e);

        const Enum& getEnum() const;

        /// Get the current value of the feature.
        Identifier get() const;

        /// Set the value in the feature.
        void set(Identifier value);

      protected:
        std::string doGetValueType() const override;
        void doAssign(const ValueFeature& other) override;
        void doSetToDefault() override;
        std::size_t doGetHash() const override;

      protected:
        ///
        const Enum& m_enum;

        ///
        Identifier m_value;
    };

    /// EnumFeature for RegisteredEnums, which can be conveniently constructed, since it assumes the Enum it
    /// references has been registered in the EnumRegistry.
    template <typename E> class RegisteredEnumFeature : public EnumFeature {
      public:
        RegisteredEnumFeature()
            : EnumFeature(*E::getRegisteredInstance()) {}
    };

} // namespace babelwires
