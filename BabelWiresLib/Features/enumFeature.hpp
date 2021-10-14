/**
 * An EnumFeature exposes an enum in the project.
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

    /// EnumFeature for RegisteredEnums, which can be conveniently constructed, since it assumes the Enum it
    /// references has been registered in the EnumRegistry.
    template <typename E> class RegisteredEnumFeature : public EnumFeature {
      public:
        RegisteredEnumFeature()
            : EnumFeature(*E::getRegisteredInstance()) {}
    };

} // namespace babelwires
