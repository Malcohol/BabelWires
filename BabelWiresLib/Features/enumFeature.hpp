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
    /// TODO: Do we still need value names?
    class EnumFeature : public ValueFeature {
      public:
        EnumFeature(const Enum& e);

        const Enum& getEnum() const;

        FieldIdentifier get() const;
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

    template <typename E> class EnumFeatureImpl : public EnumFeature {
      public:
        EnumFeatureImpl()
            : EnumFeature(*E::getRegisteredInstance()) {}

        typename E::Value getAsValue() const { return static_cast<const E&>(m_enum).getValueFromIdentifier(m_value); }

        void setFromValue(typename E::Value value) {
            set(static_cast<const E*>(m_enum)->getIdentifierFromValue(value));
        }
    };

} // namespace babelwires
